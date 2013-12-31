<?php

/**
 * @file
 * Hooks provided by the entity API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Provide an entity type via the entity CRUD API.
 *
 * This is a placeholder for describing further keys for hook_entity_info(),
 * which are introduced the entity API for providing a new entity type with the
 * entity CRUD API. For that the entity API provides two controllers:
 *  - EntityAPIController: A regular CRUD controller.
 *  - EntityAPIControllerExportable: Extends the regular controller to
 *    additionally support exportable entities and/or entities making use of a
 *    name key.
 * See entity_metadata_hook_entity_info() for the documentation of additional
 * keys for hook_entity_info() as introduced by the entity API and supported for
 * any entity type.
 *
 * The entity CRUD API supports the following keys:
 * - entity class: (optional) A class the controller will use for instantiating
 *   entities. It is suggested to make use of the provided "Entity" class or to
 *   extend it.
 * - bundle of: (optional) If the described entity type is used as a bundle for
 *   another, fieldable entity type, the entity API controller can take care of
 *   invoking the field API bundle attachers. To enable this functionality,
 *   specify the name of the fieldable entity type. But note, that the usual
 *   information about the bundles is still required for the fieldable entity
 *   type, as described by the documentation of hook_entity_info(). Also,
 *   field_attach_delete_bundle() has to be invoked manually upon module
 *   uninstallation. See entity_test_entity_info() and entity_test_uninstall()
 *   for examples.
 * - module: (optional) The module providing the entity type. This is optional,
 *   but strongly suggested.
 * - exportable: (optional) Whether the entity is exportable. Defaults to FALSE.
 *   If enabled, a name key should be specified and db columns for the module
 *   and status key as defined by entity_exportable_schema_fields() have to
 *   exist in the entity's base table. Also see 'entity keys' below.
 *   This option requires the EntityAPIControllerExportable to work.
 * - entity keys: An array of keys as defined by Drupal core. The following
 *   additional keys are used by the entity CRUD API:
 *   - name: (optional) The key of the entity property containing the unique,
 *     machine readable name of the entity. If specified, this is used as
 *     identifier of the entity, while the usual 'id' key is still required and
 *     may be used when modules deal with entities generically, or to refer to
 *     the entity internally, i.e. in the database.
 *     If a name key is given, the name is used as entity identifier by the
 *     entity API module, metadata wrappers and entity-type specific hooks.
 *     However note that for consistency all generic entity hooks like
 *     hook_entity_load() are invoked with the entities keyed by numeric id,
 *     while entity-type specific hooks like hook_{entity_type}_load() are
 *     invoked with the entities keyed by name.
 *     Also, just as entity_load_single() entity_load() may be called
 *     with names passed as the $ids parameter, while the results of
 *     entity_load() are always keyed by numeric id. Thus, it is suggested to
 *     make use of entity_load_multiple_by_name() to implement entity-type
 *     specific loading functions like {entity_type}_load_multiple(), as this
 *     function returns the entities keyed by name. See entity_test_get_types()
 *     for an example.
 *     For exportable entities, it is strongly recommended to make use of a
 *     machine name as names are portable across systems.
 *     This option requires the EntityAPIControllerExportable to work.
 *   - module: (optional) A key for the module property used by the entity CRUD
 *     API to save the source module name for exportable entities that have been
 *     provided in code. Defaults to 'module'.
 *   - status: (optional) The name of the entity property used by the entity
 *     CRUD API to save the exportable entity status using defined bit flags.
 *     Defaults to 'status'. See entity_has_status().
 * - export: (optional) An array of information used for exporting. For ctools
 *   exportables compatibility any export-keys supported by ctools may be added
 *   to this array too.
 *   - default hook: What hook to invoke to find exportable entities that are
 *     currently defined. This hook is automatically called by the CRUD
 *     controller during entity_load(). Defaults to 'default_' . $entity_type.
 * - admin ui: (optional) An array of optional information used for providing an
 *   administrative user interface. To enable the UI at least the path must be
 *   given. Apart from that, the 'access callback' (see below) is required for
 *   the entity, as well as the 'ENTITY_TYPE_form' for editing, adding and
 *   cloning. The form gets the entity and the operation ('edit', 'add' or
 *   'clone') passed. See entity_ui_get_form() for more details.
 *   Known keys are:
 *   - path: A path where the UI should show up as expected by hook_menu().
 *   - controller class: (optional) A controller class name for providing the
 *     UI. Defaults to EntityDefaultUIController, which implements an admin UI
 *     suiting for managing configuration entities.
 *     For customizing the UI inherit from the default class and overide methods
 *     as suiting and specify your class as controller class.
 *   - file: (optional) The name of the file in which the entity form resides
 *     as it is required by hook_menu().
 *   - file path: (optional) The path to the file as required by hook_menu. If
 *     not set, it defaults to entity type's module's path, thus the entity
 *     types 'module' key is required.
 *   - menu wildcard: The wildcard to use in paths of the hook_menu() items.
 *     Defaults to %entity_object which is the loader provided by Entity API.
 * - rules controller class: (optional) A controller class for providing Rules
 *   integration, or FALSE to disable this feature. The given class has to
 *   inherit from the class EntityDefaultRulesController, which serves as
 *   default in case the entity type is not marked as configuration. For
 *   configuration entities it defaults to FALSE.
 * - metadata controller class: (optional) A controller class for providing
 *   entity property info. By default some info is generated out of the
 *   information provided in your hook_schema() implementation, while only read
 *   access is granted to that properties by default. Based upon that the
 *   Entity tokens module also generates token replacements for your entity
 *   type, once activated.
 *   Override the controller class to adapt the defaults and to improve and
 *   complete the generated metadata. Set it to FALSE to disable this feature.
 *   Defaults to the EntityDefaultMetadataController class.
 * - features controller class: (optional) A controller class for providing
 *   Features module integration for exportable entities. The given class has to
 *   inherit from the default class being EntityDefaultFeaturesController. Set
 *   it to FALSE to disable this feature.
 * - views controller class: (optional) A controller class for providing views
 *   integration. The given class has to inherit from the class
 *   EntityDefaultViewsController, which is set as default in case the providing
 *   module has been specified (see 'module') and the module does not provide
 *   any views integration. Else it defaults to FALSE, which disables this
 *   feature. See EntityDefaultViewsController.
 * - access callback: (optional) Specify a callback that returns access
 *   permissions for the operations 'create', 'update', 'delete' and 'view'.
 *   The callback gets optionally the entity and the user account to check for
 *   passed. See entity_access() for more details on the arguments and
 *   entity_metadata_no_hook_node_access() for an example.
 *   This is optional, but suggested for the Rules integration, and required for
 *   the admin ui (see above).
 * - form callback: (optional) Specfiy a callback that returns a fully built
 *   edit form for your entity type. See entity_form().
 *   In case the 'admin ui' is used, no callback needs to be specified.
 * - entity cache: (optional) Whether entities should be cached using the cache
 *   system. Requires the entitycache module to be installed and enabled. As
 *   cached entities are only retrieved by id key, the cache would not apply to
 *   exportable entities retrieved by name key. If enabled, 'field cache' is
 *   obsolete and should be disabled. Defaults to FALSE.
 *
 * @see hook_entity_info()
 * @see entity_metadata_hook_entity_info()
 */
