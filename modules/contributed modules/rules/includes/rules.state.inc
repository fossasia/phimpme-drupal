<?php

/**
 * @file Contains the state and data related stuff.
 */

/**
 * The rules evaluation state.
 *
 * A rule element may clone the state, so any added variables are only visible
 * for elements in the current PHP-variable-scope.
 */
class RulesState {

  /**
   * Globally keeps the ids of rules blocked due to recursion prevention.
   */
  static protected $blocked = array();

  /**
   * The known variables.
   */
  public $variables = array();

  /**
   * Holds info about the variables.
   */
  protected $info = array();

  /**
   * Keeps wrappers to be saved later on.
   */
  protected $save;

  /**
   * Holds the arguments while an element is executed. May be used by the
   * element to easily access the wrapped arguments.
   */
  public $currentArguments;

  /**
   * Variable for saving currently blocked configs for serialization.
   */
  protected $currentlyBlocked;


  public function __construct() {
    // Use an object in order to ensure any cloned states reference the same
    // save information.
    $this->save = new ArrayObject();
    $this->addVariable('site', FALSE, self::defaultVariables('site'));
  }

  /**
   * Adds the given variable to the given execution state.
   */
  public function addVariable($name, $data, $info) {
    $this->info[$name] = $info + array(
      'skip save' => FALSE,
      'type' => 'unknown',
      'handler' => FALSE,
    );
    if (empty($this->info[$name]['handler'])) {
      $this->variables[$name] = rules_wrap_data($data, $this->info[$name]);
    }
  }

  /**
   * Runs post-evaluation tasks, such as saving variables.
   */
  public function cleanUp() {
    // Make changes permanent.
    foreach ($this->save->getArrayCopy() as $selector => $wrapper) {
      $this->saveNow($selector);
    }
    unset($this->currentArguments);
  }

  /**
   * Block a rules configuration from execution.
   */
  public function block($rules_config) {
    if (empty($rules_config->recursion) && $rules_config->id) {
      self::$blocked[$rules_config->id] = TRUE;
    }
  }

  /**
   * Unblock a rules configuration from execution.
   */
  public function unblock($rules_config) {
    if (empty($rules_config->recursion) && $rules_config->id) {
      unset(self::$blocked[$rules_config->id]);
    }
  }

  /**
   * Returns whether a rules configuration should be blocked from execution.
   */
  public function isBlocked($rule_config) {
    return !empty($rule_config->id) && isset(self::$blocked[$rule_config->id]);
  }

  /**
   * Get the info about the state variables or a single variable.
   */
  public function varInfo($name = NULL) {
    if (isset($name)) {
      return isset($this->info[$name]) ? $this->info[$name] : FALSE;
    }
    return $this->info;
  }

  /**
   * Returns whether the given wrapper is savable.
   */
  public function isSavable($wrapper) {
    return ($wrapper instanceof EntityDrupalWrapper && entity_type_supports($wrapper->type(), 'save')) || $wrapper instanceof RulesDataWrapperSavableInterface;
  }

  /**
   * Returns whether the variable with the given name is an entity.
   */
  public function isEntity($name) {
    $entity_info = entity_get_info();
    return isset($this->info[$name]['type']) && isset($entity_info[$this->info[$name]['type']]);
  }

  /**
   * Gets a variable.
   *
   * If necessary, the specified handler is invoked to fetch the variable.
   *
   * @param $name
   *   The name of the variable to return.
   *
   * @return
   *   The variable or a EntityMetadataWrapper containing the variable.
   *
   * @throws RulesEvaluationException
   *   Throws a RulesEvaluationException in case we have info about the
   *   requested variable, but it is not defined.
   */
  public function &get($name) {
    if (!array_key_exists($name, $this->variables)) {
      // If there is handler to load the variable, do it now.
      if (!empty($this->info[$name]['handler'])) {
        $data = call_user_func($this->info[$name]['handler'], rules_unwrap_data($this->variables), $name, $this->info[$name]);
        $this->variables[$name] = rules_wrap_data($data, $this->info[$name]);
        $this->info[$name]['handler'] = FALSE;
        if (!isset($data)) {
          throw new RulesEvaluationException('Unable to load variable %name, aborting.', array('%name' => $name), NULL, RulesLog::INFO);
        }
      }
      else {
        throw new RulesEvaluationException('Unable to get variable %name, it is not defined.', array('%name' => $name), NULL, RulesLog::ERROR);
      }
    }
    return $this->variables[$name];
  }

