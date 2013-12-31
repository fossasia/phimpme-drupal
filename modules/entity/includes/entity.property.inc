<?php

/**
 * @file
 * Provides API functions around hook_entity_property_info(). Also see
 * entity.info.inc, which cares for providing entity property info for all core
 * entity types.
 */

/**
 * Get the entity property info array of an entity type.
 *
 * @param $entity_type
 *   The entity type, e.g. node, for which the info shall be returned, or NULL
 *   to return an array with info about all types.
 *
 * @see hook_entity_property_info()
 * @see hook_entity_property_info_alter()
 */
function entity_get_property_info($entity_type = NULL) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static = array();
  isset($drupal_static[__FUNCTION__]) || ($drupal_static[__FUNCTION__] = &drupal_static(__FUNCTION__));
  $info = &$drupal_static[__FUNCTION__];

  // hook_entity_property_info() includes translated strings, so each language
  // is cached separately.
  $langcode = $GLOBALS['language']->language;

  if (empty($info)) {
    if ($cache = cache_get("entity_property_info:$langcode")) {
      $info = $cache->data;
    }
    else {
      $info = module_invoke_all('entity_property_info');
      // Let other modules alter the entity info.
      drupal_alter('entity_property_info', $info);
      cache_set("entity_property_info:$langcode", $info);
    }
  }
  return empty($entity_type) ? $info : (isset($info[$entity_type]) ? $info[$entity_type] : array());
}

/**
 * Returns the default information for an entity property.
 *
 * @return
 *   An array of optional property information keys mapped to their defaults.
 *
 * @see hook_entity_property_info()
 */
function entity_property_info_defaults() {
  return array(
    'type' => 'text',
    'getter callback' => 'entity_property_verbatim_get',
  );
}

/**
 * Gets an array of info about all properties of a given entity type.
 *
 * In contrast to entity_get_property_info(), this function returns info about
 * all properties the entity might have, thus it adds an all properties assigned
 * to entity bundles.
 *
 * @param $entity_type
 *   (optiona) The entity type to return properties for.
 *
 * @return
 *   An array of info about properties. If the type is ommitted, all known
 *   properties are returned.
 */
function entity_get_all_property_info($entity_type = NULL) {
  if (!isset($entity_type)) {
    // Retrieve all known properties.
    $properties = array();
    foreach (entity_get_info() as $entity_type => $info) {
      $properties += entity_get_all_property_info($entity_type);
    }
    return $properties;
  }
  // Else retrieve the properties of the given entity type only.
  $info = entity_get_property_info($entity_type);
  $info += array('properties' => array(), 'bundles' => array());
  // Add all bundle properties.
  foreach ($info['bundles'] as $bundle => $bundle_info) {
    $bundle_info += array('properties' => array());
    $info['properties'] += $bundle_info['properties'];
  }
  return $info['properties'];
}

/**
 * Queries for entities having the given property value.
 *
 * @param $entity_type
 *   The type of the entity.
 * @param $property
 *   The name of the property to query for.
 * @param $value
 *   A single property value or an array of possible values to query for.
 * @param $limit
 *   Limit the numer of results. Defaults to 30.
 *
 * @return
 *   An array of entity ids or NULL if there is no information how to query for
 *   the given property.
 */
function entity_property_query($entity_type, $property, $value, $limit = 30) {
  $properties = entity_get_all_property_info($entity_type);
  $info = $properties[$property] + array('type' => 'text', 'queryable' => !empty($properties[$property]['schema field']));

  // We still support the deprecated query callback, so just add in EFQ-based
  // callbacks in case 'queryable' is set to TRUE and make use of the callback.
  if ($info['queryable'] && empty($info['query callback'])) {
    $info['query callback'] = !empty($info['field']) ? 'entity_metadata_field_query' : 'entity_metadata_table_query';
  }

  $type = $info['type'];
  // Make sure an entity or a list of entities are passed on as identifiers
  // with the help of the wrappers. For that ensure the data type matches the
  // passed on value(s).
  if (is_array($value) && !entity_property_list_extract_type($type)) {
    $type = 'list<' . $type . '>';
  }
  elseif (!is_array($value) && entity_property_list_extract_type($type)) {
    $type = entity_property_list_extract_type($type);
  }

  $wrapper = entity_metadata_wrapper($type, $value);
  $value = $wrapper->value(array('identifier' => TRUE));

  if (!empty($info['query callback'])) {
    return $info['query callback']($entity_type, $property, $value, $limit);
  }
}

/**
 * Resets the cached information of hook_entity_property_info().
 */
