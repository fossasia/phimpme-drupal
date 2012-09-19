<?php

/**
 * @file
 * Provide views data for modules making use of the entity CRUD API.
 */

/**
 * Implements hook_views_data().
 *
 * Provides Views integration for entities if they satisfy one of these
 * conditions:
 *  - hook_entity_info() specifies a 'views controller class' key.
 *  - hook_entity_info() specifies a 'module' key, and the module does not
 *    implement hook_views_data().
 *
 * @see entity_crud_hook_entity_info()
 * @see entity_views_table_definition()
 */
function entity_views_data() {
  $data = array();

  foreach (entity_crud_get_info() as $type => $info) {
    // Provide default integration with the basic controller class if we know
    // the module providing the entity and it does not provide views integration.
    if (!isset($info['views controller class'])) {
      $info['views controller class'] = isset($info['module']) && !module_hook($info['module'], 'views_data') ? 'EntityDefaultViewsController' : FALSE;
    }
    if ($info['views controller class']) {
      $controller = new $info['views controller class']($type);
      // Relationship data may return views data for already existing tables,
      // so merge results on the second level.
      foreach ($controller->views_data() as $table => $table_data) {
        $data += array($table => array());
        $data[$table] = array_merge($data[$table], $table_data);
      }
    }
  }

  // Add tables based upon data selection "queries" for all entity types.
  foreach (entity_get_info() as $type => $info) {
    $table = entity_views_table_definition($type);
    if ($table) {
      $data['entity_' . $type] = $table;
    }
  }

  // Expose generally usable entity-related fields.
  foreach (entity_get_info() as $entity_type => $info) {
    if (entity_type_supports($entity_type, 'view')) {
      // Expose a field allowing to display the rendered entity.
      $data['views_entity_' . $entity_type]['rendered_entity'] = array(
        'title' => t('Rendered @entity-type', array('@entity-type' => $info['label'])),
        'help' => t('The @entity-type of the current relationship rendered using a view mode.', array('@entity-type' => $info['label'])),
        'field' => array(
          'handler' => 'entity_views_handler_field_entity',
          'type' => $entity_type,
          // The EntityFieldHandlerHelper treats the 'entity object' data
          // selector as special case for loading the base entity.
          'real field' => 'entity object',
        ),
      );
    }
  }

  return $data;
}

/**
 * Helper function for getting data selection based entity Views table definitions.
 *
 * This creates extra tables for each entity type that are not associated with a
 * query plugin (and thus are not base tables) and just rely on the entities to
 * retrieve the displayed data. To obtain the entities corresponding to a
 * certain result set, the field handlers defined on the table use a generic
 * interface defined for query plugins that are based on entity handling, and
 * which is described in the entity_views_example_query class.
 *
 * These tables are called "data selection tables".
 *
 * Other modules providing Views integration with new query plugins that are
 * based on entities can then use these tables as a base for their own tables
 * (by directly using this method and modifying the returned table) and/or by
 * specifying relationships to them. The tables returned here already specify
 * relationships to each other wherever an entity contains a reference to
 * another (e.g., the node author constructs a relationship from nodes to
 * users).
 *
 * As filtering and other query manipulation is potentially more plugin-specific
 * than the display, only field handlers and relationships are provided with
 * these tables. By providing a add_selector_orderby() method, the query plugin
 * can, however, support click-sorting for the field handlers in these tables.
 *
 * For a detailed discussion see http://drupal.org/node/1266036
 *
 * For example use see the Search API views module in the Search API project:
 * http://drupal.org/project/search_api
 *
 * @param $type
 *   The entity type whose table definition should be returned.
 *
 * @return
 *   An array containing the data selection Views table definition for the
 *   entity type.
 *
 * @see entity_views_field_definition()
 */
function entity_views_table_definition($type) {
  // As other modules might want to copy these tables as a base for their own
  // Views integration, we statically cache the tables to save some time.
  $tables = &drupal_static(__FUNCTION__, array());

  if (!isset($tables[$type])) {
    // Work-a-round to fix updating, see http://drupal.org/node/1330874.
    // Views data might be rebuilt on update.php before the registry is rebuilt,
    // thus the class cannot be auto-loaded.
    if (!class_exists('EntityFieldHandlerHelper')) {
      module_load_include('inc', 'entity', 'views/handlers/entity_views_field_handler_helper');
    }

    $info = entity_get_info($type);
    $tables[$type]['table'] = array(
      'group' => $info['label'],
      'entity type' => $type,
    );
    foreach (entity_get_all_property_info($type) as $key => $property) {
      entity_views_field_definition($key, $property, $tables[$type]);
    }
  }

  return $tables[$type];
}