  /**
   * Apply permanent changes provided the wrapper's data type is savable.
   *
   * @param $selector
   *   The data selector of the wrapper to save or just a variable name.
   * @param $immediate
   *   Pass FALSE to postpone saving to later on. Else it's immediately saved.
   */
  public function saveChanges($selector, $wrapper, $immediate = FALSE) {
    $info = $wrapper->info();
    if (empty($info['skip save']) && $this->isSavable($wrapper)) {
      $this->save($selector, $wrapper, $immediate);
    }
    // No entity, so try saving the parent.
    elseif (empty($info['skip save']) && isset($info['parent']) && !($wrapper instanceof EntityDrupalWrapper)) {
      // Cut of the last part of the selector.
      $selector = implode(':', explode(':', $selector, -1));
      $this->saveChanges($selector, $info['parent'], $immediate);
    }
    return $this;
  }

  /**
   * Remembers to save the wrapper on cleanup or does it now.
   */
  protected function save($selector, EntityMetadataWrapper $wrapper, $immediate) {
    // Convert variable names and selectors to both use underscores.
    $selector = strtr($selector, '-', '_');
    if (isset($this->save[$selector])) {
      if ($this->save[$selector][0]->getIdentifier() == $wrapper->getIdentifier()) {
        // The entity is already remembered. So do a combined save.
        $this->save[$selector][1] += self::$blocked;
      }
      else {
        // The wrapper is already in there, but wraps another entity. So first
        // save the old one, then care about the new one.
        $this->saveNow($selector);
      }
    }
    if (!isset($this->save[$selector])) {
      // In case of immediate saving don't clone the wrapper, so saving a new
      // entity immediately makes the identifier available afterwards.
      $this->save[$selector] = array($immediate ? $wrapper : clone $wrapper, self::$blocked);
    }
    if ($immediate) {
      $this->saveNow($selector);
    }
  }

  /**
   * Saves the wrapper for the given selector.
   */
  protected function saveNow($selector) {
    // Add the set of blocked elements for the recursion prevention.
    $previously_blocked = self::$blocked;
    self::$blocked += $this->save[$selector][1];

    // Actually save!
    $wrapper = $this->save[$selector][0];
    $entity = $wrapper->value();
    // When operating in hook_entity_insert() $entity->is_new might be still
    // set. In that case remove the flag to avoid causing another insert instead
    // of an update.
    if (!empty($entity->is_new) && $wrapper->getIdentifier()) {
      $entity->is_new = FALSE;
    }
    rules_log('Saved %selector of type %type.', array('%selector' => $selector, '%type' => $wrapper->type()));
    $wrapper->save();

    // Restore the state's set of blocked elements.
    self::$blocked = $previously_blocked;
    unset($this->save[$selector]);
  }

  /**
   * Merges the info about to be saved variables form the given state into the
   * existing state. Therefor we can aggregate saves from invoked components.
   * Merged in saves are removed from the given state, but not mergable saves
   * remain there.
   *
   * @param $state
   *   The state for which to merge the to be saved variables in.
   * @param $component
   *   The component which has been invoked, thus needs to be blocked for the
   *   merged in saves.
   * @param $settings
   *   The settings of the element that invoked the component. Contains
   *   information about variable/selector mappings between the states.
   */
  public function mergeSaveVariables(RulesState $state, RulesPlugin $component, $settings) {
    // For any saves that we take over, also block the component.
    $this->block($component);

    foreach ($state->save->getArrayCopy() as $selector => $data) {
      $parts = explode(':', $selector, 2);
      // Adapt the selector to fit for the parent state and move the wrapper.
      if (isset($settings[$parts[0] . ':select'])) {
        $parts[0] = $settings[$parts[0] . ':select'];
        $this->save(implode(':', $parts), $data[0], FALSE);
        unset($state->save[$selector]);
      }
    }
    $this->unblock($component);
  }

