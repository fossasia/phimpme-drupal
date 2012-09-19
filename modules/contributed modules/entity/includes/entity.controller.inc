<?php

/**
 * @file
 * Provides a controller building upon the core controller but providing more
 * features like full CRUD functionality.
 */

/**
 * Interface for EntityControllers compatible with the entity API.
 */
interface EntityAPIControllerInterface extends DrupalEntityControllerInterface {

  /**
   * Delete permanently saved entities.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $ids
   *   An array of entity IDs.
   */
  public function delete($ids);

  /**
   * Invokes a hook on behalf of the entity. For hooks that have a respective
   * field API attacher like insert/update/.. the attacher is called too.
   */
  public function invoke($hook, $entity);

  /**
   * Permanently saves the given entity.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $entity
   *   The entity to save.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED is returned depending on the operation
   *   performed.
   */
  public function save($entity);

  /**
   * Create a new entity.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   * @return
   *   A new instance of the entity type.
   */
  public function create(array $values = array());

  /**
   * Exports an entity as serialized string.
   *
   * @param $entity
   *   The entity to export.
   * @param $prefix
   *   An optional prefix for each line.
   *
   * @return
   *   The exported entity as serialized string. The format is determined by
   *   the controller and has to be compatible with the format that is accepted
   *   by the import() method.
   */
  public function export($entity, $prefix = '');

  /**
   * Imports an entity from a string.
   *
   * @param string $export
   *   An exported entity as serialized string.
   *
   * @return
   *   An entity object not yet saved.
   */
  public function import($export);

  /**
   * Builds a structured array representing the entity's content.
   *
   * The content built for the entity will vary depending on the $view_mode
   * parameter.
   *
   * @param $entity
   *   An entity object.
   * @param $view_mode
   *   View mode, e.g. 'full', 'teaser'...
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @return
   *   The renderable array.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL);

  /**
   * Generate an array for rendering the given entities.
   *
   * @param $entities
   *   An array of entities to render.
   * @param $view_mode
   *   View mode, e.g. 'full', 'teaser'...
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @param $page
   *   (optional) If set will control if the entity is rendered: if TRUE
   *   the entity will be rendered without its title, so that it can be embeded
   *   in another context. If FALSE the entity will be displayed with its title
   *   in a mode suitable for lists.
   *   If unset, the page mode will be enabled if the current path is the URI
   *   of the entity, as returned by entity_uri().
   *   This parameter is only supported for entities which controller is a
   *   EntityAPIControllerInterface.
   * @return
   *   The renderable array, keyed by entity name or numeric id.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL);
}

/**
 * A controller implementing EntityAPIControllerInterface for the database.
 */
class EntityAPIController extends DrupalDefaultEntityController implements EntityAPIControllerInterface {

  protected $cacheComplete = FALSE;
  protected $bundleKey;

  /**
   * Overridden.
   * @see DrupalDefaultEntityController#__construct()
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
    // If this is the bundle of another entity, set the bundle key.
    if (isset($this->entityInfo['bundle of'])) {
      $info = entity_get_info($this->entityInfo['bundle of']);
      $this->bundleKey = $info['bundle keys']['bundle'];
    }
  }

  /**
   * Builds and executes the query for loading.
   *
   * @return The results in a Traversable object.
   */
  public function query($ids, $conditions, $revision_id = FALSE) {
    // Build the query.
    $query = $this->buildQuery($ids, $conditions, $revision_id);
    $result = $query->execute();
    if (!empty($this->entityInfo['entity class'])) {
      $result->setFetchMode(PDO::FETCH_CLASS, $this->entityInfo['entity class'], array(array(), $this->entityType));
    }
    return $result;
  }

