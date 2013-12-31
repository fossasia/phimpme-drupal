<?php

/**
 * @file
 * Provides wrappers allowing easy usage of the entity metadata.
 */

/**
 * A common base class for all wrappers.
 */
abstract class EntityMetadataWrapper {

  protected $type;
  protected $data;
  protected $info;
  protected $cache = array();

  /**
   * Construct a new wrapper object.
   *
   * @param $type
   *   The type of the passed data.
   * @param $data
   *   Optional. The data to wrap.
   * @param $info
   *   Optional. Used internally to pass info about properties down the tree.
   */
  public function __construct($type, $data = NULL, $info = array()) {
    $this->type = $type;
    $this->info = $info + array(
      'langcode' => NULL,
    );
    $this->info['type'] = $type;
    if (isset($data)) {
      $this->set($data);
    }
  }

  /**
   * Gets info about the wrapped data.
   *
   * @return Array
   *   Keys set are all keys as specified for a property in hook_entity_info()
   *   as well as possible the following keys:
   *    - name: If this wraps a property, the name of the property.
   *    - parent: The parent wrapper, if any.
   *    - langcode: The language code, if this data is language specific.
   */
  public function info() {
    return $this->info;
  }

  /**
   * Gets the (entity)type of the wrapped data.
   */
  public function type() {
    return $this->type;
  }

  /**
   * Returns the wrapped data. If no options are given the data is returned as
   * described in the info.
   *
   * @param $options
   *   (optional) A keyed array of options:
   *   - sanitize: A boolean flag indicating that textual properties should be
   *     sanitized for display to a web browser. Defaults to FALSE.
   *   - decode: If set to TRUE and some textual data is already sanitized, it
   *     strips HTML tags and decodes HTML entities. Defaults to FALSE.
   *
   *  @return
   *    The value of the wrapped data. If the data property is not set, NULL
   *    is returned.
   *
   *  @throws EntityMetadataWrapperException
   *    In case there are no data values available to the wrapper, an exception
   *    is thrown. E.g. if the value for an entity property is to be retrieved
   *    and there is no entity available, the exception is thrown. However, if
   *    an entity is available but the property is not set, NULL is returned.
   */
  public function value(array $options = array()) {
    if (!$this->dataAvailable() && isset($this->info['parent'])) {
      throw new EntityMetadataWrapperException('Missing data values.');
    }
    if (!isset($this->data) && isset($this->info['name'])) {
      $this->data = $this->info['parent']->getPropertyValue($this->info['name'], $this->info);
    }
    return $this->data;
  }

  /**
   * Returns the raw, unprocessed data. Most times this is the same as returned
   * by value(), however for already processed and sanitized textual data, this
   * will return the unprocessed data in contrast to value().
   */
  public function raw() {
    if (!$this->dataAvailable()) {
      throw new EntityMetadataWrapperException('Missing data values.');
    }
    if (isset($this->info['name']) && isset($this->info['parent'])) {
      return $this->info['parent']->getPropertyRaw($this->info['name'], $this->info);
    }
    // Else return the usual value, which should be raw in this case.
    return $this->value();
  }

  /**
   * Returns whether data is available to work with.
   *
   * @return
   *   If we operate without any data FALSE, else TRUE.
   */
  protected function dataAvailable() {
    return isset($this->data) || (isset($this->info['parent']) && $this->info['parent']->dataAvailable());
  }

  /**
   * Set a new data value.
   */
  public function set($value) {
    if (!$this->validate($value)) {
      throw new EntityMetadataWrapperException('Invalid data value given. Be sure it matches the required data type and format.');
    }
    $this->clear();
    $this->data = $value;
    $this->updateParent($value);
    return $this;
  }

  /**
   * Updates the parent data structure of a data property with the latest data value.
   */
  protected function updateParent($value) {
    if (isset($this->info['parent'])) {
      $this->info['parent']->setProperty($this->info['name'], $value);
    }
  }

  /**
   * Returns whether $value is a valid value to set.
   */
  public function validate($value) {
    if (isset($value) && !entity_property_verify_data_type($value, $this->type)) {
      return FALSE;
    }
    // Only proceed with further checks if this is not a list item. If this is
    // a list item, the checks are performed on the list property level.
    if (isset($this->info['parent']) && $this->info['parent'] instanceof EntityListWrapper) {
      return TRUE;
    }
    if (!isset($value) && !empty($this->info['required'])) {
      // Do not allow NULL values if the property is required.
      return FALSE;
    }
    return !isset($this->info['validation callback']) || call_user_func($this->info['validation callback'], $value, $this->info);
  }