function entity_property_info_cache_clear() {
  drupal_static_reset('entity_get_property_info');
  // Clear all languages.
  cache_clear_all('entity_property_info:', 'cache', TRUE);
}

/**
 * Implements hook_hook_info().
 */
function entity_hook_info() {
  $hook_info['entity_property_info'] = array(
    'group' => 'info',
  );
  return $hook_info;
}

/**
 * Implements hook_field_info_alter().
 * Defines default property types for core field types.
 */
function entity_field_info_alter(&$field_info) {
  if (module_exists('number')) {
    $field_info['number_integer']['property_type'] = 'integer';
    $field_info['number_decimal']['property_type'] = 'decimal';
    $field_info['number_float']['property_type'] = 'decimal';
  }
  if (module_exists('text')) {
    $field_info['text']['property_type'] = 'text';
    $field_info['text']['property_callbacks'][] = 'entity_metadata_field_text_property_callback';
    $field_info['text_long']['property_type'] = 'text';
    $field_info['text_long']['property_callbacks'][] = 'entity_metadata_field_text_property_callback';
    $field_info['text_with_summary']['property_type'] = 'field_item_textsummary';
    $field_info['text_with_summary']['property_callbacks'][] = 'entity_metadata_field_text_property_callback';
  }
  if (module_exists('list')) {
    $field_info['list_integer']['property_type'] = 'integer';
    $field_info['list_boolean']['property_type'] = 'boolean';
    $field_info['list_float']['property_type'] = 'decimal';
    $field_info['list_text']['property_type'] = 'text';
  }
  if (module_exists('taxonomy')) {
    $field_info['taxonomy_term_reference']['property_type'] = 'taxonomy_term';
    $field_info['taxonomy_term_reference']['property_callbacks'][] = 'entity_metadata_field_term_reference_callback';
  }
  if (module_exists('file')) {
    // The callback specifies a custom data structure matching the file field
    // items. We introduce a custom type name for this data structure.
    $field_info['file']['property_type'] = 'field_item_file';
    $field_info['file']['property_callbacks'][] = 'entity_metadata_field_file_callback';
  }
  if (module_exists('image')) {
    // The callback specifies a custom data structure matching the image field
    // items. We introduce a custom type name for this data structure.
    $field_info['image']['property_type'] = 'field_item_image';
    $field_info['image']['property_callbacks'][] = 'entity_metadata_field_file_callback';
    $field_info['image']['property_callbacks'][] = 'entity_metadata_field_image_callback';
  }
}

/**
 * Implements hook_field_create_instance().
 * Clear the cache when a field instance changed.
 */
function entity_field_create_instance() {
  entity_property_info_cache_clear();
}

/**
 * Implements hook_field_delete_instance().
 * Clear the cache when a field instance changed.
 */
function entity_field_delete_instance() {
  entity_property_info_cache_clear();
}

/**
 * Implements hook_field_update_instance().
 * Clear the cache when a field instance changed.
 */
function entity_field_update_instance() {
  entity_property_info_cache_clear();
}

/**
 * Verifies that the given data can be safely used as the given type regardless
 * of the PHP variable type of $data. Example: the string "15" is a valid
 * integer, but "15nodes" is not.
 *
 * @return
 *   Whether the data is valid for the given type.
 */
function entity_property_verify_data_type($data, $type) {
  // First off check for entities, which may be represented by their ids too.
  $info = entity_get_info();
  if (isset($info[$type])) {
    $nameKey = isset($info[$type]['entity keys']['name']);
    return is_object($data) || ($nameKey && entity_property_verify_data_type($data, 'token')) || (!$nameKey  && entity_property_verify_data_type($data, 'integer'));
  }

  switch ($type) {
    case 'site':
    case 'unknown':
      return TRUE;
    case 'date':
    case 'duration':
    case 'integer':
      return is_numeric($data) && strpos($data, '.') === FALSE;
    case 'decimal':
      return is_numeric($data);
    case 'text':
      return is_scalar($data);
    case 'token':
      return is_scalar($data) && preg_match('!^[a-z][a-z0-9_]*$!', $data);
    case 'boolean':
      return is_scalar($data) && (is_bool($data) || $data == 0 || $data == 1);
    case 'uri':
      return valid_url($data, TRUE);
    case 'list':
      return (is_array($data) && array_values($data) == $data) || (is_object($data) && $data instanceof EntityMetadataArrayObject);
    case 'entity':
      return is_object($data) && $data instanceof EntityDrupalWrapper;
    default:
    case 'struct':
      return is_object($data) || is_array($data);
  }
}