/**
 * Helper function for adding a Views field definition to data selection based Views tables.
 *
 * @param $field
 *   The data selector of the field to add. E.g. "title" would derive the node
 *   title property, "body:summary" the node body's summary.
 * @param array $property_info
 *   The property information for which to create a field definition.
 * @param array $table
 *   The table into which the definition should be inserted.
 * @param $title_prefix
 *   Internal use only.
 *
 * @see entity_views_table_definition()
 */
function entity_views_field_definition($field, array $property_info, array &$table, $title_prefix = '') {
  $additional = array();
  $additional_field = array();

  // Create a valid Views field identifier (no colons, etc.). Keep the original
  // data selector as real field though.
  $key = _entity_views_field_identifier($field, $table);
  if ($key != $field) {
    $additional['real field'] = $field;
  }
  $field_name = EntityFieldHandlerHelper::get_selector_field_name($field);

  $field_handlers = entity_views_get_field_handlers();

  $property_info += entity_property_info_defaults();
  $type = entity_property_extract_innermost_type($property_info['type']);
  $title = $title_prefix . $property_info['label'];
  if ($info = entity_get_info($type)) {
    $additional['relationship'] = array(
      'handler' => $field_handlers['relationship'],
      'base' => 'entity_' . $type,
      'base field' => $info['entity keys']['id'],
      'relationship field' => $field,
      'label' => $title,
    );
    if ($property_info['type'] != $type) {
      // This is a list of entities, so we should mark the relationship as such.
      $additional['relationship']['multiple'] = TRUE;
    }
    // Implementers of the field handlers alter hook could add handlers for
    // specific entity types.
    if (!isset($field_handlers[$type])) {
      $type = 'entity';
    }
  }
  elseif (!empty($property_info['field'])) {
    $type = 'field';
    // Views' Field API field handler needs some extra definitions to work.
    $additional_field['field_name'] = $field_name;
    $additional_field['entity_tables'] = array();
    $additional_field['entity type'] = $table['table']['entity type'];
    $additional_field['is revision'] = FALSE;
  }
  // Copied from EntityMetadataWrapper::optionsList()
  elseif (isset($property_info['options list']) && is_callable($property_info['options list'])) {
    // If this is a nested property, we need to get rid of all prefixes first.
    $type = 'options';
    $additional_field['options callback'] = array(
      'function' => $property_info['options list'],
      'info' => $property_info,
    );
  }
  elseif ($type == 'decimal') {
    $additional_field['float'] = TRUE;
  }

  if (isset($field_handlers[$type])) {
    $table += array($key => array());
    $table[$key] += array(
      'title' => $title,
      'help' => empty($property_info['description']) ? t('(No information available)') : $property_info['description'],
      'field' => array(),
    );
    $table[$key]['field'] += array(
      'handler' => $field_handlers[$type],
      'type' => $property_info['type'],
    );
    $table[$key] += $additional;
    $table[$key]['field'] += $additional_field;
  }
  if (!empty($property_info['property info'])) {
    foreach ($property_info['property info'] as $nested_key => $nested_property) {
      entity_views_field_definition($field . ':' . $nested_key, $nested_property, $table, $title . ' » ');
    }
  }
}

/**
 * @return array
 *   The handlers to use for the data selection based Views tables.
 *
 * @see hook_entity_views_field_handlers_alter()
 */
function entity_views_get_field_handlers() {
  $field_handlers = drupal_static(__FUNCTION__);
  if (!isset($field_handlers)) {
    // Field handlers for the entity tables, by type.
    $field_handlers = array(
      'text'         => 'entity_views_handler_field_text',
      'token'        => 'entity_views_handler_field_text',
      'integer'      => 'entity_views_handler_field_numeric',
      'decimal'      => 'entity_views_handler_field_numeric',
      'date'         => 'entity_views_handler_field_date',
      'duration'     => 'entity_views_handler_field_duration',
      'boolean'      => 'entity_views_handler_field_boolean',
      'uri'          => 'entity_views_handler_field_uri',
      'options'      => 'entity_views_handler_field_options',
      'field'        => 'entity_views_handler_field_field',
      'entity'       => 'entity_views_handler_field_entity',
      'relationship' => 'entity_views_handler_relationship',
    );
    drupal_alter('entity_views_field_handlers', $field_handlers);
  }
  return $field_handlers;
}

/**
 * Helper function for creating valid Views field identifiers out of data selectors.
 *
 * Uses $table to test whether the identifier is already used, and also
 * recognizes if a definition for the same field is already present and returns
 * that definition's identifier.
 *
 * @return string
 *   A valid Views field identifier that is not yet used as a key in $table.
 */