  public function __toString() {
    return isset($this->info) ? 'Property ' . $this->info['name'] : $this->type;
  }

  /**
   * Clears the data value and the wrapper cache.
   */
  protected function clear() {
    $this->data = NULL;
    foreach ($this->cache as $wrapper) {
      $wrapper->clear();
    }
  }

  /**
   * Returns the options list specifying possible values for the property, if
   * defined.
   *
   * @param $op
   *   (optional) One of 'edit' or 'view'. In case the list of possible values
   *   a user could set for a property differs from the list of values a
   *   property could have, $op determines which options should be returned.
   *   Defaults to 'edit'.
   *   E.g. all possible roles a user could have include the anonymous and the
   *   authenticated user roles, while those roles cannot be added to a user
   *   account. So their options would be included for 'view', but for 'edit'
   *   not.
   *
   * @return
   *   An array as used by hook_options_list() or FALSE.
   */
  public function optionsList($op = 'edit') {
    if (isset($this->info['options list']) && is_callable($this->info['options list'])) {
      $name = isset($this->info['name']) ? $this->info['name'] : NULL;
      return call_user_func($this->info['options list'], $name, $this->info, $op);
    }
    return FALSE;
  }

  /**
   * Returns the label for the currently set property value if there is one
   * available, i.e. if an options list has been specified.
   */
  public function label() {
    if ($options = $this->optionsList('view')) {
      $options = entity_property_options_flatten($options);
      $value = $this->value();
      if (is_scalar($value) && isset($options[$value])) {
        return $options[$value];
      }
    }
  }

  /**
   * Determines whether the given user has access to view or edit this property.
   * Apart from relying on access metadata of properties, this takes into
   * account information about entity level access, if available:
   *  - Referenced entities can only be viewed, when the user also has
   *    permission to view the entity.
   *  - A property may be only edited, if the user has permission to update the
   *    entity containing the property.
   *
   * @param $op
   *   The operation being performed. One of 'view' or 'edit.
   * @param $account
   *   The user to check for. Leave it to NULL to check for the global user.
   * @return boolean
   *   Whether access to entity property is allowed for the given operation.
   *   However if we wrap no data, it returns whether access is allowed to the
   *   property of all entities of this type.
   *   If there is no access information for this property, TRUE is returned.
   */
  public function access($op, $account = NULL) {
    if (empty($this->info['parent']) && $this instanceof EntityDrupalWrapper) {
      // If there is no parent just incorporate entity based access.
      return $this->entityAccess($op == 'edit' ? 'update' : 'view', $account);
    }
    return !empty($this->info['parent']) ? $this->info['parent']->propertyAccess($this->info['name'], $op, $account) : TRUE;
  }

  /**
   * Prepare for serializiation.
   */
  public function __sleep() {
    $vars = get_object_vars($this);
    unset($vars['cache']);
    return drupal_map_assoc(array_keys($vars));
  }
}

/**
 * Wraps a single value.
 */
class EntityValueWrapper extends EntityMetadataWrapper {

  /**
   * Overrides EntityMetadataWrapper#value().
   * Sanitizes or decode textual data if necessary.
   */
  public function value(array $options = array()) {
    $data = parent::value();
    if ($this->type == 'text' && isset($data)) {
      $info = $this->info + array('sanitized' => FALSE, 'sanitize' => 'check_plain');
      $options += array('sanitize' => FALSE, 'decode' => FALSE);
      if ($options['sanitize'] && !$info['sanitized']) {
        return call_user_func($info['sanitize'], $data);
      }
      elseif ($options['decode'] && $info['sanitized']) {
        return decode_entities(strip_tags($data));
      }
    }
    return $data;
  }
}

/**
 * Provides a general wrapper for any data structure. For this to work the
 * metadata has to be passed during construction.
 */
class EntityStructureWrapper extends EntityMetadataWrapper implements IteratorAggregate {

  protected $propertyInfo = array(), $propertyInfoAltered = FALSE;
  protected $langcode = LANGUAGE_NONE;