function entity_crud_hook_entity_info() {
  $return = array(
    'entity_test' => array(
      'label' => t('Test Entity'),
      'entity class' => 'Entity',
      'controller class' => 'EntityAPIController',
      'base table' => 'entity_test',
      'module' => 'entity_test',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'pid',
        'name' => 'name',
        'bundle' => 'type',
      ),
      'bundles' => array(),
    ),
  );
  foreach (entity_test_get_types() as $name => $info) {
    $return['entity_test']['bundles'][$name] = array(
      'label' => $info['label'],
    );
  }
  return $return;
}

/**
 * Provide additional metadata for entities.
 *
 * This is a placeholder for describing further keys for hook_entity_info(),
 * which are introduced the entity API in order to support any entity type; e.g.
 * to make entity_save(), entity_create(), entity_view() and others work.
 * See entity_crud_hook_entity_info() for the documentation of additional keys
 * for hook_entity_info() as introduced by the entity API for providing new
 * entity types with the entity CRUD API.
 *
 * Additional keys are:
 * - plural label: (optional) The human-readable, plural name of the entity
 *   type. As 'label' it should start capitalized.
 * - description: (optional) A human-readable description of the entity type.
 * - access callback: (optional) Specify a callback that returns access
 *   permissions for the operations 'create', 'update', 'delete' and 'view'.
 *   The callback gets optionally the entity and the user account to check for
 *   passed. See entity_access() for more details on the arguments and
 *   entity_metadata_no_hook_node_access() for an example.
 * - creation callback: (optional) A callback that creates a new instance of
 *   this entity type. See entity_metadata_create_node() for an example.
 * - save callback: (optional) A callback that permanently saves an entity of
 *   this type.
 * - deletion callback: (optional) A callback that permanently deletes an
 *   entity of this type.
 * - view callback: (optional) A callback to render a list of entities.
 *   See entity_metadata_view_node() as example.
 * - form callback: (optional) A callback that returns a fully built edit form
 *   for the entity type.
 * - token type: (optional) A type name to use for token replacements. Set it
 *   to FALSE if there aren't any token replacements for this entity type.
 * - configuration: (optional) A boolean value that specifies whether the entity
 *   type should be considered as configuration. Modules working with entities
 *   may use this value to decide whether they should deal with a certain entity
 *   type. Defaults to TRUE to for entity types that are exportable, else to
 *   FALSE.
 *
 * @see hook_entity_info()
 * @see entity_crud_hook_entity_info()
 * @see entity_access()
 * @see entity_create()
 * @see entity_save()
 * @see entity_delete()
 * @see entity_view()
 * @see entity_form()
 */
