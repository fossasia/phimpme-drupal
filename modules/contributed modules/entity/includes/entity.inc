<?php

/**
 * @file
 * Provides a base class for entities.
 */

/**
 * A common class for entities.
 *
 * It's suggested, but not required, to extend this class and to override
 * __construct() in order to specify a fixed entity type.
 *
 * For providing an entity label and URI it is suggested to override the
 * defaultLabel() and defaultUri() methods, and to specify the
 * entity_class_label() and entity_class_uri() as respective callbacks in
 * hook_entity_info(). That way modules are able to override your defaults
 * by altering the hook_entity_info() callbacks, while $entity->label() and
 * $entity->uri() reflect this changes as well.
 *
 * Defaults for entity properties can be easily defined by adding class
 * properties, e.g.:
 * @code
 *   public $name = '';
 *   public $count = 0;
 * @endcode
 */
class Entity {

  protected $entityType;
  protected $entityInfo;
  protected $idKey, $nameKey, $statusKey;

  /**
   * Creates a new entity.
   *
   * @see entity_create()
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    if (empty($entityType)) {
      throw new Exception('Cannot created an instance of Entity without a specified entity type.');
    }
    $this->entityType = $entityType;
    $this->setUp();
    // Set initial values.
    foreach ($values as $key => $value) {
      $this->$key = $value;
    }
  }

  /**
   * Set up the object instance on construction or unserializiation.
   */
  protected function setUp() {
    $this->entityInfo = entity_get_info($this->entityType);
    $this->idKey = $this->entityInfo['entity keys']['id'];
    $this->nameKey = isset($this->entityInfo['entity keys']['name']) ? $this->entityInfo['entity keys']['name'] : $this->idKey;
    $this->statusKey = empty($info['entity keys']['status']) ? 'status' : $info['entity keys']['status'];
  }

  /**
   * Returns the internal, numeric identifier.
   *
   * Returns the numeric identifier, even if the entity type has specified a
   * name key. In the latter case, the numeric identifier is supposed to be used
   * when dealing generically with entities or internally to refer to an entity,
   * i.e. in a relational database. If unsure, use Entity:identifier().
   */
  public function internalIdentifier() {
    return isset($this->{$this->idKey}) ? $this->{$this->idKey} : NULL;
  }

  /**
   * Returns the entity identifier, i.e. the entities name or numeric id.
   *
   * @return
   *   The identifier of the entity. If the entity type makes use of a name key,
   *   the name is returned, else the numeric id.
   *
   * @see entity_id()
   */
  public function identifier() {
    return isset($this->{$this->nameKey}) ? $this->{$this->nameKey} : NULL;
  }

  /**
   * Returns the info of the type of the entity.
   *
   * @see entity_get_info()
   */
  public function entityInfo() {
    return $this->entityInfo;
  }

  /**
   * Returns the type of the entity.
   */
  public function entityType() {
    return $this->entityType;
  }

  /**
   * Returns the label of the entity.
   *
   * Modules may alter the label by specifying another 'label callback' using
   * hook_entity_info_alter().
   *
   * @see entity_label()
   */
  public function label() {
    if (isset($this->entityInfo['label callback']) && $this->entityInfo['label callback'] == 'entity_class_label') {
      return $this->defaultLabel();
    }
    return entity_label($this->entityType, $this);
  }

  /**
   * Override this in order to implement a custom default label and specify
   * 'entity_class_label' as 'label callback' hook_entity_info(). As an
   * alternative to that the label key may be provided in hook_entity_info().
   */
  protected function defaultLabel() {
    return 'unlabelled';
  }

  /**
   * Returns the uri of the entity just as entity_uri().
   *
   * Modules may alter the uri by specifying another 'uri callback' using
   * hook_entity_info_alter().
   *
   * @see entity_uri()
   */
  public function uri() {
    if (isset($this->entityInfo['uri callback']) && $this->entityInfo['uri callback'] == 'entity_class_uri') {
      return $this->defaultUri();
    }
    return entity_uri($this->entityType, $this);
  }

  /**
   * Override this in order to implement a custom default URI and specify
   * 'entity_class_uri' as 'uri callback' hook_entity_info().
   */
  protected function defaultUri() {
    return array('path' => 'default/' . $this->identifier());
  }

  /**
   * Checks if the entity has a certain exportable status.
   *
   * @param $status
   *   A status constant, i.e. one of ENTITY_CUSTOM, ENTITY_IN_CODE,
   *   ENTITY_OVERRIDDEN or ENTITY_FIXED.
   *
   * @return
   *   For exportable entities TRUE if the entity has the status, else FALSE.
   *   In case the entity is not exportable, NULL is returned.
   *
   * @see entity_has_status()
   */
  public function hasStatus($status) {
    if (!empty($this->entityInfo['exportable'])) {
      return isset($this->{$this->statusKey}) && ($this->{$this->statusKey} & $status) == $status;
    }
  }

  /**
   * Permanently saves the entity.
   *
   * @see entity_save()
   */
  public function save() {
    return entity_get_controller($this->entityType)->save($this);
  }

  /**
   * Permanently deletes the entity.
   *
   * @see entity_delete()
   */
  public function delete() {
    $id = $this->identifier();
    if (isset($id)) {
      entity_get_controller($this->entityType)->delete(array($id));
    }
  }

  /**
   * Exports the entity.
   *
   * @see entity_export()
   */
  public function export($prefix = '') {
    return entity_get_controller($this->entityType)->export($this, $prefix);
  }

  /**
   * Generate an array for rendering the entity.
   *
   * @see entity_view()
   */
  public function view($view_mode = 'full', $langcode = NULL, $page = NULL) {
    return entity_get_controller($this->entityType)->view(array($this), $view_mode, $langcode, $page);
  }

  /**
   * Builds a structured array representing the entity's content.
   *
   * @see entity_build_content()
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    return entity_get_controller($this->entityType)->buildContent($this, $view_mode, $langcode);
  }

  /**
   * Magic method to only serialize what's necessary.
   */
  public function __sleep() {
    $vars = get_object_vars($this);
    unset($vars['entityInfo'], $vars['idKey'], $vars['nameKey'], $vars['statusKey']);
    // Also key the returned array with the variable names so the method may
    // be easily overridden and customized.
    return drupal_map_assoc(array_keys($vars));
  }

  /**
   * Magic method to invoke setUp() on unserialization.
   */
  public function __wakeup() {
    $this->setUp();
  }
}

/**
 * These classes are deprecated by "Entity" and are only here for backward
 * compatibility reasons.
 */
class EntityDB extends Entity {}
class EntityExtendable extends Entity {}
class EntityDBExtendable extends Entity {}