  protected $propertyInfoDefaults = array(
    'type' => 'text',
    'getter callback' => 'entity_property_verbatim_get',
    'clear' => array(),
  );

  /**
   * Construct a new EntityStructureWrapper object.
   *
   * @param $type
   *   The type of the passed data.
   * @param $data
   *   Optional. The data to wrap.
   * @param $info
   *   Used to for specifying metadata about the data and internally to pass
   *   info about properties down the tree. For specifying metadata known keys
   *   are:
   *   - property info: An array of info about the properties of the wrapped
   *     data structure. It has to contain an array of property info in the same
   *     structure as used by hook_entity_property_info().
   */
  public function __construct($type, $data = NULL, $info = array()) {
    parent::__construct($type, $data, $info);
    $this->info += array('property defaults' => array());
    $info += array('property info' => array());
    $this->propertyInfo['properties'] = $info['property info'];
  }

  /**
   * May be used to lazy-load additional info about the data, depending on the
   * concrete passed data.
   */
  protected function spotInfo() {
    // Apply the callback if set, such that the caller may alter the info.
    if (!empty($this->info['property info alter']) && !$this->propertyInfoAltered) {
      $this->propertyInfo = call_user_func($this->info['property info alter'], $this, $this->propertyInfo);
      $this->propertyInfoAltered = TRUE;
    }
  }

  /**
   * Gets the info about the given property.
   *
   * @param $name
   *   The name of the property. If not given, info about all properties will
   *   be returned.
   * @throws EntityMetadataWrapperException
   *   If there is no such property.
   * @return
   *   An array of info about the property.
   */
  public function getPropertyInfo($name = NULL) {
    $this->spotInfo();
    if (!isset($name)) {
      return $this->propertyInfo['properties'];
    }
    if (!isset($this->propertyInfo['properties'][$name])) {
      throw new EntityMetadataWrapperException('Unknown data property ' . check_plain($name) . '.');
    }
    return $this->propertyInfo['properties'][$name] + $this->info['property defaults'] + $this->propertyInfoDefaults;
  }

  /**
   * Returns a reference on the property info.
   *
   * If possible, use the property info alter callback for spotting metadata.
   * The reference may be used to alter the property info for any remaining
   * cases, e.g. if additional metadata has been asserted.
   */
  public function &refPropertyInfo() {
    return $this->propertyInfo;
  }

  /**
   * Sets a new language to use for retrieving properties.
   *
   * @param $langcode
   *   The language code of the language to set.
   * @return EntityWrapper
   */
  public function language($langcode = LANGUAGE_NONE) {
    if ($langcode != $this->langcode) {
      $this->langcode = $langcode;
      $this->cache = array();
    }
    return $this;
  }

  /**
   * Gets the language used for retrieving properties.
   *
   * @return String
   *   The language object of the language or NULL for the default language.
   *
   * @see EntityStructureWrapper::language()
   */
  public function getPropertyLanguage() {
    if ($this->langcode != LANGUAGE_NONE && $list = language_list()) {
      if (isset($list[$this->langcode])) {
        return $list[$this->langcode];
      }
    }
    return NULL;
  }

  /**
   * Get the wrapper for a property.
   *
   * @return
   *   An instance of EntityMetadataWrapper.
   */
  public function get($name) {
    // Look it up in the cache if possible.
    if (!array_key_exists($name, $this->cache)) {
      if ($info = $this->getPropertyInfo($name)) {
        $info += array('parent' => $this, 'name' => $name, 'langcode' => $this->langcode, 'property defaults' => array());
        $info['property defaults'] += $this->info['property defaults'];
        $this->cache[$name] = entity_metadata_wrapper($info['type'], NULL, $info);
      }
      else {
        throw new EntityMetadataWrapperException('There is no property ' . check_plain($name) . " for this entity.");
      }
    }
    return $this->cache[$name];
  }

  /**
   * Magic method: Get a wrapper for a property.
   */
  public function __get($name) {
    if (strpos($name, 'krumo') === 0) {
      // #914934 Ugly workaround to allow krumo to write its recursion property.
      // This is necessary to make dpm() work without throwing exceptions.
      return NULL;
    }
    $get = $this->get($name);
    return $get;
  }