  /**
   * Returns an entity metadata wrapper as specified in the selector.
   *
   * @param $selector
   *   The selector string, e.g. "node:author:mail".
   * @param $langcode
   *   (optional) The language code used to get the argument value if the
   *   argument value should be translated. Defaults to LANGUAGE_NONE.
   *
   * @return EntityMetadataWrapper
   *   The wrapper for the given selector.
   *
   * @throws RulesEvaluationException
   *   Throws a RulesEvaluationException in case the selector cannot be applied.
   */
  public function applyDataSelector($selector, $langcode = LANGUAGE_NONE) {
    $parts = explode(':', str_replace('-', '_', $selector), 2);
    $wrapper = $this->get($parts[0]);
    if (count($parts) == 1) {
      return $wrapper;
    }
    elseif (!$wrapper instanceof EntityMetadataWrapper) {
      throw new RulesEvaluationException('Unable to apply data selector %selector. The specified variable is not wrapped correctly.', array('%selector' => $selector));
    }
    try {
      foreach (explode(':', $parts[1]) as $name) {
        if ($wrapper instanceof EntityListWrapper || $wrapper instanceof EntityStructureWrapper) {
          // Make sure we are usign the right language. Wrappers might be cached
          // and have previous langcodes set, so always set the right language.
          if ($wrapper instanceof EntityStructureWrapper) {
            $wrapper->language($langcode);
          }
          $wrapper = $wrapper->get($name);
        }
        else {
          throw new RulesEvaluationException('Unable to apply data selector %selector. The specified variable is not a list or a structure: %wrapper.', array('%selector' => $selector, '%wrapper' => $wrapper));
        }
      }
    }
    catch (EntityMetadataWrapperException $e) {
      // In case of an exception, re-throw it.
      throw new RulesEvaluationException('Unable to apply data selector %selector: %error', array('%selector' => $selector, '%error' => $e->getMessage()));
    }
    return $wrapper;
  }

  /**
   * Magic method. Only serialize variables and their info.
   * Additionally we remember currently blocked configs, so we can restore them
   * upon deserialization using restoreBlocks().
   */
  public function __sleep () {
    $this->currentlyBlocked = self::$blocked;
    return array('info', 'variables', 'currentlyBlocked');
  }

  public function __wakeup() {
    $this->save = new ArrayObject();
  }

  /**
   * Restore the before serialization blocked configurations.
   *
   * Warning: This overwrites any possible currently blocked configs. Thus
   * do not invoke this method, if there might be evaluations active.
   */
  public function restoreBlocks() {
    self::$blocked = $this->currentlyBlocked;
  }

  /**
   * Defines always available variables.
   */
  public static function defaultVariables($key = NULL) {
    // Add a variable for accessing site-wide data properties.
    $vars['site'] = array(
      'type' => 'site',
      'label' => t('Site information'),
      'description' => t("Site-wide settings and other global information."),
      // Add the property info via a callback making use of the cached info.
      'property info alter' => array('RulesData', 'addSiteMetadata'),
      'property info' => array(),
      'optional' => TRUE,
    );
    return isset($key) ? $vars[$key] : $vars;
  }
}

/**
 * A class holding static methods related to data.
 */
class RulesData  {

  /**
   * Returns whether the type match. They match if type1 is compatible to type2.
   *
   * @param $var_info
   *   The name of the type to check for whether it is compatible to type2.
   * @param $param_info
   *   The type expression to check for.
   * @param $ancestors
   *   Whether sub-type relationships for checking type compatibility should be
   *   taken into account. Defaults to TRUE.
   *
   * @return
   *   Whether the types match.
   */
  public static function typesMatch($var_info, $param_info, $ancestors = TRUE) {
    $var_type = $var_info['type'];
    $param_type = $param_info['type'];

    if ($param_type == '*' || $param_type == 'unknown') {
      return TRUE;
    }

    if ($var_type == $param_type) {
      // Make sure the bundle matches, if specified by the parameter.
      return !isset($param_info['bundles']) || isset($var_info['bundle']) && in_array($var_info['bundle'], $param_info['bundles']);
    }

    // Parameters may specify multiple types using an array.
    $valid_types = is_array($param_type) ? $param_type : array($param_type);
    if (in_array($var_type, $valid_types)) {
      return TRUE;
    }

    // Check for sub-type relationships.
    if ($ancestors && !isset($param_info['bundles'])) {
      $cache = &rules_get_cache();
      self::typeCalcAncestors($cache, $var_type);
      // If one of the types is an ancestor return TRUE.
      return (bool)array_intersect_key($cache['data_info'][$var_type]['ancestors'], array_flip($valid_types));
    }
    return FALSE;
  }