function _entity_views_field_identifier($field, array $table) {
  $key = $base = preg_replace('/[^a-zA-Z0-9]+/S', '_', $field);
  $i = 0;
  // The condition checks whether this sanitized field identifier is already
  // used for another field in this table (and whether the identifier is
  // "table", which can never be used).
  // If $table[$key] is set, the identifier is already used, but this might be
  // already for the same field. To test that, we need the original field name,
  // which is either $table[$key]['real field'], if set, or $key. If this
  // original field name is equal to $field, we can use that key. Otherwise, we
  // append numeric suffixes until we reach an unused key.
  while ($key == 'table' || (isset($table[$key]) && (isset($table[$key]['real field']) ? $table[$key]['real field'] : $key) != $field)) {
    $key = $base . '_' . ++$i;
  }
  return $key;
}

/**
 * Implements hook_views_plugins().
 */
function entity_views_plugins() {
  foreach (views_fetch_data() as $table => $data) {
    if (!empty($data['table']['entity type']) && !empty($data['table']['base'])) {
      $base_tables[] = $table;
    }
  }
  if (!empty($base_tables)) {
    return array(
      'module' => 'entity',
      'row' => array(
        'entity' => array(
          'title' => t('Rendered entity'),
          'help' => t('Renders a single entity in a specific view mode (e.g. teaser).'),
          'handler' => 'entity_views_plugin_row_entity_view',
          'uses fields' => FALSE,
          'uses options' => TRUE,
          'type' => 'normal',
          'base' => $base_tables,
        ),
      ),
    );
  }
}

/**
 * Default controller for generating basic views integration.
 *
 * The controller tries to generate suiting views integration for the entity
 * based upon the schema information of its base table and the provided entity
 * property information.
 * For that it is possible to map a property name to its schema/views field
 * name by adding a 'schema field' key with the name of the field as value to
 * the property info.
 */
class EntityDefaultViewsController {

  protected $type, $info, $relationships;

  public function __construct($type) {
    $this->type = $type;
    $this->info = entity_get_info($type);
  }

  /**
   * Defines the result for hook_views_data().
   */
  public function views_data() {
    $data = array();
    $this->relationships = array();

    if (!empty($this->info['base table'])) {
      $table = $this->info['base table'];
      // Define the base group of this table. Fields that don't
      // have a group defined will go into this field by default.
      $data[$table]['table']['group']  = drupal_ucfirst($this->info['label']);
      $data[$table]['table']['entity type'] = $this->type;

      $data[$table]['table']['base'] = array(
        'field' => $this->info['entity keys']['id'],
        'title' => drupal_ucfirst($this->info['label']),
        'help' => isset($this->info['description']) ? $this->info['description'] : '',
      );
      $data[$table]['table']['entity type'] = $this->type;
      $data[$table] += $this->schema_fields();

      // Add in any reverse-relationships which have been determined.
      $data += $this->relationships;
    }
    return $data;
  }

  /**
   * Try to come up with some views fields with the help of the schema and
   * the entity property information.
   */
  protected function schema_fields() {
    $schema = drupal_get_schema($this->info['base table']);
    $properties = entity_get_property_info($this->type) + array('properties' => array());
    $data = array();

    foreach ($properties['properties'] as $name => $property_info) {
      if (isset($property_info['schema field']) && isset($schema['fields'][$property_info['schema field']])) {
        if ($views_info = $this->map_from_schema_info($name, $schema['fields'][$property_info['schema field']], $property_info)) {
          $data[$name] = $views_info;
        }
      }
    }
    return $data;
  }