  /**
   * Magic method: Set a property.
   */
  public function __set($name, $value) {
    if (strpos($name, 'krumo') === 0) {
      // #914934 Ugly workaround to allow krumo to write its recursion property.
      // This is necessary to make dpm() work without throwing exceptions.
      $this->$name = $value;
    }
    else {
      $this->get($name)->set($value);
    }
  }

  /**
   * Gets the value of a property.
   */
  protected function getPropertyValue($name, &$info) {
    $options = array('language' => $this->getPropertyLanguage(), 'absolute' => TRUE);
    $data = $this->value();
    if (!isset($data)) {
      throw new EntityMetadataWrapperException('Unable to get the data property ' . check_plain($name) . ' as the parent data structure is not set.');
    }
    return $info['getter callback']($data, $options, $name, $this->type, $info);
  }

  /**
   * Gets the raw value of a property.
   */
  protected function getPropertyRaw($name, &$info) {
    if (!empty($info['raw getter callback'])) {
      $options = array('language' => $this->getPropertyLanguage(), 'absolute' => TRUE);
      $data = $this->value();
      if (!isset($data)) {
        throw new EntityMetadataWrapperException('Unable to get the data property ' . check_plain($name) . ' as the parent data structure is not set.');
      }
      return $info['raw getter callback']($data, $options, $name, $this->type, $info);
    }
    return $this->getPropertyValue($name, $info);
  }

  /**
   * Sets a property.
   */
  protected function setProperty($name, $value) {
    $info = $this->getPropertyInfo($name);
    if (!empty($info['setter callback'])) {
      $data = $this->value();

      // In case the data structure is not set, support simple auto-creation
      // for arrays. Else an exception is thrown.
      if (!isset($data)) {
        if (!empty($this->info['auto creation']) && !($this instanceof EntityDrupalWrapper)) {
          $data = $this->info['auto creation']($name, $this->info);
        }
        else {
          throw new EntityMetadataWrapperException('Unable to set the data property ' . check_plain($name) . ' as the parent data structure is not set.');
        }
      }

      // Invoke the setter callback for updating our data.
      $info['setter callback']($data, $name, $value, $this->langcode, $this->type, $info);

      // If the setter has not thrown any exceptions, proceed and apply the
      // update to the current and any parent wrappers as necessary.
      $this->set($data);

      // Clear the cache of properties dependent on this value.
      foreach ($info['clear'] as $name) {
        if (isset($this->cache[$name])) {
          $this->cache[$name]->clear();
        }
      }
    }
    else {
      throw new EntityMetadataWrapperException('Entity property ' . check_plain($name) . " doesn't support writing.");
    }
  }

  protected function propertyAccess($name, $op, $account = NULL) {
    $info = $this->getPropertyInfo($name);
    // If the property should be accessed and it's an entity, make sure the user
    // is allowed to view that entity.
    if ($op == 'view' && $this->$name instanceof EntityDrupalWrapper && !$this->$name->entityAccess($op, $account)) {
      return FALSE;
    }
    // If a property should be edited and this is an entity, make sure the user
    // has update access for this entity.
    if ($op == 'edit') {
      $entity = $this;
      while (!($entity instanceof EntityDrupalWrapper) && isset($entity->info['parent'])) {
        $entity = $entity->info['parent'];
      }
      if ($entity instanceof EntityDrupalWrapper && !$entity->entityAccess('update', $account)) {
        return FALSE;
      }
    }
    if (!empty($info['access callback'])) {
      $data = $this->dataAvailable() ? $this->value() : NULL;
      return call_user_func($info['access callback'], $op, $name, $data, $account, $this->type);
    }
    elseif ($op == 'edit' && isset($info['setter permission'])) {
      return user_access($info['setter permission'], $account);
    }
    return TRUE;
  }

  /**
   * Magic method: Can be used to check if a property is known.
   */
  public function __isset($name) {
    $this->spotInfo();
    return isset($this->propertyInfo['properties'][$name]);
  }

  public function getIterator() {
    $this->spotInfo();
    return new EntityMetadataWrapperIterator($this, array_keys($this->propertyInfo['properties']));
  }

  /**
   * Returns the identifier of the data structure. If there is none, NULL is
   * returned.
   */
  public function getIdentifier() {
    return isset($this->id) && $this->dataAvailable() ? $this->id->value() : NULL;
  }