/**
 * Creates the entity object for an array of given property values.
 *
 * @param $entity_type
 *   The entity type to create an entity for.
 * @param $values
 *   An array of values as described by the entity's property info. All entity
 *   properties of the given entity type that are marked as required, must be
 *   present.
 *   If the passed values have no matching property, their value will be
 *   assigned to the entity directly, without the use of the metadata-wrapper
 *   property.
 *
 * @return EntityDrupalWrapper
 *   An EntityDrupalWrapper wrapping the newly created entity or FALSE, if
 *   there were no information how to create the entity.
 */
function entity_property_values_create_entity($entity_type, $values = array()) {
  if (entity_type_supports($entity_type, 'create')) {
    $info = entity_get_info($entity_type);
    // Create the initial entity by passing the values for all 'entity keys'
    // to entity_create().
    $entity_keys = array_filter($info['entity keys']);
    $creation_values = array_intersect_key($values, array_flip($entity_keys));

    // In case the bundle key does not match the property that sets it, ensure
    // the bundle key is initialized somehow, so entity_extract_ids()
    // does not bail out during wrapper creation.
    if (!empty($info['entity keys']['bundle'])) {
      $creation_values += array($info['entity keys']['bundle'] => FALSE);
    }
    $entity = entity_create($entity_type, $creation_values);

    // Now set the remaining values using the wrapper.
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    foreach ($values as $key => $value) {
      if (!in_array($key, $info['entity keys'])) {
        if (isset($wrapper->$key)) {
          $wrapper->$key->set($value);
        }
        else {
          $entity->$key = $value;
        }
      }
    }
    // @todo: Once we require Drupal 7.7 or later, verify the entity has
    // now a valid bundle and throw the EntityMalformedException if not.
    return $wrapper;
  }
  return FALSE;
}


/**
 * Extracts the contained type for a list type string like list<date>.
 *
 * @return
 *   The contained type or FALSE, if the given type string is no list.
 */
function entity_property_list_extract_type($type) {
  if (strpos($type, 'list<') === 0 && $type[strlen($type)-1] == '>') {
    return substr($type, 5, -1);
  }
  return FALSE;
}

/**
 * Extracts the innermost type for a type string like list<list<date>>.
 *
 * @param $type
 *   The type to examine.
 *
 * @return
 *   For list types, the innermost type. The type itself otherwise.
 */
function entity_property_extract_innermost_type($type) {
  while (strpos($type, 'list<') === 0 && $type[strlen($type)-1] == '>') {
    $type = substr($type, 5, -1);
  }
  return $type;
}

/**
 * Gets the property just as it is set in the data.
 */
function entity_property_verbatim_get($data, array $options, $name, $type, $info) {
  $name = isset($info['schema field']) ? $info['schema field'] : $name;
  if ((is_array($data) || (is_object($data) && $data instanceof ArrayAccess)) && isset($data[$name])) {
    return $data[$name];
  }
  elseif (is_object($data) && isset($data->$name)) {
    return $data->$name;
  }
  return NULL;
}

/**
 * Date values are converted from ISO strings to timestamp if needed.
 */
function entity_property_verbatim_date_get($data, array $options, $name, $type, $info) {
  $name = isset($info['schema field']) ? $info['schema field'] : $name;
  return is_numeric($data[$name]) ? $data[$name] : strtotime($data[$name], time());
}

/**
 * Sets the property to the given value. May be used as 'setter callback'.
 */
function entity_property_verbatim_set(&$data, $name, $value, $langcode, $type, $info) {
  $name = isset($info['schema field']) ? $info['schema field'] : $name;
  if (is_array($data) || (is_object($data) && $data instanceof ArrayAccess)) {
    $data[$name] = $value;
  }
  elseif (is_object($data)) {
    $data->$name = $value;
  }
}

/**
 * Gets the property using the getter method (named just like the property).
 */
function entity_property_getter_method($object, array $options, $name) {
  // Remove any underscores as classes are expected to use CamelCase.
  $method = strtr($name, array('_' => ''));
  return $object->$method();
}

/**
 * Sets the property to the given value using the setter method. May be used as
 * 'setter callback'.
 */
function entity_property_setter_method($object, $name, $value) {
  // Remove any underscores as classes are expected to use CamelCase.
  $method = 'set' . strtr($name, array('_' => ''));
  // Invoke the setProperty() method where 'Property' is the property name.
  $object->$method($value);
}

/**
 * Getter callback for getting an array. Makes sure it's numerically indexed.
 */
function entity_property_get_list($data, array $options, $name) {
  return isset($data->$name) ? array_values($data->$name) : array();
}

/**
 * A validation callback ensuring the passed integer is positive.
 */