function entity_metadata_hook_entity_info() {
  return array(
    'node' => array(
      'label' => t('Node'),
      'access callback' => 'entity_metadata_no_hook_node_access',
      // ...
    ),
  );
}

/**
 * Allow modules to define metadata about entity properties.
 *
 * Modules providing properties for any entities defined in hook_entity_info()
 * can implement this hook to provide metadata about this properties.
 * For making use of the metadata have a look at the provided wrappers returned
 * by entity_metadata_wrapper().
 * For providing property information for fields see entity_field_info().
 *
 * @return
 *   An array whose keys are entity type names and whose values are arrays
 *   containing the keys:
 *   - properties: The array describing all properties for this entity. Entries
 *     are keyed by the property name and contain an array of metadata for each
 *     property. The name may only contain alphanumeric lowercase characters
 *     and underscores. Known keys are:
 *     - label: A human readable, translated label for the property.
 *     - description: (optional) A human readable, translated description for
 *       the property.
 *     - type: The data type of the property. To make the property actually
 *       useful it is important to map your properties to one of the known data
 *       types, which currently are:
 *        - text: Any text.
 *        - token: A string containing only lowercase letters, numbers, and
 *          underscores starting with a letter; e.g. this type is useful for
 *          machine readable names.
 *        - integer: A usual PHP integer value.
 *        - decimal: A PHP float or integer.
 *        - date: A full date and time, as timestamp.
 *        - duration: A duration as number of seconds.
 *        - boolean: A usual PHP boolean value.
 *        - uri: An absolute URI or URL.
 *        - entities - You may use the type of each entity known by
 *          hook_entity_info(), e.g. 'node' or 'user'. Internally entities are
 *          represented by their identifieres. In case of single-valued
 *          properties getter callbacks may return full entity objects as well,
 *          while a value of FALSE is interpreted like a NULL value as "property
 *          is not set".
 *        - entity: A special type to be used generically for entities where the
 *          entity type is not known beforehand. The entity has to be
 *          represented using an EntityMetadataWrapper.
 *        - struct: This as well as any else not known type may be used for
 *          supporting arbitrary data structures. For that additional metadata
 *          has to be specified with the 'property info' key. New type names
 *          have to be properly prefixed with the module name.
 *        - list: A list of values, represented as numerically indexed array.
 *          The list<TYPE> notation may be used to specify the type of the
 *          contained items, where TYPE may be any valid type expression.
 *     - bundle: (optional) If the property is an entity, you may specify the
 *       bundle of the referenced entity.
 *     - options list: (optional) A callback that returns a list of possible
 *       values for the property. The callback has to return an array as
 *       used by hook_options_list().
 *       Note that it is possible to return a different set of options depending
 *       whether they are used in read or in write context. See
 *       EntityMetadataWrapper::optionsList() for more details on that.
 *     - getter callback: (optional) A callback used to retrieve the value of
 *       the property. Defaults to entity_property_verbatim_get().
 *       It is important that your data is represented, as documented for your
 *       data type, e.g. a date has to be a timestamp. Thus if necessary, the
 *       getter callback has to do the necessary conversion. In case of an empty
 *       or not set value, the callback has to return NULL.
 *     - setter callback: (optional) A callback used to set the value of the
 *       property. In many cases entity_property_verbatim_set() can be used.
 *     - validation callback: (optional) A callback that returns whether the
 *       passed data value is valid for the property. May be used to implement
 *       additional validation checks, such as to ensure the value is a valid
 *       mail address.
 *     - access callback: (optional) An access callback to allow for checking
 *       'view' and 'edit' access for the described property. If no callback
 *       is specified, a 'setter permission' may be specified instead.
 *     - setter permission: (optional) A permission that describes whether
 *       a user has permission to set ('edit') this property. This permission
 *       is only be taken into account, if no 'access callback' is given.
 *     - schema field: (optional) In case the property is directly based upon
 *       a field specified in the entity's hook_schema(), the name of the field.
 *     - queryable: (optional) Whether a property is queryable with
 *       EntityFieldQuery. Defaults to TRUE if a 'schema field' is specified, or
 *       if the deprecated 'query callback' is set to
 *       'entity_metadata_field_query'. Otherwise it defaults to FALSE.
 *     - query callback: (deprecated) A callback for querying for entities
 *       having the given property value. See entity_property_query().
 *       Generally, properties should be queryable via EntityFieldQuery. If
 *       that is the case, just set 'queryable' to TRUE.
 *     - required: (optional) Whether this property is required for the creation
 *       of a new instance of its entity. See
 *       entity_property_values_create_entity().
 *     - field: (optional) A boolean indicating whether a property is stemming
 *       from a field.
 *     - computed: (optional) A boolean indiciating whether a property is
 *       computed, i.e. the property value is not stored or loaded by the
 *       entity's controller but determined on the fly by the getter callback.
 *       Defaults to FALSE.
 *     - sanitized: (optional) For textual properties only, whether the text is
 *       already sanitized. In this case you might want to also specify a raw
 *       getter callback. Defaults to FALSE.
 *     - sanitize: (optional) For textual properties, that are not sanitized
 *       yet, specify a function for sanitizing the value. Defaults to
 *       check_plain().
 *     - raw getter callback: (optional) For sanitized textual properties, a
 *       separate callback which can be used to retrieve the raw, unprocessed
 *       value.
 *     - clear: (optional) An array of property names, of which the cache should
 *       be cleared too once this property is updated. As a rule of thumb any
 *       duplicated properties should be avoided though.
 *     - property info: (optional) An array of info for an arbitrary data
 *       structure together with any else not defined type, see data type
 *       'struct'. Specify metadata in the same way as defined for this hook.
 *     - property info alter: (optional) A callback for altering the property
 *       info before it is used by the metadata wrappers.
 *     - property defaults: (optional) An array of property info defaults for
 *       each property derived of the wrapped data item (e.g. an entity).
 *       Applied by the metadata wrappers.
 *     - auto creation: (optional) Properties of type 'struct' may specify
 *       this callback which is used to automatically create the data structure
 *       (e.g. an array) if necessary. This is necessary in order to support
 *       setting a property of a not yet initialized data structure.
 *       See entity_metadata_field_file_callback() for an example.
 *     - translatable: (optional) Whether the property is translatable, defaults
 *       to FALSE.
 *     - entity token: (optional) If Entity tokens module is enabled, the
 *       module provides a token for the property if one does not exist yet.
 *       Specify FALSE to disable this functionality for the property.
 *   - bundles: An array keyed by bundle name containing further metadata
 *     related to the bundles only. This array may contain the key 'properties'
 *     with an array of info about the bundle specific properties, structured in
 *     the same way as the entity properties array.
 *
 *  @see hook_entity_property_info_alter()
 *  @see entity_metadata_get_info()
 *  @see entity_metadata_wrapper()
 */