  /**
   * Prepare for serializiation.
   */
  public function __sleep() {
    $vars = parent::__sleep();
    unset($vars['propertyInfoDefaults']);
    return $vars;
  }

  public function clear() {
    $this->propertyInfoAltered = FALSE;
    parent::clear();
  }
}

/**
 * Provides a wrapper for entities registrered in hook_entity_info().
 *
 * The wrapper eases applying getter and setter callbacks of entity properties
 * specified in hook_entity_property_info().
 */
class EntityDrupalWrapper extends EntityStructureWrapper {

  /**
   * Contains the entity id.
   */
  protected $id = FALSE;
  protected $bundle;
  protected $entityInfo;

  /**
   * Construct a new EntityDrupalWrapper object.
   *
   * @param $type
   *   The type of the passed data.
   * @param $data
   *   Optional. The entity to wrap or its identifier.
   * @param $info
   *   Optional. Used internally to pass info about properties down the tree.
   */
  public function __construct($type, $data = NULL, $info = array()) {
    parent::__construct($type, $data, $info);
    $this->setUp();
  }

  protected function setUp() {
    $this->propertyInfo = entity_get_property_info($this->type) + array('properties' => array());
    $info = $this->info + array('property info' => array(), 'bundle' => NULL);
    $this->propertyInfo['properties'] += $info['property info'];
    $this->bundle = $info['bundle'];
    $this->entityInfo = entity_get_info($this->type);
    if (isset($this->bundle)) {
      $this->spotBundleInfo(FALSE);
    }
  }

  /**
   * Sets the entity internally accepting both the entity id and object.
   */
  protected function setEntity($data) {
    // For entities we allow getter callbacks to return FALSE, which we
    // interpret like NULL values as unset properties.
    if (isset($data) && $data !== FALSE && !is_object($data)) {
      $this->id = $data;
      $this->data = FALSE;
    }
    elseif (is_object($data) && $data instanceof EntityDrupalWrapper) {
      // We got a wrapped entity passed, so take over its values.
      $this->id = $data->id;
      $this->data = $data->data;
      // For generic entity references, also update the entity type accordingly.
      if ($this->info['type'] == 'entity') {
        $this->type = $data->type;
      }
    }
    elseif (is_object($data)) {
      // We got the entity object passed.
      $this->data = $data;
      $id = entity_id($this->type, $data);
      $this->id = isset($id) ? $id : FALSE;
    }
    else {
      $this->id = FALSE;
      $this->data = NULL;
    }
  }

  /**
   * Used to lazy-load bundle info. So the wrapper can be loaded e.g. just
   * for setting without the data being loaded.
   */
  protected function spotInfo() {
    if (!$this->propertyInfoAltered) {
      if ($this->info['type'] == 'entity' && $this->dataAvailable() && $this->value()) {
        // Add in entity-type specific details.
        $this->setUp();
      }
      $this->spotBundleInfo(TRUE);
      parent::spotInfo();
      $this->propertyInfoAltered = TRUE;
    }
  }

  /**
   * Tries to determine the bundle and adds in the according property info.
   *
   * @param $load
   *   Whether the entity should be loaded to spot the info if necessary.
   */
  protected function spotBundleInfo($load = TRUE) {
    // Like entity_extract_ids() assume the entity type if no key is given.
    if (empty($this->entityInfo['entity keys']['bundle']) && $this->type != 'entity') {
      $this->bundle = $this->type;
    }
    // Detect the bundle if not set yet and add in properties from the bundle.
    elseif (!$this->bundle && !empty($this->entityInfo['fieldable']) && $load && $this->dataAvailable()) {
      try {
        if ($entity = $this->value()) {
          list($id, $vid, $bundle) = entity_extract_ids($this->type, $entity);
          $this->bundle = $bundle;
        }
      }
      catch (EntityMetadataWrapperException $e) {
        // Loading data failed, so we cannot derive the used bundle.
      }
    }

    if ($this->bundle && isset($this->propertyInfo['bundles'][$this->bundle])) {
      $bundle_info = (array) $this->propertyInfo['bundles'][$this->bundle] + array('properties' => array());
      $this->propertyInfo['properties'] += $bundle_info['properties'];
    }
  }