function entity_property_validate_integer_positive($value) {
  return $value > 0;
}

/**
 * A validation callback ensuring the passed integer is non-negative.
 */
function entity_property_validate_integer_non_negative($value) {
  return $value >= 0;
}

/**
 * A simple auto-creation callback for array based data structures.
 */
function entity_property_create_array($property_name, $context) {
  return array();
}

/**
 * Flattens the given options in single dimensional array.
 * We don't depend on options module, so we cannot use options_array_flatten().
 *
 * @see options_array_flatten()
 */
function entity_property_options_flatten($options) {
  $result = array();
  foreach ($options as $key => $value) {
    if (is_array($value)) {
      $result += $value;
    }
    else {
      $result[$key] = $value;
    }
  }
  return $result;
}

/**
 * Defines info for the properties of the text_formatted data structure.
 */
function entity_property_text_formatted_info() {
  return array(
    'value' => array(
      'type' => 'text',
      'label' => t('Text'),
      'sanitized' => TRUE,
      'getter callback' => 'entity_metadata_field_text_get',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer nodes',
      'raw getter callback' => 'entity_property_verbatim_get',
    ),
    'summary' => array(
      'type' => 'text',
      'label' => t('Summary'),
      'sanitized' => TRUE,
      'getter callback' => 'entity_metadata_field_text_get',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer nodes',
      'raw getter callback' => 'entity_property_verbatim_get',
    ),
    'format' => array(
      'type' => 'token',
      'label' => t('Text format'),
      'options list' => 'entity_metadata_field_text_formats',
      'getter callback' => 'entity_property_verbatim_get',
    ),
  );
}

/**
 * Defines info for the properties of the field_item_textsummary data structure.
 */
function entity_property_field_item_textsummary_info() {
  return array(
    'value' => array(
      'type' => 'text',
      'label' => t('Text'),
      'setter callback' => 'entity_property_verbatim_set',
    ),
    'summary' => array(
      'type' => 'text',
      'label' => t('Summary'),
      'setter callback' => 'entity_property_verbatim_set',
    ),
  );
}

/**
 * Defines info for the properties of the file-field item data structure.
 */
function entity_property_field_item_file_info() {
  $properties['file'] = array(
    'type' => 'file',
    'label' => t('The file.'),
    'getter callback' => 'entity_metadata_field_file_get',
    'setter callback' => 'entity_metadata_field_file_set',
    'required' => TRUE,
  );
  $properties['description'] = array(
    'type' => 'text',
    'label' => t('The file description'),
    'setter callback' => 'entity_property_verbatim_set',
  );
  $properties['display'] = array(
    'type' => 'boolean',
    'label' => t('Whether the file is being displayed.'),
    'setter callback' => 'entity_property_verbatim_set',
  );
  return $properties;
}

/**
 * Defines info for the properties of the image-field item data structure.
 */
function entity_property_field_item_image_info() {
  $properties['file'] = array(
    'type' => 'file',
    'label' => t('The image file.'),
    'getter callback' => 'entity_metadata_field_file_get',
    'setter callback' => 'entity_metadata_field_file_set',
    'required' => TRUE,
  );
  $properties['alt'] = array(
    'type' => 'text',
    'label' => t('The <em>Alt</em> attribute text'),
    'setter callback' => 'entity_property_verbatim_set',
  );
  $properties['title'] = array(
    'type' => 'text',
    'label' => t('The <em>Title</em> attribute text'),
    'setter callback' => 'entity_property_verbatim_set',
  );
  return $properties;
}


/**
 * Previously, hook_entity_property_info() has been provided by the removed
 * entity metadata module. To provide backward compatibility for provided
 * helpers that may be specified in hook_entity_property_info(), the following
 * (deprecated) functions are provided.
 */

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_verbatim_get($data, array $options, $name) {
  return entity_property_verbatim_get($data, $options, $name);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_verbatim_set($data, $name, $value) {
  return entity_property_verbatim_set($data, $name, $value);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_getter_method($object, array $options, $name) {
  return entity_property_getter_method($object, $options, $name);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_setter_method($object, $name, $value) {
  entity_property_setter_method($object, $name, $value);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_get_list($data, array $options, $name) {
  return entity_property_get_list($data, $options, $name);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_validate_integer_positive($value) {
  return entity_property_validate_integer_positive($value);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_validate_integer_non_negative($value) {
  return entity_property_validate_integer_non_negative($value);
}

/**
 * Deprecated.
 * Do not make use of this function, instead use the new one.
 */
function entity_metadata_text_formatted_properties() {
  return entity_property_text_formatted_info();
}