function hook_entity_property_info() {
  $info = array();
  $properties = &$info['node']['properties'];

  $properties['nid'] = array(
    'label' => t("Content ID"),
    'type' => 'integer',
    'description' => t("The unique content ID."),
  );
  return $info;
}

/**
 * Allow modules to alter metadata about entity properties.
 *
 * @see hook_entity_property_info()
 */
function hook_entity_property_info_alter(&$info) {
  $properties = &$info['node']['bundles']['poll']['properties'];

  $properties['poll-votes'] = array(
    'label' => t("Poll votes"),
    'description' => t("The number of votes that have been cast on a poll node."),
    'type' => 'integer',
    'getter callback' => 'entity_property_poll_node_get_properties',
  );
}

/**
 * Provide entity property information for fields.
 *
 * This is a placeholder for describing further keys for hook_field_info(),
 * which are introduced by the entity API.
 *
 * For providing entity property info for fields each field type may specify a
 * property type to map to using the key 'property_type'. With that info in
 * place useful defaults are generated, which suffice for a lot of field
 * types.
 * However it is possible to specify further callbacks that may alter the
 * generated property info. To do so use the key 'property_callbacks' and set
 * it to an array of function names. Apart from that any property info provided
 * for a field instance using the key 'property info' is added in too.
 *
 * @see entity_field_info_alter()
 * @see entity_metadata_field_text_property_callback()
 */
function entity_hook_field_info() {
  return array(
    'text' => array(
      'label' => t('Text'),
      'property_type' => 'text',
      // ...
    ),
  );
}

/**
 * Alter the handlers used by the data selection tables provided by this module.
 *
 * @param array $field_handlers
 *   An array of the field handler classes to use for specific types. The keys
 *   are the types, mapped to their respective classes. Contained types are:
 *   - All primitive types known by the entity API (see
 *     hook_entity_property_info()).
 *   - options: Special type for fields having an options list.
 *   - field: Special type for Field API fields.
 *   - entity: Special type for entity-valued fields.
 *   - relationship: Views relationship handler to use for relationships.
 *   Values for all specific entity types can be additionally added.
 *
 * @see entity_views_field_definition()
 * @see entity_views_get_field_handlers()
 */
function hook_entity_views_field_handlers_alter(array &$field_handlers) {
  $field_handlers['duration'] = 'example_duration_handler';
  $field_handlers['node'] = 'example_node_handler';
}

/**
 * @} End of "addtogroup hooks".
 */