  /**
   * Returns the identifier of the wrapped entity.
   *
   * @see entity_id()
   */
  public function getIdentifier() {
    return $this->dataAvailable() ? $this->value(array('identifier' => TRUE)) : NULL;
  }

  /**
   * Returns the bundle of an entity, or FALSE if it has no bundles.
   */
  public function getBundle() {
    if ($this->dataAvailable()) {
      $this->spotInfo();
      return $this->bundle;
    }
  }

  /**
   * Overridden.
   *
   * @param $options
   *   An array of options. Known keys:
   *   - identifier: If set to TRUE, the entity identifier is returned.
   */
  public function value(array $options = array()) {
    // Try loading the data via the getter callback if there is none yet.
    if (!isset($this->data)) {
      $this->setEntity(parent::value());
    }
    if (!empty($options['identifier'])) {
      return $this->id;
    }
    elseif (!$this->data && !empty($this->id)) {
      // Lazy load the entity if necessary.
      $return = entity_load($this->type, array($this->id));
      // In case the entity cannot be loaded, we return NULL just as for empty
      // properties.
      $this->data = $return ? reset($return) : NULL;
    }
    return $this->data;
  }

  /**
   * Overridden to support setting the entity by either the object or the id.
   */
  public function set($value) {
    if (!$this->validate($value)) {
      throw new EntityMetadataWrapperException('Invalid data value given. Be sure it matches the required data type and format.');
    }
    $previous_id = $this->id;
    $previous_type = $this->type;
    // Set value, so we get the identifier and pass it to the normal setter.
    $this->clear();
    $this->setEntity($value);
    // Generally, we have to update the parent only if the entity reference
    // has changed. In case of a generic entity reference, we pass the entity
    // wrapped. Else we just pass the id of the entity to the setter callback.
    if ($this->info['type'] == 'entity' && ($previous_id != $this->id || $previous_type != $this->type)) {
      // We need to clone the wrapper we pass through as value, so it does not
      // get cleared when the current wrapper instance gets cleared.
      $this->updateParent(clone $this);
    }
    // In case the entity has been unset, we cannot properly detect changes as
    // the previous id defaults to FALSE for unloaded entities too. So in that
    // case we just always update the parent.
    elseif ($this->id === FALSE && !$this->data) {
      $this->updateParent(NULL);
    }
    elseif ($previous_id != $this->id) {
      $this->updateParent($this->id);
    }
    return $this;
  }

  /**
   * Overridden.
   */
  public function clear() {
    $this->id = NULL;
    // Reset entity info / property info based upon the info provided during
    // the creation of the wrapper.
    $this->type = $this->info['type'];
    $this->setUp();
    parent::clear();
  }

  /**
   * Overridden.
   */
  public function type() {
    // In case of a generic entity wrapper, load the data first to determine
    // the type of the concrete entity.
    if ($this->dataAvailable() && $this->info['type'] == 'entity') {
      try {
        $this->value(array('identifier' => TRUE));
      }
      catch (EntityMetadataWrapper $e) {
        // If loading data fails, we cannot determine the concrete entity type.
      }
    }
    return $this->type;
  }

  /**
   * Checks whether the operation $op is allowed on the entity.
   *
   * @see entity_access()
   */
  public function entityAccess($op, $account = NULL) {
    $entity = $this->dataAvailable() ? $this->value() : NULL;
    return entity_access($op, $this->type, $entity, $account);
  }

  /**
   * Permanently save the wrapped entity.
   *
   * @return EntityDrupalWrapper
   */
  public function save() {
    if ($this->data) {
      if (!entity_type_supports($this->type, 'save')) {
        throw new EntityMetadataWrapperException("There is no information about how to save entities of type " . check_plain($this->type) . '.');
      }
      $return = entity_save($this->type, $this->data);
      // On insert, update the identifier afterwards.
      if (!$this->id) {
        list($this->id, , ) = entity_extract_ids($this->type, $this->data);
      }
    }
    // If the entity hasn't been loaded yet, don't bother saving it.
    return $this;
  }

  /**
   * Permanently delete the wrapped entity.
   *
   * @return EntityDrupalWrapper
   */
  public function delete() {
    if ($this->dataAvailable() && $this->value()) {
      $return = entity_delete($this->type, $this->id);
      if ($return === FALSE) {
        throw new EntityMetadataWrapperException("There is no information about how to delete entities of type " . check_plain($this->type) . '.');
      }
    }
    return $this;
  }