  protected static function typeCalcAncestors(&$cache, $type) {
    if (!isset($cache['data_info'][$type]['ancestors'])) {
      $cache['data_info'][$type]['ancestors'] = array();
      if (isset($cache['data_info'][$type]['parent']) && $parent = $cache['data_info'][$type]['parent']) {
        $cache['data_info'][$type]['ancestors'][$parent] = TRUE;
        self::typeCalcAncestors($cache, $parent);
        // Add all parent ancestors to our own ancestors.
        $cache['data_info'][$type]['ancestors'] += $cache['data_info'][$parent]['ancestors'];
      }
      // For special lists like list<node> add in "list" as valid parent.
      if (entity_property_list_extract_type($type)) {
        $cache['data_info'][$type]['ancestors']['list'] = TRUE;
      }
    }
  }

  /**
   * Returns matching data variables or properties for the given info and the to
   * be configured parameter.
   *
   * @param $source
   *   Either an array of info about available variables or a entity metadata
   *   wrapper.
   * @param $param_info
   *   The information array about the to be configured parameter.
   * @param $prefix
   *   An optional prefix for the data selectors.
   * @param $recursions
   *   The number of recursions used to go down the tree. Defaults to 2.
   * @param $suggestions
   *   Whether possibilities to recurse are suggested as soon as the deepest
   *   level of recursions is reached. Defaults to TRUE.
   *
   * @return
   *  An array of info about matching variables or properties that match, keyed
   *  with the data selector.
   */
  public static function matchingDataSelector($source, $param_info, $prefix = '', $recursions = 2, $suggestions = TRUE) {
    // If an array of info is given, get entity metadata wrappers first.
    $data = NULL;
    if (is_array($source)) {
      foreach ($source as $name => $info) {
        $source[$name] = rules_wrap_data($data, $info, TRUE);
      }
    }

    $matches = array();
    foreach ($source as $name => $wrapper) {
      $info = $wrapper->info();
      $name = str_replace('_', '-', $name);

      if (self::typesMatch($info, $param_info)) {
        $matches[$prefix . $name] = $info;
        if (!is_array($source) && $source instanceof EntityListWrapper) {
          // Add some more possible list items.
          for ($i = 1; $i < 4; $i++) {
            $matches[$prefix . $i] = $info;
          }
        }
      }
      // Recurse later on to get an improved ordering of the results.
      if ($wrapper instanceof EntityStructureWrapper || $wrapper instanceof EntityListWrapper) {
        $recurse[$prefix . $name] = $wrapper;
        if ($recursions > 0) {
          $matches += self::matchingDataSelector($wrapper, $param_info, $prefix . $name . ':', $recursions - 1, $suggestions);
        }
        elseif ($suggestions) {
          // We may not recurse any more, but indicate the possibility to recurse.
          $matches[$prefix . $name . ':'] = $wrapper->info();
          if (!is_array($source) && $source instanceof EntityListWrapper) {
            // Add some more possible list items.
            for ($i = 1; $i < 4; $i++) {
              $matches[$prefix . $i . ':'] = $wrapper->info();
            }
          }
        }
      }
    }
    return $matches;
  }