  /**
   * Comes up with views information based on the given schema and property
   * info.
   */
  protected function map_from_schema_info($property_name, $schema_field_info, $property_info) {
    $type = isset($property_info['type']) ? $property_info['type'] : 'text';
    $views_field_name = $property_info['schema field'];

    $return = array();

    if (!empty($schema_field_info['serialize'])) {
      return FALSE;
    }

    $description = array(
      'title' => $property_info['label'],
      'help' => $property_info['description'],
    );

      // Add in relationships to related entities.
    if (($info = entity_get_info($type)) && !empty($info['base table'])) {

      // Prepare reversed relationship data.
      $label_lowercase = drupal_strtolower($this->info['label'][0]) . drupal_substr($this->info['label'], 1);
      $property_label_lowercase = drupal_strtolower($property_info['label'][0]) . drupal_substr($property_info['label'], 1);

      // We name the field of the first reverse-relationship just with the
      // base table to be backward compatible, for subsequents relationships we
      // append the views field name in order to get a unique name.
      $name = !isset($this->relationships[$info['base table']][$this->info['base table']]) ? $this->info['base table'] : $this->info['base table'] . '_' . $views_field_name;
      $this->relationships[$info['base table']][$name] = array(
        'title' => $this->info['label'],
        'help' => t("Associated @label via the @label's @property.", array('@label' => $label_lowercase, '@property' => $property_label_lowercase)),
        'relationship' => array(
          'label' => $this->info['label'],
          'handler' => $this->getRelationshipHandlerClass($this->type, $type),
          'base' => $this->info['base table'],
          'base field' => $views_field_name,
          'relationship field' => isset($info['entity keys']['name']) ? $info['entity keys']['name'] : $info['entity keys']['id'],
        ),
      );

      $return['relationship'] = array(
        'label' => drupal_ucfirst($info['label']),
        'handler' => $this->getRelationshipHandlerClass($type, $this->type),
        'base' => $info['base table'],
        'base field' => isset($info['entity keys']['name']) ? $info['entity keys']['name'] : $info['entity keys']['id'],
        'relationship field' => $views_field_name,
      );

      // Add in direct field/filters/sorts for the id itself too.
      $type = isset($info['entity keys']['name']) ? 'token' : 'integer';
      // Append the views-field-name to the title if it is different to the
      // property name.
      if ($property_name != $views_field_name) {
        $description['title'] .= ' ' . $views_field_name;
      }
    }

    switch ($type) {
      case 'token':
      case 'text':
        $return += $description + array(
          'field' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
           ),
          'sort' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_sort',
          ),
          'filter' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_filter_string',
          ),
          'argument' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_argument_string',
          ),
        );
      break;

      case 'decimal':
      case 'integer':
        $return += $description + array(
          'field' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_field_numeric',
            'click sortable' => TRUE,
            'float' => ($type == 'decimal'),
           ),
          'sort' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_sort',
          ),
          'filter' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_filter_numeric',
          ),
          'argument' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_argument_numeric',
          ),
        );
      break;

      case 'date':
        $return += $description + array(
          'field' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_field_date',
            'click sortable' => TRUE,
           ),
          'sort' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_sort_date',
          ),
          'filter' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_filter_date',
          ),
          'argument' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_argument_date',
          ),
        );
      break;

      case 'uri':
        $return += $description + array(
          'field' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_field_url',
            'click sortable' => TRUE,
           ),
          'sort' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_sort',
          ),
          'filter' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_filter_string',
          ),
          'argument' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_argument_string',
          ),
        );
      break;

      case 'boolean':
        $return += $description + array(
          'field' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_field_boolean',
            'click sortable' => TRUE,
           ),
          'sort' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_sort',
          ),
          'filter' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_filter_boolean_operator',
          ),
          'argument' => array(
            'real field' => $views_field_name,
            'handler' => 'views_handler_argument_string',
          ),
        );
      break;
    }

    // If there is an options list callback, add to the filter.
    if (isset($return['filter']) && !empty($property_info['options list'])) {
      $return['filter']['handler'] = 'views_handler_filter_in_operator';
      $return['filter']['options callback'] = array('EntityDefaultViewsController', 'optionsListCallback');
      $return['filter']['options arguments'] = array($this->type, $property_name, 'view');
    }
    return $return;
  }

  /**
   * Determines the handler to use for a relationship to an entity type.
   *
   * @param $entity_type
   *   The entity type to join to.
   * @param $left_type
   *   The data type from which to join.
   */
  function getRelationshipHandlerClass($entity_type, $left_type) {
    // Look for an entity type which is used as bundle for the given entity
    // type. If there is one, allow filtering the relation by bundle by using
    // our own handler.
    foreach (entity_get_info() as $type => $info) {
      // In case we already join from the bundle entity we do not need to filter
      // by bundle entity any more, so we stay with the general handler.
      if (!empty($info['bundle of']) && $info['bundle of'] == $entity_type && $type != $left_type) {
        return 'entity_views_handler_relationship_by_bundle';
      }
    }
    return 'views_handler_relationship';
  }

  /**
   * A callback returning property options, suitable to be used as views options callback.
   */
  public static function optionsListCallback($type, $selector, $op = 'view') {
    $wrapper = entity_metadata_wrapper($type, NULL);
    $parts = explode(':', $selector);
    foreach ($parts as $part) {
      $wrapper = $wrapper->get($part);
    }
    return $wrapper->optionsList($op);
  }
}