  /**
   * Gets the info about the wrapped entity.
   */
  public function entityInfo() {
    return $this->entityInfo;
  }

  /**
   * Returns the name of the key used by the entity for given entity key.
   *
   * @param $name
   *   One of 'id', 'name', 'bundle' or 'revision'.
   * @return
   *   The name of the key used by the entity.
   */
  public function entityKey($name) {
    return isset($this->entityInfo['entity keys'][$name]) ? $this->entityInfo['entity keys'][$name] : FALSE;
  }

  /**
   * Returns the entity label.
   *
   * @see entity_label()
   */
  public function label() {
    if ($entity = $this->value()) {
      return entity_label($this->type, $entity);
    }
  }

  /**
   * Prepare for serializiation.
   */
  public function __sleep() {
    $vars = parent::__sleep();
    // Don't serialize the loaded entity and its property info.
    unset($vars['data'], $vars['propertyInfo'], $vars['propertyInfoAltered'], $vars['entityInfo']);
    // In case the entity is not saved yet, serialize the unsaved data.
    if ($this->dataAvailable() && $this->id === FALSE) {
      $vars['data'] = 'data';
    }
    return $vars;
  }

  public function __wakeup() {
    $this->setUp();
    if ($this->id !== FALSE) {
      // Make sure data is set, so the entity will be loaded when needed.
      $this->data = FALSE;
    }
  }
}

/**
 * Wraps a list of values.
 *
 * If the wrapped data is a list of data, its numerical indexes may be used to
 * retrieve wrappers for the list items. For that this wrapper implements
 * ArrayAccess so it may be used like a usual numerically indexed array.
 */
class EntityListWrapper extends EntityMetadataWrapper implements IteratorAggregate, ArrayAccess, Countable {

  protected function itemType() {
    $type = entity_property_list_extract_type($this->type);
    return $type ? $type : 'unknown';
  }

  /**
   * Get the wrapper for a single item.
   *
   * @return
   *   An instance of EntityMetadataWrapper.
   */
  public function get($delta) {
    // Look it up in the cache if possible.
    if (!array_key_exists($delta, $this->cache)) {
      if (!isset($delta)) {
        // The [] operator has been used so point at a new entry.
        $values = parent::value();
        $delta = $values ? max(array_keys($values)) + 1 : 0;
      }
      if (is_numeric($delta)) {
        $info = array('parent' => $this, 'name' => $delta) + $this->info;
        $this->cache[$delta] = entity_metadata_wrapper($this->itemType(), NULL, $info);
      }
      else {
        throw new EntityMetadataWrapperException('There can be only numerical keyed items in a list.');
      }
    }
    return $this->cache[$delta];
  }

  protected function getPropertyValue($delta) {
    // Make use parent::value() to easily by-pass any entity-loading.
    $data = parent::value();
    if (isset($data[$delta])) {
      return $data[$delta];
    }
  }

  protected function getPropertyRaw($delta) {
    return $this->getPropertyValue($delta);
  }

  protected function setProperty($delta, $value) {
    $data = parent::value();
    if (is_numeric($delta)) {
      $data[$delta] = $value;
      $this->set($data);
    }
  }

  protected function propertyAccess($delta, $op, $account = NULL) {
    return $this->access($op, $account);
  }

  /**
   * Returns the list as numerically indexed array.
   *
   * Note that a list of entities might contain stale entity references. In
   * that case the wrapper and the identifier of a stale reference would be
   * still accessible, however the entity object value would be NULL. That way,
   * there may be NULL values in lists of entity objects due to stale entity
   * references.
   *
   * @param $options
   *   An array of options. Known keys:
   *   - identifier: If set to TRUE for a list of entities, it won't be returned
   *     as list of fully loaded entity objects, but as a list of entity ids.
   *     Note that this list may contain ids of stale entity references.
   */
  public function value(array $options = array()) {
    // For lists of entities fetch full entity objects before returning.
    // Generic entity-wrappers need to be handled separately though.
    if ($this->get(0) instanceof EntityDrupalWrapper && $this->itemType() != 'entity' && empty($options['identifier']) && $this->dataAvailable()) {
      $list = parent::value();
      $entities = $list ? entity_load($this->get(0)->type, $list) : array();
      // Make sure to keep the array keys as present in the list.
      foreach ($list as $key => $id) {
        // In case the entity cannot be loaded, we return NULL just as for empty
        // properties.
        $list[$key] = isset($entities[$id]) ? $entities[$id] : NULL;
      }
      return $list;
    }
    return parent::value();
  }