  /**
   * Adds asserted metadata to the variable info. In case there are already
   * assertions for a variable, the assertions are merged such that both apply.
   *
   * @see RulesData::applyMetadataAssertions()
   */
  public static function addMetadataAssertions($var_info, $assertions) {
    foreach ($assertions as $selector => $assertion) {
      // Convert the selector back to underscores, such it matches the varname.
      $selector = str_replace('-', '_', $selector);

      $parts = explode(':', $selector);
      if (isset($var_info[$parts[0]])) {
        // Apply the selector to determine the right target array. We build an
        // array like
        // $var_info['rules assertion']['property1']['property2']['#info'] = ..
        $target = &$var_info[$parts[0]]['rules assertion'];
        foreach (array_slice($parts, 1) as $part) {
          $target = &$target[$part];
        }

        // In case the assertion is directly for a variable, we have to modify
        // the variable info directly. In case the asserted property is nested
        // the info-has to be altered by RulesData::applyMetadataAssertions()
        // before the child-wrapper is created.
        if (count($parts) == 1) {
          // Support asserting a type in case of generic entity references only.
          if (isset($assertion['type']) && $var_info[$parts[0]]['type'] == 'entity') {
            if (entity_get_info($assertion['type'])) {
              $var_info[$parts[0]]['type'] = $assertion['type'];
            }
            unset($assertion['type']);
          }
          // Add any single bundle directly to the variable info, so the
          // variable fits as argument for parameters requiring the bundle.
          if (isset($assertion['bundle']) && count($bundles = (array) $assertion['bundle']) == 1) {
            $var_info[$parts[0]]['bundle'] = reset($bundles);
          }
        }

        // Add the assertions, but merge them with any previously added
        // assertions if necessary.
        $target['#info'] = isset($target['#info']) ? rules_update_array($target['#info'], $assertion) : $assertion;

        // Add in a callback that the entity metadata wrapper pick up for
        // altering the property info, such that we can add in the assertions.
        $var_info[$parts[0]] += array('property info alter' => array('RulesData', 'applyMetadataAssertions'));

        // In case there is a VARNAME_unchanged variable as it is used in update
        // hooks, assume the assertions are valid for the unchanged variable
        // too.
        if (isset($var_info[$parts[0] . '_unchanged'])) {
          $name = $parts[0] . '_unchanged';
          $var_info[$name]['rules assertion'] = $var_info[$parts[0]]['rules assertion'];
          $var_info[$name]['property info alter'] = array('RulesData', 'applyMetadataAssertions');

          if (isset($var_info[$parts[0]]['bundle']) && !isset($var_info[$name]['bundle'])) {
            $var_info[$name]['bundle'] = $var_info[$parts[0]]['bundle'];
          }
        }
      }
    }
    return $var_info;
  }

  /**
   * Property info alter callback for the entity metadata wrapper for applying
   * the rules metadata assertions.
   *
   * @see RulesData::addMetadataAssertions()
   */
  public static function applyMetadataAssertions(EntityMetadataWrapper $wrapper, $property_info) {
    $info = $wrapper->info();

    if (!empty($info['rules assertion'])) {
      $assertion = $info['rules assertion'];

      // In case there are list-wrappers pass through the assertions of the item
      // but make sure we only apply the assertions for the list items for
      // which the conditions are executed.
      if (isset($info['parent']) && $info['parent'] instanceof EntityListWrapper) {
        $assertion = isset($assertion[$info['name']]) ? $assertion[$info['name']] : array();
      }

      // Support specifying multiple bundles, whereas the added properties are
      // the intersection of the bundle properties.
      if (isset($assertion['#info']['bundle'])) {
        $bundles = (array) $assertion['#info']['bundle'];
        foreach ($bundles as $bundle) {
          $properties[] = isset($property_info['bundles'][$bundle]['properties']) ? $property_info['bundles'][$bundle]['properties'] : array();
        }
        // Add the intersection.
        $property_info['properties'] += count($properties) > 1 ? call_user_func_array('array_intersect_key', $properties) : reset($properties);
      }
      // Support adding directly asserted property info.
      if (isset($assertion['#info']['property info'])) {
        $property_info['properties'] += $assertion['#info']['property info'];
      }

      // Pass through any rules assertion of properties to their info, so any
      // derived wrappers apply it.
      foreach (element_children($assertion) as $key) {
        $property_info['properties'][$key]['rules assertion'] = $assertion[$key];
        $property_info['properties'][$key]['property info alter'] = array('RulesData', 'applyMetadataAssertions');

        // Apply any 'type' and 'bundle' assertion directly to the propertyinfo.
        if (isset($assertion[$key]['#info']['type'])) {
          $type = $assertion[$key]['#info']['type'];
          // Support asserting a type in case of generic entity references only.
          if ($property_info['properties'][$key]['type'] == 'entity' && entity_get_info($type)) {
            $property_info['properties'][$key]['type'] = $type;
          }
        }
        if (isset($assertion[$key]['#info']['bundle'])) {
          $bundle = (array) $assertion[$key]['#info']['bundle'];
          // Add any single bundle directly to the variable info, so the
          // property fits as argument for parameters requiring the bundle.
          if (count($bundle) == 1) {
            $property_info['properties'][$key]['bundle'] = reset($bundle);
          }
        }
      }
    }
    return $property_info;
  }