  /**
   * Overridden.
   * @see DrupalDefaultEntityController#load($ids, $conditions)
   *
   * In contrast to the parent implementation we factor out query execution, so
   * fetching can be further customized easily.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();

    // Revisions are not statically cached, and require a different query to
    // other conditions, so separate the revision id into its own variable.
    if ($this->revisionKey && isset($conditions[$this->revisionKey])) {
      $revision_id = $conditions[$this->revisionKey];
      unset($conditions[$this->revisionKey]);
    }
    else {
      $revision_id = FALSE;
    }

    // Create a new variable which is either a prepared version of the $ids
    // array for later comparison with the entity cache, or FALSE if no $ids
    // were passed. The $ids array is reduced as items are loaded from cache,
    // and we need to know if it's empty for this reason to avoid querying the
    // database when all requested entities are loaded from cache.
    $passed_ids = !empty($ids) ? array_flip($ids) : FALSE;

    // Try to load entities from the static cache.
    if ($this->cache && !$revision_id) {
      $entities = $this->cacheGet($ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      if ($passed_ids) {
        $ids = array_keys(array_diff_key($passed_ids, $entities));
      }
    }

    // Support the entitycache module if activated.
    if (!empty($this->entityInfo['entity cache']) && !$revision_id && $ids && !$conditions) {
      $cached_entities = EntityCacheControllerHelper::entityCacheGet($this, $ids, $conditions);
      // If any entities were loaded, remove them from the ids still to load.
      $ids = array_diff($ids, array_keys($cached_entities));
      $entities += $cached_entities;

      // Add loaded entities to the static cache if we are not loading a
      // revision.
      if ($this->cache && !empty($cached_entities) && !$revision_id) {
        $this->cacheSet($cached_entities);
      }
    }

    // Load any remaining entities from the database. This is the case if $ids
    // is set to FALSE (so we load all entities), if there are any ids left to
    // load or if loading a revision.
    if (!($this->cacheComplete && $ids === FALSE && !$conditions) && ($ids === FALSE || $ids || $revision_id)) {
      $queried_entities = array();
      foreach ($this->query($ids, $conditions, $revision_id) as $record) {
        // Skip entities already retrieved from cache.
        if (isset($entities[$record->{$this->idKey}])) {
          continue;
        }

        // Take care of serialized columns.
        $schema = drupal_get_schema($this->entityInfo['base table']);

        foreach ($schema['fields'] as $field => $info) {
          if (!empty($info['serialize']) && isset($record->$field)) {
            $record->$field = unserialize($record->$field);
            // Support automatic merging of 'data' fields into the entity.
            if (!empty($info['merge']) && is_array($record->$field)) {
              foreach ($record->$field as $key => $value) {
                $record->$key = $value;
              }
              unset($record->$field);
            }
          }
        }

        $queried_entities[$record->{$this->idKey}] = $record;
      }
    }

    // Pass all entities loaded from the database through $this->attachLoad(),
    // which attaches fields (if supported by the entity type) and calls the
    // entity type specific load callback, for example hook_node_load().
    if (!empty($queried_entities)) {
      $this->attachLoad($queried_entities, $revision_id);
      $entities += $queried_entities;
    }

    // Entitycache module support: Add entities to the entity cache if we are
    // not loading a revision.
    if (!empty($this->entityInfo['entity cache']) && !empty($queried_entities) && !$revision_id) {
      EntityCacheControllerHelper::entityCacheSet($this, $queried_entities);
    }

    if ($this->cache) {
      // Add entities to the cache if we are not loading a revision.
      if (!empty($queried_entities) && !$revision_id) {
        $this->cacheSet($queried_entities);

        // Remember if we have cached all entities now.
        if (!$conditions && $ids === FALSE) {
          $this->cacheComplete = TRUE;
        }
      }
    }
    // Ensure that the returned array is ordered the same as the original
    // $ids array if this was passed in and remove any invalid ids.
    if ($passed_ids && $passed_ids = array_intersect_key($passed_ids, $entities)) {
      foreach ($passed_ids as $id => $value) {
        $passed_ids[$id] = $entities[$id];
      }
      $entities = $passed_ids;
    }
    return $entities;
  }

  public function resetCache(array $ids = NULL) {
    $this->cacheComplete = FALSE;
    parent::resetCache($ids);
    // Support the entitycache module.
    if (!empty($this->entityInfo['entity cache'])) {
      EntityCacheControllerHelper::resetEntityCache($this, $ids);
    }
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function invoke($hook, $entity) {
    if (!empty($this->entityInfo['fieldable']) && function_exists($function = 'field_attach_' . $hook)) {
      $function($this->entityType, $entity);
    }

    if (isset($this->entityInfo['bundle of']) && $type = $this->entityInfo['bundle of']) {
      // Call field API bundle attachers for the entity we are a bundle of.
      if ($hook == 'insert') {
        field_attach_create_bundle($type, $entity->{$this->bundleKey});
      }
      elseif ($hook == 'delete') {
        field_attach_delete_bundle($type, $entity->{$this->bundleKey});
      }
      elseif ($hook == 'update' && $id = $entity->{$this->nameKey}) {
        if ($entity->original->{$this->bundleKey} != $entity->{$this->bundleKey}) {
          field_attach_rename_bundle($type, $entity->original->{$this->bundleKey}, $entity->{$this->bundleKey});
        }
      }
    }
    // Invoke the hook.
    module_invoke_all($this->entityType . '_' . $hook, $entity);
    // Invoke the respective entity level hook.
    if ($hook == 'presave' || $hook == 'insert' || $hook == 'update' || $hook == 'delete') {
      module_invoke_all('entity_' . $hook, $entity, $this->entityType);
    }
    // Invoke rules.
    if (module_exists('rules')) {
      rules_invoke_event($this->entityType . '_' . $hook, $entity);
    }
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if (!$entities) {
      // Do nothing, in case invalid or no ids have been passed.
      return;
    }
    // This transaction causes troubles on MySQL, see
    // http://drupal.org/node/1007830. So we deactivate this by default until
    // is shipped in a point release.
    // $transaction = isset($transaction) ? $transaction : db_transaction();

    try {
      $ids = array_keys($entities);

      db_delete($this->entityInfo['base table'])
        ->condition($this->idKey, $ids, 'IN')
        ->execute();
      // Reset the cache as soon as the changes have been applied.
      $this->resetCache($ids);

      foreach ($entities as $id => $entity) {
        $this->invoke('delete', $entity);
      }
      // Ignore slave server temporarily.
      db_ignore_slave();
    }
    catch (Exception $e) {
      if (isset($transaction)) {
        $transaction->rollback();
      }
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();
    try {
      // Load the stored entity, if any.
      if (!empty($entity->{$this->idKey}) && !isset($entity->original)) {
        // In order to properly work in case of name changes, load the original
        // entity using the id key if it is available.
        $entity->original = entity_load_unchanged($this->entityType, $entity->{$this->idKey});
      }

      $this->invoke('presave', $entity);

      if (!empty($entity->{$this->idKey}) && empty($entity->is_new)) {
        $return = drupal_write_record($this->entityInfo['base table'], $entity, $this->idKey);
        $this->resetCache(array($entity->{$this->idKey}));
        $this->invoke('update', $entity);
      }
      else {
        $return = drupal_write_record($this->entityInfo['base table'], $entity);
        $this->invoke('insert', $entity);
      }
      // Ignore slave server temporarily.
      db_ignore_slave();
      unset($entity->is_new);
      unset($entity->original);

      return $return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function create(array $values = array()) {
    // Add is_new property if it is not set.
    $values += array('is_new' => TRUE);
    if (isset($this->entityInfo['entity class']) && $class = $this->entityInfo['entity class']) {
      return new $class($values, $this->entityType);
    }
    return (object) $values;
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @return
   *   A serialized string in JSON format suitable for the import() method.
   */
  public function export($entity, $prefix = '') {
    $vars = get_object_vars($entity);
    unset($vars['is_new']);
    return entity_var_json_export($vars, $prefix);
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $export
   *   A serialized string in JSON format as produced by the export() method.
   */
  public function import($export) {
    $vars = drupal_json_decode($export);
    if (is_array($vars)) {
      return $this->create($vars);
    }
    return FALSE;
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $content
   *   Optionally. Allows pre-populating the built content to ease overridding
   *   this method.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    // Remove previously built content, if exists.
    $entity->content = $content;
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    // Add in fields.
    if (!empty($this->entityInfo['fieldable'])) {
      $entity->content += field_attach_view($this->entityType, $entity, $view_mode, $langcode);
    }
    // Invoke hook_ENTITY_view() to allow modules to add their additions.
    if (module_exists('rules')) {
      rules_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    else {
      module_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    module_invoke_all('entity_view', $entity, $this->entityType, $view_mode, $langcode);
    $build = $entity->content;
    unset($entity->content);
    return $build;
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    // For Field API and entity_prepare_view, the entities have to be keyed by
    // (numeric) id.
    $entities = entity_key_array_by_property($entities, $this->idKey);
    if (!empty($this->entityInfo['fieldable'])) {
      field_attach_prepare_view($this->entityType, $entities, $view_mode);
    }
    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    $view = array();
    foreach ($entities as $entity) {
      $build = entity_build_content($this->entityType, $entity, $view_mode, $langcode);
      $build += array(
        // If the entity type provides an implementation, use this instead the
        // generic one.
        // @see template_preprocess_entity()
        '#theme' => 'entity',
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );
      // Allow modules to modify the structured entity.
      drupal_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $view[$this->entityType][$key] = $build;
    }
    return $view;
  }
}

/**
 * A controller implementing exportables stored in the database.
 */
class EntityAPIControllerExportable extends EntityAPIController {

  protected $entityCacheByName = array();
  protected $nameKey, $statusKey, $moduleKey;

  /**
   * Overridden.
   *
   * Allows specifying a name key serving as uniform identifier for this entity
   * type while still internally we are using numeric identifieres.
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
    // Use the name key as primary identifier.
    $this->nameKey = isset($this->entityInfo['entity keys']['name']) ? $this->entityInfo['entity keys']['name'] : $this->idKey;
    if (!empty($this->entityInfo['exportable'])) {
      $this->statusKey = isset($this->entityInfo['entity keys']['status']) ? $this->entityInfo['entity keys']['status'] : 'status';
      $this->moduleKey = isset($this->entityInfo['entity keys']['module']) ? $this->entityInfo['entity keys']['module'] : 'module';
    }
  }

  /**
   * Support loading by name key.
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    // Add the id condition ourself, as we might have a separate name key.
    $query = parent::buildQuery(array(), $conditions, $revision_id);
    if ($ids) {
      // Support loading by numeric ids as well as by machine names.
      $key = is_numeric(reset($ids)) ? $this->idKey : $this->nameKey;
      $query->condition("base.$key", $ids, 'IN');
    }
    return $query;
  }

  /**
   * Overridden to support passing numeric ids as well as names as $ids.
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();

    // Only do something if loaded by names.
    if (!$ids || $this->nameKey == $this->idKey || is_numeric(reset($ids))) {
      return parent::load($ids, $conditions);
    }

    // Revisions are not statically cached, and require a different query to
    // other conditions, so separate the revision id into its own variable.
    if ($this->revisionKey && isset($conditions[$this->revisionKey])) {
      $revision_id = $conditions[$this->revisionKey];
      unset($conditions[$this->revisionKey]);
    }
    else {
      $revision_id = FALSE;
    }
    $passed_ids = !empty($ids) ? array_flip($ids) : FALSE;

    // Care about the static cache.
    if ($this->cache && !$revision_id) {
      $entities = $this->cacheGetByName($ids, $conditions);
    }
    // If any entities were loaded, remove them from the ids still to load.
    if ($entities) {
      $ids = array_keys(array_diff_key($passed_ids, $entities));
    }

    $entities_by_id = parent::load($ids, $conditions);
    $entities += entity_key_array_by_property($entities_by_id, $this->nameKey);

    // Ensure that the returned array is keyed by numeric id and ordered the
    // same as the original $ids array and remove any invalid ids.
    $return = array();
    foreach ($passed_ids as $name => $value) {
      if (isset($entities[$name])) {
        $return[$entities[$name]->{$this->idKey}] = $entities[$name];
      }
    }
    return $return;
  }

  /**
   * Overridden.
   * @see DrupalDefaultEntityController::cacheGet()
   */
  protected function cacheGet($ids, $conditions = array()) {
    if (!empty($this->entityCache) && $ids !== array()) {
      $entities = $ids ? array_intersect_key($this->entityCache, array_flip($ids)) : $this->entityCache;
      return $this->applyConditions($entities, $conditions);
    }
    return array();
  }

  /**
   * Like cacheGet() but keyed by name.
   */
  protected function cacheGetByName($names, $conditions = array()) {
    if (!empty($this->entityCacheByName) && $names !== array() && $names) {
      // First get the entities by ids, then apply the conditions.
      // Generally, we make use of $this->entityCache, but if we are loading by
      // name, we have to use $this->entityCacheByName.
      $entities = array_intersect_key($this->entityCacheByName, array_flip($names));
      return $this->applyConditions($entities, $conditions);
    }
    return array();
  }

  protected function applyConditions($entities, $conditions = array()) {
    if ($conditions) {
      foreach ($entities as $key => $entity) {
        $entity_values = (array) $entity;
        if (array_diff_assoc($conditions, $entity_values)) {
          unset($entities[$key]);
        }
      }
    }
    return $entities;
  }

  /**
   * Overridden.
   * @see DrupalDefaultEntityController::cacheSet()
   */
  protected function cacheSet($entities) {
    $this->entityCache += $entities;
    // If we have a name key, also support static caching when loading by name.
    if ($this->nameKey != $this->idKey) {
      $this->entityCacheByName += entity_key_array_by_property($entities, $this->nameKey);
    }
  }

  /**
   * Overridden.
   * @see DrupalDefaultEntityController::attachLoad()
   *
   * Changed to call type-specific hook with the entities keyed by name if they
   * have one.
   */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    // Attach fields.
    if ($this->entityInfo['fieldable']) {
      if ($revision_id) {
        field_attach_load_revision($this->entityType, $queried_entities);
      }
      else {
        field_attach_load($this->entityType, $queried_entities);
      }
    }

    // Call hook_entity_load().
    foreach (module_implements('entity_load') as $module) {
      $function = $module . '_entity_load';
      $function($queried_entities, $this->entityType);
    }
    // Call hook_TYPE_load(). The first argument for hook_TYPE_load() are
    // always the queried entities, followed by additional arguments set in
    // $this->hookLoadArguments.
    // For entities with a name key, pass the entities keyed by name to the
    // specific load hook.
    if ($this->nameKey != $this->idKey) {
      $entities_by_name = entity_key_array_by_property($queried_entities, $this->nameKey);
    }
    else {
      $entities_by_name = $queried_entities;
    }
    $args = array_merge(array($entities_by_name), $this->hookLoadArguments);
    foreach (module_implements($this->entityInfo['load hook']) as $module) {
      call_user_func_array($module . '_' . $this->entityInfo['load hook'], $args);
    }
  }

  public function resetCache(array $ids = NULL) {
    $this->cacheComplete = FALSE;
    if (isset($ids)) {
      foreach (array_intersect_key($this->entityCache, array_flip($ids)) as $id => $entity) {
        unset($this->entityCacheByName[$this->entityCache[$id]->{$this->nameKey}]);
        unset($this->entityCache[$id]);
      }
    }
    else {
      $this->entityCache = array();
      $this->entityCacheByName = array();
    }
  }

  /**
   * Overridden to care about reverted entities.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if ($entities) {
      parent::delete($ids, $transaction);

      foreach ($entities as $id => $entity) {
        if (entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
          entity_defaults_rebuild(array($this->entityType));
          break;
        }
      }
    }
  }

  /**
   * Overridden to care about reverted bundle entities and to skip Rules.
   */
  public function invoke($hook, $entity) {
    if ($hook == 'delete') {
      // To ease figuring out whether this is a revert, make sure that the
      // entity status is updated in case the providing module has been
      // disabled.
      if (entity_has_status($this->entityType, $entity, ENTITY_IN_CODE) && !module_exists($entity->{$this->moduleKey})) {
        $entity->{$this->statusKey} = ENTITY_CUSTOM;
      }
      $is_revert = entity_has_status($this->entityType, $entity, ENTITY_IN_CODE);
    }

    if (!empty($this->entityInfo['fieldable']) && function_exists($function = 'field_attach_' . $hook)) {
      $function($this->entityType, $entity);
    }

    if (isset($this->entityInfo['bundle of']) && $type = $this->entityInfo['bundle of']) {
      // Call field API bundle attachers for the entity we are a bundle of.
      if ($hook == 'insert') {
        field_attach_create_bundle($type, $entity->{$this->bundleKey});
      }
      elseif ($hook == 'delete' && !$is_revert) {
        field_attach_delete_bundle($type, $entity->{$this->bundleKey});
      }
      elseif ($hook == 'update' && $id = $entity->{$this->nameKey}) {
        if ($entity->original->{$this->bundleKey} != $entity->{$this->bundleKey}) {
          field_attach_rename_bundle($type, $entity->original->{$this->bundleKey}, $entity->{$this->bundleKey});
        }
      }
    }
    // Invoke the hook.
    module_invoke_all($this->entityType . '_' . $hook, $entity);
    // Invoke the respective entity level hook.
    if ($hook == 'presave' || $hook == 'insert' || $hook == 'update' || $hook == 'delete') {
      module_invoke_all('entity_' . $hook, $entity, $this->entityType);
    }
  }

  /**
   * Overridden to care exportables that are overridden.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Preload $entity->original by name key if necessary.
    if (!empty($entity->{$this->nameKey}) && empty($entity->{$this->idKey}) && !isset($entity->original)) {
      $entity->original = entity_load_unchanged($this->entityType, $entity->{$this->nameKey});
    }
    // Update the status for entities getting overridden.
    if (entity_has_status($this->entityType, $entity, ENTITY_IN_CODE) && empty($entity->is_rebuild)) {
      $entity->{$this->statusKey} |= ENTITY_CUSTOM;
    }
    return parent::save($entity, $transaction);
  }

  /**
   * Overridden.
   */
  public function export($entity, $prefix = '') {
    $vars = get_object_vars($entity);
    unset($vars[$this->statusKey], $vars[$this->moduleKey], $vars['is_new']);
    if ($this->nameKey != $this->idKey) {
      unset($vars[$this->idKey]);
    }
    return entity_var_json_export($vars, $prefix);
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    $view = parent::view($entities, $view_mode, $langcode, $page);

    if ($this->nameKey != $this->idKey) {
      // Re-key the view array to be keyed by name.
      $return = array();
      foreach ($view[$this->entityType] as $id => $content) {
        $key = isset($content['#entity']->{$this->nameKey}) ? $content['#entity']->{$this->nameKey} : NULL;
        $return[$this->entityType][$key] = $content;
      }
      $view = $return;
    }
    return $view;
  }
}