  public function set($values) {
    // Support setting lists of fully loaded entities.
    if ($this->get(0) instanceof EntityDrupalWrapper && $this->itemType() != 'entity' && $values && is_object(reset($values))) {
      foreach ($values as $key => $value) {
        list($id, $vid, $bundle) = entity_extract_ids($this->get(0)->type, $value);
        $values[$key] = $id;
      }
    }
    return parent::set($values);
  }

  /**
   * If we wrap a list, we return an iterator over the data list.
   */
  public function getIterator() {
    // In case there is no data available, just iterate over the first item.
    return new EntityMetadataWrapperIterator($this, $this->dataAvailable() ? array_keys(parent::value()) : array(0));
  }

  /**
   * Implements the ArrayAccess interface.
   */
  public function offsetGet($delta) {
    return $this->get($delta);
  }

  public function offsetExists($delta) {
    return $this->dataAvailable() && ($data = $this->value()) && isset($data[$delta]);
  }

  public function offsetSet($delta, $value) {
    $this->get($delta)->set($value);
  }

  public function offsetUnset($delta) {
    if ($this->offsetExists($delta)) {
      unset($this->data[$delta]);
      $this->set($this->data);
    }
  }

  public function count() {
    return $this->dataAvailable() ? count($this->value()) : 0;
  }

  /**
   * Overridden.
   */
  public function validate($value) {
    // Required lists may not be empty or unset.
    if (!empty($this->info['required']) && empty($value)) {
      return FALSE;
    }
    return parent::validate($value);
  }

  /**
   * Returns the label for the list of set values if available.
   */
  public function label() {
    if ($options = $this->optionsList('view')) {
      $options = entity_property_options_flatten($options);
      $labels = array_intersect_key($options, array_flip((array) parent::value()));
    }
    else {
      // Get each label on its own, e.g. to support getting labels of a list
      // of entities.
      $labels = array();
      foreach ($this as $key => $property) {
        $label = $property->label();
        if (!$label) {
          return NULL;
        }
        $labels[] = $label;
      }
    }
    return isset($labels) ? implode(', ', $labels) : NULL;
  }
}

/**
 * Provide a separate Exception so it can be caught separately.
 */
class EntityMetadataWrapperException extends Exception { }


/**
 * Allows to easily iterate over existing child wrappers.
 */
class EntityMetadataWrapperIterator implements RecursiveIterator {

  protected $position = 0;
  protected $wrapper, $keys;

  public function __construct(EntityMetadataWrapper $wrapper, array $keys) {
    $this->wrapper = $wrapper;
    $this->keys = $keys;
  }

  function rewind() {
    $this->position = 0;
  }

  function current() {
    return $this->wrapper->get($this->keys[$this->position]);
  }

  function key() {
    return $this->keys[$this->position];
  }

  function next() {
    $this->position++;
  }

  function valid() {
    return isset($this->keys[$this->position]);
  }

  public function hasChildren () {
    return $this->current() instanceof IteratorAggregate;
  }

  public function getChildren () {
    return $this->current()->getIterator();
  }
}

/**
 * An array object implementation keeping the reference on the given array so
 * changes to the object are reflected in the passed array.
 */
class EntityMetadataArrayObject implements ArrayAccess, Countable, IteratorAggregate {

  protected $data;

  public function __construct(&$array) {
    $this->data =& $array;
  }

  public function &getArray() {
    return $this->data;
  }

  /**
   * Implements the ArrayAccess interface.
   */
  public function offsetGet($delta) {
    return $this->data[$delta];
  }

  public function offsetExists($delta) {
    return isset($this->data[$delta]);
  }

  public function offsetSet($delta, $value) {
    $this->data[$delta] = $value;
  }

  public function offsetUnset($delta) {
    unset($this->data[$delta]);
  }

  public function count() {
    return count($this->data);
  }

  public function getIterator() {
    return new ArrayIterator($this->data);
  }
}