  /**
   * Property info alter callback for the entity metadata wrapper to inject
   * metadata for the 'site' variable. In contrast to doing this via
   * hook_rules_data_info() this callback makes use of the already existing
   * property info cache for site information of entity metadata.
   *
   * @see RulesPlugin::availableVariables()
   */
  public static function addSiteMetadata(EntityMetadataWrapper $wrapper, $property_info) {
    $site_info = entity_get_property_info('site');
    $property_info['properties'] += $site_info['properties'];
    // Also invoke the usual callback for altering metadata, in case actions
    // have specified further metadata.
    return RulesData::applyMetadataAssertions($wrapper, $property_info);
  }
}

/**
 * A wrapper class similar to the EntityDrupalWrapper, but for non-entities.
 *
 * This class is intended to serve as base for a custom wrapper classes of
 * identifiable data types, which are non-entities. By extending this class only
 * the extractIdentifier() and load() methods have to be defined.
 * In order to make the data type savable implement the
 * RulesDataWrapperSavableInterface.
 *
 * That way it is possible for non-entity data types to be work with Rules, i.e.
 * one can implement a 'ui class' with a direct input form returning the
 * identifier of the data. However, instead of that it is suggested to implement
 * an entity type, such that the same is achieved via general API functions like
 * entity_load().
 */
abstract class RulesIdentifiableDataWrapper extends EntityStructureWrapper {

  /**
   * Contains the id.
   */
  protected $id = FALSE;

  /**
   * Construct a new wrapper object.
   *
   * @param $type
   *   The type of the passed data.
   * @param $data
   *   Optional. The data to wrap or its identifier.
   * @param $info
   *   Optional. Used internally to pass info about properties down the tree.
   */
  public function __construct($type, $data = NULL, $info = array()) {
    parent::__construct($type, $data, $info);
    $this->setData($data);
  }

  /**
   * Sets the data internally accepting both the data id and object.
   */
  protected function setData($data) {
    if (isset($data) && $data !== FALSE && !is_object($data)) {
      $this->id = $data;
      $this->data = FALSE;
    }
    elseif (is_object($data)) {
      // We got the data object passed.
      $this->data = $data;
      $id = $this->extractIdentifier($data);
      $this->id = isset($id) ? $id : FALSE;
    }
  }

  /**
   * Returns the identifier of the wrapped data.
   */
  public function getIdentifier() {
    return $this->dataAvailable() && $this->value() ? $this->id : NULL;
  }

  /**
   * Overridden.
   */
  public function value(array $options = array()) {
    $this->setData(parent::value());
    if (!$this->data && !empty($this->id)) {
      // Lazy load the data if necessary.
      $this->data = $this->load($this->id);
      if (!$this->data) {
        throw new EntityMetadataWrapperException('Unable to load the ' . check_plain($this->type) . ' with the id ' . check_plain($this->id) . '.');
      }
    }
    return $this->data;
  }

  /**
   * Overridden to support setting the data by either the object or the id.
   */
  public function set($value) {
    if (!$this->validate($value)) {
      throw new EntityMetadataWrapperException('Invalid data value given. Be sure it matches the required data type and format.');
    }
    // As custom wrapper classes can only appear for Rules variables, but not
    // as properties we don't have to care about updating the parent.
    $this->clear();
    $this->setData($value);
    return $this;
  }

  /**
   * Overridden.
   */
  public function clear() {
    $this->id = NULL;
    parent::clear();
  }

  /**
   * Prepare for serializiation.
   */
  public function __sleep() {
    $vars = parent::__sleep();
    // Don't serialize the loaded data, except for the case the data is not
    // saved yet.
    if (!empty($this->id)) {
      unset($vars['data']);
    }
    return $vars;
  }

  public function __wakeup() {
    if ($this->id !== FALSE) {
      // Make sure data is set, so the data will be loaded when needed.
      $this->data = FALSE;
    }
  }

  /**
   * Extract the identifier of the given data object.
   *
   * @return
   *   The extracted identifier.
   */
  abstract protected function extractIdentifier($data);

  /**
   * Load a data object given an identifier.
   *
   * @return
   *   The loaded data object, or FALSE if loading failed.
   */
  abstract protected function load($id);
}

/**
 * Interface that allows custom wrapper classes to declare that they are savable.
 */
interface RulesDataWrapperSavableInterface {

  /**
   * Save the currently wrapped data.
   */
  public function save();
}
