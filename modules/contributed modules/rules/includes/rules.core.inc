<?php

/**
 * @file
 * Rules base classes and interfaces needed for any rule evaluation.
 */

// This is not necessary as the classes are autoloaded via the registry. However
// it saves some possible update headaches until the registry is rebuilt.
// @todo
//   Remove for a future release.
require_once dirname(__FILE__) . '/faces.inc';

/**
 * Make sure loaded rule configs are instantiated right.
 */
class RulesEntityController extends EntityAPIControllerExportable {

  /**
   * Overriden.
   * @see DrupalDefaultEntityController::attachLoad()
   */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    // Retrieve stdClass records and turn them in rules objects stored in 'data'
    $ids = array_keys($queried_entities);
    $result = db_select('rules_tags')
      ->fields('rules_tags', array('id', 'tag'))
      ->condition('id', $ids, 'IN')
      ->execute();
    foreach ($result as $row) {
      $tags[$row->id][] = $row->tag;
    }
    $result = db_select('rules_dependencies')
      ->fields('rules_dependencies', array('id', 'module'))
      ->condition('id', $ids, 'IN')
      ->execute();
    foreach ($result as $row) {
      $modules[$row->id][] = $row->module;
    }

    $entities = array();
    foreach ($queried_entities as $record) {
      $entity = $record->data;
      // Set the values of the other columns.
      foreach ($this->entityInfo['schema_fields_sql']['base table'] as $field) {
        $entity->$field = $record->$field;
      }
      unset($entity->data, $entity->plugin);
      // Add any tags or dependencies.
      $entity->dependencies = isset($modules[$entity->id]) ? $modules[$entity->id] : array();
      $entity->tags = isset($tags[$entity->id]) ? $tags[$entity->id] : array();
      $entities[$entity->id] = $entity;
    }
    $queried_entities = $entities;
    parent::attachLoad($queried_entities, $revision_id);
  }

  /**
   * Override to support having events and tags as conditions.
   * @see EntityAPIController::applyConditions($entities, $conditions)
   * @see rules_query_rules_config_load_multiple_alter()
   */
  protected function applyConditions($entities, $conditions = array()) {
    if (isset($conditions['event']) || isset($conditions['plugin'])) {
      foreach ($entities as $key => $entity) {
        if (isset($conditions['event']) && (!($entity instanceof RulesTriggerableInterface) || !in_array($conditions['event'], $entity->events()))) {
          unset($entities[$key]);
        }
        if (isset($conditions['plugin']) && !is_array($conditions['plugin'])) {
          $conditions['plugin'] = array($conditions['plugin']);
        }
        if (isset($conditions['plugin']) && !in_array($entity->plugin(), $conditions['plugin'])) {
          unset($entities[$key]);
        }
      }
      unset($conditions['event'], $conditions['plugin']);
    }
    if (!empty($conditions['tags'])) {
      foreach ($entities as $key => $entity) {
        foreach ($conditions['tags'] as $tag) {
          if (in_array($tag, $entity->tags)) {
            continue 2;
          }
        }
        unset($entities[$key]);
      }
      unset($conditions['tags']);
    }
    return parent::applyConditions($entities, $conditions);
  }

  /**
   * Overridden to work with Rules' custom export format.
   *
   * @param $export
   *   A serialized string in JSON format as produced by the
   *   RulesPlugin::export() method, or the PHP export as usual PHP array.
   */
  public function import($export, &$error_msg = '') {
    $export = is_array($export) ? $export : drupal_json_decode($export);
    if (!is_array($export)) {
      $error_msg = t('Unable to parse the pasted export.');
      return FALSE;
    }
    // The key ist the configuration name and the value the actual export.
    list($name, $export) = each($export);
    if (!isset($export['PLUGIN'])) {
      $error_msg = t('Export misses plugin information.');
      return FALSE;
    }
    // Create an empty configuration, re-set basic keys and import.
    $config = rules_plugin_factory($export['PLUGIN']);
    $config->name = $name;
    foreach (array('label', 'active', 'weight', 'tags', 'access_exposed') as $key) {
      if (isset($export[strtoupper($key)])) {
        $config->$key = $export[strtoupper($key)];
      }
    }
    if (!empty($export['REQUIRES'])) {
      foreach ($export['REQUIRES'] as $module) {
        if (!module_exists($module)) {
          $error_msg = t('Missing the required module %module.', array('%module' => $module));
          return FALSE;
        }
      }
      $config->dependencies = $export['REQUIRES'];
    }
    $config->import($export);
    return $config;
  }

  public function save($rules_config, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();

    $return = parent::save($rules_config, $transaction);
    $this->storeTags($rules_config);
    if ($rules_config instanceof RulesTriggerableInterface) {
      $this->storeEvents($rules_config);
    }
    $this->storeDependencies($rules_config);
    return $return;
  }

  /**
   * Save tagging information to the rules_tags table.
   */
  protected function storeTags($rules_config) {
    db_delete('rules_tags')
      ->condition('id', $rules_config->id)
      ->execute();
    if (!empty($rules_config->tags)) {
      foreach ($rules_config->tags as $tag) {
        db_insert('rules_tags')
          ->fields(array('id', 'tag'), array($rules_config->id, $tag))
          ->execute();
      }
    }
  }

  /**
   * Save event information to the rules_trigger table.
   */
  protected function storeEvents(RulesTriggerableInterface $rules_config) {
    db_delete('rules_trigger')
      ->condition('id', $rules_config->id)
      ->execute();
    foreach ($rules_config->events() as $event) {
      db_insert('rules_trigger')
        ->fields(array(
        'id' => $rules_config->id,
        'event' => $event,
      ))
      ->execute();
    }
  }

  protected function storeDependencies($rules_config) {
    db_delete('rules_dependencies')
      ->condition('id', $rules_config->id)
      ->execute();
    if (!empty($rules_config->dependencies)) {
      foreach ($rules_config->dependencies as $dependency) {
        db_insert('rules_dependencies')
          ->fields(array(
          'id' => $rules_config->id,
          'module' => $dependency,
        ))
        ->execute();
      }
    }
  }

  /**
   * Overridden to support tags and events in $conditions.
   * @see EntityAPIControllerExportable::buildQuery()
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $query_conditions =& $query->conditions();
    foreach ($query_conditions as &$condition) {
      // Support using 'tags' => array('tag1', 'tag2') as condition.
      if ($condition['field'] == 'base.tags') {
        $query->join('rules_tags', 'rt', 'base.id = rt.id');
        $condition['field'] = 'rt.tag';
      }
      // Support using 'event' => $name as condition.
      if ($condition['field'] == 'base.event') {
        $query->join('rules_trigger', 'tr', "base.id = tr.id");
        $condition['field'] = 'tr.event';
      }
    }
    return $query;
  }

  /**
   * Overridden to also delete tags and events.
   * @see EntityAPIControllerExportable::delete()
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();
    // Use entity-load as ids may be the names as well as the ids.
    $configs = $ids ? entity_load('rules_config', $ids) : array();
    if ($configs) {
      foreach ($configs as $config) {
        db_delete('rules_trigger')
          ->condition('id', $config->id)
          ->execute();
        db_delete('rules_tags')
          ->condition('id', $config->id)
          ->execute();
        db_delete('rules_dependencies')
          ->condition('id', $config->id)
          ->execute();
      }
    }
    return parent::delete($ids, $transaction);
  }
}

/**
 * The RulesExtendable uses the rules cache to setup the defined extenders
 * and overrides automatically.
 * As soon faces is used the faces information is autoloaded using setUp().
 */
abstract class RulesExtendable extends FacesExtendable {

  /**
   * The name of the info definitions associated with info about this class.
   * This would be defined abstract, if possible. Common rules hooks with class
   * info are e.g. plugin_info and data_info.
   */
  protected $hook;

  /**
   * The name of the item this class represents in the info hook.
   */
  protected $itemName;

  protected $cache, $itemInfo = array();


  public function __construct() {
    $this->setUp();
  }

  protected function setUp() {
    // Keep a reference on the cache, so elements created during cache
    // rebuilding end up with a complete cache in the end too.
    $this->cache = &rules_get_cache();
    if (isset($this->cache[$this->hook][$this->itemName])) {
      $this->itemInfo = &$this->cache[$this->hook][$this->itemName];
    }
    // Set up the Faces Extenders
    if (!empty($this->itemInfo['faces_cache'])) {
      list($this->facesMethods, $this->facesIncludes, $this->faces) = $this->itemInfo['faces_cache'];
    }
  }

  /**
   * Force the object to be setUp, this executes setUp() if not done yet.
   */
  public function forceSetUp() {
    if (!isset($this->cache) || (!empty($this->itemInfo['faces_cache']) && !$this->faces)) {
      $this->setUp();
    }
  }

  /**
   * Magic method: Invoke the dynamically implemented methods.
   */
  public function __call($name, $arguments = array()) {
    $this->forceSetUp();
    return parent::__call($name, $arguments);
  }

  public function facesAs($interface = NULL) {
    $this->forceSetUp();
    return parent::facesAs($interface);
  }

  /**
   * Allows items to add something to the rules cache.
   */
  public function rebuildCache(&$itemInfo, &$cache) {
    // Speed up setting up items by caching the faces methods.
    if (!empty($itemInfo['extenders'])) {
      // Apply extenders and overrides.
      $itemInfo += array('overrides' => array());
      foreach ($itemInfo['extenders'] as $face => $data) {
        $data += array('file' => array());
        if (isset($data['class'])) {
          $this->extendByClass($face, $data['class'], $data['file']);
        }
        elseif (isset($data['methods'])) {
          $this->extend($face, $data['methods'], $data['file']);
        }
      }
      foreach ($itemInfo['overrides'] as $data) {
        $data += array('file' => array());
        $this->override($data['methods'], $data['file']);
      }
      $itemInfo['faces_cache'] = array($this->facesMethods, $this->facesIncludes, $this->faces);
      // We don't need that any more.
      unset($itemInfo['extenders'], $itemInfo['overrides']);
    }
  }

  /**
   * Returns whether the a RuleExtendable supports the given interface.
   *
   * @param $itemInfo
   *   The info about the item as specified in the hook.
   * @param $interface
   *   The interface to check for.
   * @return
   *   Whether it supports the given interface.
   */
  public static function itemFacesAs(&$itemInfo, $interface) {
    return in_array($interface, class_implements($itemInfo['class'])) || isset($itemInfo['faces_cache'][2][$interface]);
  }
}

/**
 * Base class for rules plugins.
 *
 * We cannot inherit from EntityDB at the same time, so we implement our own
 * entity related methods. Any CRUD related actions performed on contained
 * plugins are applied and the root element representing the configuration is
 * saved.
 */
abstract class RulesPlugin extends RulesExtendable {

  /**
   * If this is a configuration saved to the db, the id of it.
   */
  public $id = NULL;
  public $weight = 0;
  public $name = NULL;

  /**
   * An array of settings for this element.
   */
  public $settings = array();

  /**
   * Info about this element. Usage depends on the plugin.
   */
  protected $info = array();

  /**
   * The parent element, if any.
   * @var RulesContainerPlugin
   */
  protected $parent = NULL;

  protected $cache = NULL, $hook = 'plugin_info';

  /**
   * Identifies an element inside a configuration.
   */
  protected $elementId = NULL;

  /**
   * Static cache for availableVariables().
   */
  protected $availableVariables;


  /**
   * Sets a new parent element.
   */
  public function setParent(RulesContainerPlugin $parent) {
    if ($this->parent == $parent) {
      return;
    }
    if (isset($this->parent) && ($key = array_search($this, $this->parent->children)) !== FALSE) {
      // Remove element from any previous parent.
      unset($this->parent->children[$key]);
      $this->parent->resetInternalCache();
    }
    // Make sure the interface matches the type of the container.
    if (($parent instanceof RulesActionContainer && $this instanceof RulesActionInterface) ||
       ($parent instanceof RulesConditionContainer && $this instanceof RulesConditionInterface)) {

      $this->parent = $parent;
      $parent->children[] = $this;
      $this->parent->resetInternalCache();
    }
    else {
      throw new RulesEvaluationException('The given container is incompatible with this element.', array(), $this, RulesLog::ERROR);
    }
  }

  /**
   * Gets the root element of the configuration.
   */
  public function root() {
    $element = $this;
    while (!$element->isRoot()) {
      $element = $element->parent;
    }
    return $element;
  }

  /**
   * Returns whether the element is the root of the configuration.
   */
  public function isRoot() {
    return empty($this->parent) || isset($this->name);
  }

  /**
   * Returns the element's parent.
   */
  public function parentElement() {
    return $this->parent;
  }

  /**
   * Returns the element id, which identifies the element inside the config.
   */
  public function elementId() {
    if (!isset($this->elementId)) {
      $this->elementMap()->index();
    }
    return $this->elementId;
  }

  /**
   * Gets the element map helper object, which helps mapping elements to ids.
   *
   * @return RulesElementMap
   */
  public function elementMap() {
    $config = $this->root();
    if (empty($config->map)) {
      $config->map = new RulesElementMap($config);
    }
    return $config->map;
  }

  /**
   * Iterate over all elements nested below the current element.
   *
   * This helper can be used to recursively iterate over all elements of a
   * configuration. To iterate over the children only, just regulary iterate
   * over the object.
   *
   * @param $mode
   *   (optional) The iteration mode used. See
   *   RecursiveIteratorIterator::construct(). Defaults to SELF_FIRST.
   *
   * @return RecursiveIteratorIterator
   */
  public function elements($mode = RecursiveIteratorIterator::SELF_FIRST) {
    return new RecursiveIteratorIterator($this, $mode);
  }

  /**
   * Do a deep clone.
   */
  public function __clone() {
    // Make sure the element map is cleared.
    // @see self::elementMap()
    unset($this->map);
  }

  /**
   * Returns the depth of this element in the configuration.
   */
  public function depth() {
    $element = $this;
    $i = 0;
    while (!empty($element->parent)) {
      $element = $element->parent;
      $i++;
    }
    return $i;
  }

  /**
   * Execute the configuration.
   *
   * @param ...
   *   Arguments to pass to the configuration.
   */
  public function execute() {
    return $this->executeByArgs(func_get_args());
  }

  /**
   * Execute the configuration by passing arguments in a single array.
   */
  abstract public function executeByArgs($args = array());

  /**
   * Evaluate the element on a given rules evaluation state.
   */
  abstract function evaluate(RulesState $state);

  protected static function compare(RulesPlugin $a, RulesPlugin $b) {
    if ($a->weight == $b->weight) {
      return 0;
    }
    return ($a->weight < $b->weight) ? -1 : 1;
  }

  /**
   * Returns info about parameters needed by the plugin.
   *
   * Note that not necessarily all parameters are needed when executing the
   * plugin, as values for the parameter might have been already configured via
   * the element settings.
   *
   * @see self::parameterInfo()
   */
  public function pluginParameterInfo() {
    return isset($this->info['parameter']) ? $this->info['parameter'] : array();
  }

  /**
   * Returns info about parameters needed for executing the configured plugin.
   *
   * @param $optional
   *   Whether optional parameters should be included.
   *
   * @see self::pluginParameterInfo()
   */
  public function parameterInfo($optional = FALSE) {
    // We have to filter out parameters that are already configured.
    foreach ($this->pluginParameterInfo() as $name => $info) {
      if (!isset($this->settings[$name . ':select']) && !isset($this->settings[$name]) && ($optional || (empty($info['optional']) && $info['type'] != 'hidden'))) {
        $vars[$name] = $info;
      }
    }
    return isset($vars) ? $vars : array();
  }

  /**
   * Returns the about variables the plugin provides for later evaluated elements.
   *
   * Note that this method returns info about the provided variables as defined
   * by the plugin. Thus this resembles the original info, which may be
   * adapted via configuration.
   *
   * @see self::providesVariables()
   */
  public function pluginProvidesVariables() {
    return isset($this->info['provides']) ? $this->info['provides'] : array();
  }

  /**
   * Returns info about all variables provided for later evaluated elements.
   *
   * @see self::pluginProvidesVariables()
   */
  public function providesVariables() {
    foreach ($this->pluginProvidesVariables() as $name => $info) {
      $info['source name'] = $name;
      $info['label'] = isset($this->settings[$name . ':label']) ? $this->settings[$name . ':label'] : $info['label'];
      if (isset($this->settings[$name . ':var'])) {
        $name = $this->settings[$name . ':var'];
      }
      $provides[$name] = $info;
    }
    return isset($provides) ? $provides : array();
  }

  /**
   * Returns the info of the plugin.
   */
  public function info() {
    return $this->info;
  }

  /**
   * When converted to a string, just use the export format.
   */
  public function __toString() {
    return $this->isRoot() ? $this->export() : entity_var_json_export($this->export());
  }

  /**
   * Gets variables to return once the configuration has been executed.
   */
  protected function returnVariables(RulesState $state, $result = NULL) {
    $var_info = $this->providesVariables();
    foreach ($var_info as $name => $info) {
      try {
        $vars[$name] = $this->getArgument($name, $info, $state);
      }
      catch (RulesEvaluationException $e) {
        // Ignore not existing variables.
        $vars[$name] = NULL;
      }
      $var_info[$name] += array('allow null' => TRUE);
    }
    return isset($vars) ? array_values(rules_unwrap_data($vars, $var_info)) : array();
  }

  /**
   * Sets up the execution state for the given arguments.
   */
  public function setUpState(array $args) {
    $state = new RulesState();
    $vars = $this->setUpVariables();
    // Fix numerically indexed args to start with 0.
    if (!isset($args[rules_array_key($vars)])) {
      $args = array_values($args);
    }
    $offset = 0;
    foreach (array_keys($vars) as $i => $name) {
      $info = $vars[$name];
      if (!empty($info['handler']) || (isset($info['parameter']) && $info['parameter'] === FALSE)) {
        $state->addVariable($name, NULL, $info);
        // Count the variables that are not passed as parameters.
        $offset++;
      }
      // Support numerically indexed arrays as well as named parameter style.
      // The index is reduced to exclude non-parameter variables.
      elseif (isset($args[$i - $offset])) {
        $state->addVariable($name, $args[$i - $offset], $info);
      }
      elseif (isset($args[$name])) {
        $state->addVariable($name, $args[$name], $info);
      }
      elseif (empty($info['optional']) && $info['type'] != 'hidden') {
        throw new RulesEvaluationException('Argument %name is missing.', array('%name' => $name), $this, RulesLog::ERROR);
      }
    }
    return $state;
  }

  /**
   * Returns info about all variables that have to be setup in the state.
   */
  protected function setUpVariables() {
    return $this->parameterInfo(TRUE);
  }

  /**
   * Returns info about variables available to be used as arguments for this element.
   *
   * As this is called very often, e.g. during integrity checks, we statically
   * cache the results.
   *
   * @see RulesPlugin::resetInternalCache()
   */
  public function availableVariables() {
    if (!isset($this->availableVariables)) {
      $this->availableVariables = !$this->isRoot() ? $this->parent->stateVariables($this) : RulesState::defaultVariables();
    }
    return $this->availableVariables;
  }

  /**
   * Returns asserted additions to the available variable info. Any returned
   * info is merged into the variable info, in case the execution flow passes
   * the element.
   * E.g. this is used to assert the content type of a node if the condition
   * is met, such that the per node type properties are available.
   */
  protected function variableInfoAssertions() {
    return array();
  }

  /**
   * Get the name of this plugin instance. The returned name should identify
   * the code which drives this plugin.
   */
  public function getPluginName() {
    return $this->itemName;
  }

  /**
   * Calculates an array of required modules.
   *
   * You can use $this->dependencies to access dependencies for saved
   * configurations.
   */
  public function dependencies() {
    $this->processSettings();
    $modules = isset($this->itemInfo['module']) && $this->itemInfo['module'] != 'rules' ? array($this->itemInfo['module'] => 1) : array();
    foreach ($this->pluginParameterInfo() as $name => $info) {
      if (isset($this->settings[$name . ':process']) && $this->settings[$name . ':process'] instanceof RulesDataProcessor) {
        $modules += array_flip($this->settings[$name . ':process']->dependencies());
      }
    }
    return array_keys($modules);
  }

  /**
   * Whether the currently logged in user has access to all configured elements.
   *
   * Note that this only checks whether the current user has permission to all
   * configured elements, but not whether a user has access to configure Rule
   * configurations in general. Use rules_config_access() for that.
   *
   * Use this to determine access permissions for configuring or triggering the
   * execution of certain configurations independent of the Rules UI.
   *
   * @see rules_config_access()
   */
  public function access() {
    $this->processSettings();
    foreach ($this->pluginParameterInfo() as $name => $info) {
      if (isset($this->settings[$name . ':select']) && $wrapper = $this->applyDataSelector($this->settings[$name . ':select'])) {
        if ($wrapper->access('view') === FALSE) {
          return FALSE;
        }
      }
      // Incorporate access checks for data processors and input evaluators.
      if (isset($this->settings[$name . ':process']) && $this->settings[$name . ':process'] instanceof RulesDataProcessor && !$this->settings[$name . ':process']->editAccess()) {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Processes the settings e.g. to prepare input evaluators.
   *
   * Usually settings get processed automatically, however if $this->settings
   * has been altered manually after element construction, it needs to be
   * invoked explicitly with $force set to TRUE.
   *
   */
  public function processSettings($force = FALSE) {
    // Process if not done yet.
    if ($force || !empty($this->settings['#_needs_processing'])) {
      $var_info = $this->availableVariables();
      foreach ($this->pluginParameterInfo() as $name => $info) {
        // Prepare input evaluators.
        if (isset($this->settings[$name])) {
          $this->settings[$name . ':process'] = $this->settings[$name];
          RulesDataInputEvaluator::prepareSetting($this->settings[$name . ':process'], $info, $var_info);
        }
        // Prepare data processors.
        elseif (isset($this->settings[$name . ':select']) && !empty($this->settings[$name . ':process'])) {
          RulesDataProcessor::prepareSetting($this->settings[$name . ':process'], $info, $var_info);
        }
        // Clean up.
        if (empty($this->settings[$name . ':process'])) {
          unset($this->settings[$name . ':process']);
        }
      }
      unset($this->settings['#_needs_processing']);
    }
  }

  /**
   * Makes sure the plugin is configured right, e.g. all needed variables
   * are available in the element's scope and dependent modules are enabled.
   *
   * @return RulesPlugin
   *   Returns $this to support chained usage.
   *
   * @throws RulesIntegrityException
   *   In case of a failed integraty check, a RulesIntegrityException exception
   *   is thrown.
   */
  public function integrityCheck() {
    // First process the settings if not done yet.
    $this->processSettings();
    // Check dependencies using the pre-calculated dependencies stored in
    // $this->dependencies. Fail back to calculation them on the fly, e.g.
    // during creation.
    $dependencies = empty($this->dependencies) ? $this->dependencies() : $this->dependencies;
    foreach ($dependencies as $module) {
      if (!module_exists($module)) {
        throw new RulesDependencyException(t('Missing required module %name.', array('%name' => $module)));
      }
    }
    // Check the parameter settings.
    $this->checkParameterSettings();
    // Check variable names for provided variables to be valid.
    foreach ($this->pluginProvidesVariables() as $name => $info) {
      if (isset($this->settings[$name . ':var'])) {
        $this->checkVarName($this->settings[$name . ':var']);
      }
    }
    return $this;
  }

  protected function checkVarName($name) {
    if (!preg_match('/^[0-9a-zA-Z_]*$/', $name)) {
      throw new RulesIntegrityException(t('%plugin: The variable name %name contains not allowed characters.', array('%plugin' => $this->getPluginName(), '%name' => $name)), $this);
    }
  }

  /**
   * Checks whether parameters are correctly configured.
   */
  protected function checkParameterSettings() {
    foreach ($this->pluginParameterInfo() as $name => $info) {
      if (isset($info['restriction']) && $info['restriction'] == 'selector' && isset($this->settings[$name])) {
        throw new RulesIntegrityException(t("The parameter %name may only be configured using a selector.", array('%name' => $name)), array($this, 'parameter', $name));
      }
      elseif (isset($info['restriction']) && $info['restriction'] == 'input' && isset($this->settings[$name . ':select'])) {
        throw new RulesIntegrityException(t("The parameter %name may not be configured using a selector.", array('%name' => $name)), array($this, 'parameter', $name));
      }
      elseif (!empty($this->settings[$name . ':select']) && !$this->applyDataSelector($this->settings[$name . ':select'])) {
        throw new RulesIntegrityException(t("Data selector %selector for parameter %name is invalid.", array('%selector' => $this->settings[$name . ':select'], '%name' => $name)), array($this, 'parameter', $name));
      }
      elseif ($arg_info = $this->getArgumentInfo($name)) {
        // If we have enough metadata, check whether the types match.
        if (!RulesData::typesMatch($arg_info, $info)) {
          throw new RulesIntegrityException(t("The data type of the configured argument does not match the parameter's %name requirement.", array('%name' => $name)), array($this, 'parameter', $name));
        }
      }
      elseif (!$this->isRoot() && !isset($this->settings[$name]) && empty($info['optional']) && $info['type'] != 'hidden') {
        throw new RulesIntegrityException(t('Missing configuration for parameter %name.', array('%name' => $name)), array($this, 'parameter', $name));
      }
      //TODO: Make sure used values are allowed. (key/value pairs + allowed values)
    }
  }

  /**
   * Returns the argument as configured in the element settings for the
   * parameter $name described with $info.
   *
   * @param $name
   *   The name of the parameter for which to get the argument.
   * @param $info
   *   Info about the parameter.
   * @param RulesState $state
   *   The current evaluation state.
   * @param $langcode
   *   (optional) The language code used to get the argument value if the
   *   argument value should be translated. By default (NULL) the current
   *   interface language will be used.
   *
   * @return
   *   The argument, possibly wrapped.
   *
   * @throws RulesEvaluationException
   *   In case the argument cannot be retrieved an exception is thrown.
   */
  protected function getArgument($name, $info, RulesState $state, $langcode = NULL) {
    // Only apply the langcode if the parameter has been marked translatable.
    if (empty($info['translatable'])) {
      $langcode = LANGUAGE_NONE;
    }
    elseif (!isset($langcode)) {
      $langcode = $GLOBALS['language']->language;
    }

    if (!empty($this->settings[$name . ':select'])) {
      $arg = $state->applyDataSelector($this->settings[$name . ':select'], $langcode);
    }
    elseif (isset($this->settings[$name])) {
      $arg = rules_wrap_data($this->settings[$name], $info);
      // We don't sanitize directly specified values.
      $skip_sanitize = TRUE;
    }
    elseif ($state->varinfo($name)) {
      $arg = $state->get($name);
    }
    elseif (empty($info['optional']) && $info['type'] != 'hidden') {
      throw new RulesEvaluationException('Required parameter %name is missing.', array('%name' => $name), $this, RulesLog::ERROR);
    }
    else {
      $arg = isset($info['default value']) ? $info['default value'] : NULL;
      $skip_sanitize = TRUE;
      $info['allow null'] = TRUE;
    }
    // Make sure the given value is set if required (default).
    if (!isset($arg) && empty($info['allow null'])) {
      throw new RulesEvaluationException('The provided argument for parameter %name is empty.', array('%name' => $name), $this);
    }

    // Apply any configured data processors.
    if (!empty($this->settings[$name . ':process'])) {
      // For processing, make sure the data is unwrapped now.
      $return = rules_unwrap_data(array($arg), array($info));
      // @todo for Drupal 8: Refactor to add the name and language code as
      // separate parameter to process().
      $info['#name'] = $name;
      $info['#langcode'] = $langcode;
      return isset($return[0]) ? $this->settings[$name . ':process']->process($return[0], $info, $state, $this) : NULL;
    }
    // Support passing already sanitized values.
    if ($info['type'] == 'text' && !isset($skip_sanitize) && !empty($info['sanitize']) && !($arg instanceof EntityMetadataWrapper)) {
      return check_plain((string) $arg);
    }
    return $arg;
  }

  /**
   * Gets the right arguments for executing the element.
   *
   * @throws RulesEvaluationException
   *   If case an argument cannot be retrieved an exception is thrown.
   */
  protected function getExecutionArguments(RulesState $state) {
    $parameters = $this->pluginParameterInfo();
    // If there is language parameter, get its value first so it can be used
    // for getting other translatable values.
    $langcode = NULL;
    if (isset($parameters['language'])) {
      $lang_arg = $this->getArgument('language', $parameters['language'], $state);
      $langcode = $lang_arg instanceof EntityMetadataWrapper ? $lang_arg->value() : $lang_arg;
    }
    // Now get all arguments.
    foreach ($parameters as $name => $info) {
      $args[$name] = $name == 'language' ? $lang_arg : $this->getArgument($name, $info, $state, $langcode);
    }
    // Append the settings and the execution state. Faces will append $this.
    $args['settings'] = $this->settings;
    $args['state'] = $state;
    // Make the wrapped variables for the arguments available in the state.
    $state->currentArguments = $args;
    return rules_unwrap_data($args, $parameters);
  }

  /**
   * Apply the given data selector by using the info about available variables.
   * Thus it doesn't require an actual evaluation state.
   *
   * @param $selector
   *   The selector string, e.g. "node:author:mail".
   *
   * @return EntityMetadataWrapper
   *   An empty wrapper for the given selector or FALSE if the selector couldn't
   *   be applied.
   */
  public function applyDataSelector($selector) {
    $parts = explode(':', str_replace('-', '_', $selector), 2);
    if (($vars = $this->availableVariables()) && isset($vars[$parts[0]]['type'])) {
      $wrapper = rules_wrap_data(NULL, $vars[$parts[0]], TRUE);
      if (count($parts) > 1 && $wrapper instanceof EntityMetadataWrapper) {
        try {
          foreach (explode(':', $parts[1]) as $name) {
            if ($wrapper instanceof EntityListWrapper || $wrapper instanceof EntityStructureWrapper) {
              $wrapper = $wrapper->get($name);
            }
            else {
              return FALSE;
            }
          }
        }
        // In case of an exception or we were unable to get a wrapper, return FALSE.
        catch (EntityMetadataWrapperException $e) {
          return FALSE;
        }
      }
    }
    return isset($wrapper) ? $wrapper : FALSE;
  }

  /**
   * Returns info about the configured argument.
   *
   * @return
   *   The determined info. If it's not known NULL is returned.
   */
  public function getArgumentInfo($name) {
    $vars = $this->availableVariables();
    if (!empty($this->settings[$name . ':select']) && !empty($vars[$this->settings[$name . ':select']])) {
      return $vars[$this->settings[$name . ':select']];
    }
    elseif (!empty($this->settings[$name . ':select'])) {
      if ($wrapper = $this->applyDataSelector($this->settings[$name . ':select'])) {
        return $wrapper->info();
      }
      return;
    }
    elseif (isset($this->settings[$name . ':type'])) {
      return array('type' => $this->settings[$name . ':type']);
    }
    elseif (!isset($this->settings[$name]) && isset($vars[$name])) {
      return $vars[$name];
    }
  }

  /**
   * Saves the configuration to the database, regardless whether this is invoked
   * on the rules configuration or a contained rule element.
   */
  public function save($name = NULL, $module = 'rules') {
    if (isset($this->parent)) {
      $this->parent->sortChildren();
      return $this->parent->save($name, $module);
    }
    else {
      // Update the dirty flag before saving.
      // However, this operation depends on a fully built Rules-cache, so skip
      // it when entities in code are imported to the database.
      // @see _rules_rebuild_cache()
      if (empty($this->is_rebuild)) {
        rules_config_update_dirty_flag($this, FALSE);
        // In case the config is not dirty, pre-calculate the dependencies for
        // later checking. Note that this also triggers processing settings if
        // necessary.
        // @see rules_modules_enabled()
        if (empty($this->dirty)) {
          $this->dependencies = $this->dependencies();
        }
      }

      $this->plugin = $this->itemName;
      $this->name = isset($name) ? $name : $this->name;
      $this->module = !isset($this->module) || $module != 'rules' ? $module : $this->module;
      $this->ensureNameExists();
      $this->data = $this;
      $return = entity_get_controller('rules_config')->save($this);
      unset($this->data);

      // Care about clearing necessary caches.
      if (!empty($this->is_rebuild)) {
        rules_clear_cache();
      }
      else {
        $plugin_info = $this->pluginInfo();
        if (!empty($plugin_info['component'])) {
          // When component variables changes rebuild the complete cache so the
          // changes to the provided action/condition take affect.
          if (empty($this->original) || $this->componentVariables() != $this->original->componentVariables()) {
            rules_clear_cache();
          }
          // Clear components cached for evaluation.
          cache_clear_all('comp_', 'cache_rules', TRUE);
        }
        elseif ($this->plugin == 'reaction rule') {
          // Clear event sets cached for evaluation.
          cache_clear_all('event_', 'cache_rules', TRUE);
          variable_del('rules_empty_sets');
        }
        drupal_static_reset('rules_get_cache');
        drupal_static_reset('rules_config_update_dirty_flag');
      }

      return $return;
    }
  }

  /**
   * Ensure the configuration has a name. If not, generate one.
   */
  protected function ensureNameExists() {
    if (!isset($this->module)) {
      $this->module = 'rules';
    }
    if (!isset($this->name)) {
      // Find a unique name for this configuration.
      $this->name = $this->module . '_';
      for ($i = 0; $i < 8; $i++) {
        // Alphanumeric name generation.
        $rnd = mt_rand(97, 122);
        $this->name .= chr($rnd);
      }
    }
  }

  public function __sleep() {
    // Keep the id always as we need it for the recursion prevention.
    $array = drupal_map_assoc(array('parent', 'id', 'elementId', 'weight', 'settings'));
    // Keep properties related to configurations if they are there.
    foreach (array('name', 'module', 'status', 'label', 'recursion', 'tags') as $key) {
      if (isset($this->$key)) {
        $array[$key] = $key;
      }
    }
    return $array;
  }

  /**
   * Optimizes a rule configuration in order to speed up evaluation.
   *
   * Additional optimization methods may be inserted by an extender
   * implementing the RulesOptimizationInterface. By default, there is no
   * optimization extender.
   *
   * An optimization method may rearrange the internal structure of a
   * configuration in order to speed up the evaluation. As the configuration may
   * change optimized configurations should not be saved permanently, except
   * when saving it temporary, for later execution only.
   *
   * @see RulesOptimizationInterface
   */
  public function optimize() {
    // Make sure settings are processed before configs are cached.
    $this->processSettings();
    if ($this->facesAs('RulesOptimizationInterface')) {
      $this->__call('optimize');
    }
  }

  /**
   * If invoked on a rules configuration it is deleted from database. If
   * invoked on a contained rule element, it's removed from the configuration.
   */
  public function delete() {
    if (isset($this->parent)) {
      foreach ($this->parent->children as $key => $child) {
        if ($child === $this) {
          unset($this->parent->children[$key]);
          break;
        }
      }
    }
    elseif (isset($this->id)) {
      entity_get_controller('rules_config')->delete(array($this->name));
      rules_clear_cache();
    }
  }

  public function internalIdentifier() {
    return isset($this->id) ? $this->id : NULL;
  }

  /**
   * Returns the config name.
   */
  public function identifier() {
    return isset($this->name) ? $this->name : NULL;
  }

  public function entityInfo() {
    return entity_get_info('rules_config');
  }

  public function entityType() {
    return 'rules_config';
  }

  /**
   * Checks if the configuration has a certain exportable status.
   *
   * @param $status
   *   A status constant, i.e. one of ENTITY_CUSTOM, ENTITY_IN_CODE,
   *   ENTITY_OVERRIDDEN or ENTITY_FIXED.
   *
   * @return
   *   TRUE if the configuration has the status, else FALSE.
   *
   * @see entity_has_status()
   */
  public function hasStatus($status) {
    return $this->isRoot() && isset($this->status) && ($this->status & $status) == $status;
  }

  /**
   * Remove circular object references so the PHP garbage collector does its
   * work.
   */
  public function destroy() {
    parent::destroy();
    $this->parent = NULL;
  }

  /**
   * Seamlessy invokes the method implemented via faces without having to think
   * about references.
   */
  public function form(&$form, &$form_state, array $options = array()) {
    $this->__call('form', array(&$form, &$form_state, $options));
  }

  public function form_validate($form, &$form_state) {
    $this->__call('form_validate', array($form, &$form_state));
  }

  public function form_submit($form, &$form_state) {
    $this->__call('form_submit', array($form, &$form_state));
  }

  /**
   * Returns the label of the element.
   */
  public function label() {
    if (!empty($this->label) && $this->label != t('unlabeled')) {
      return $this->label;
    }
    $info = $this->info();
    return isset($info['label']) ? $info['label'] : (!empty($this->name) ? $this->name : t('unlabeled'));
  }

  /**
   * Returns the name of the element's plugin.
   */
  public function plugin() {
    return $this->itemName;
  }

  /**
   * Returns info about the element's plugin.
   */
  public function pluginInfo() {
    $this->forceSetUp();
    return $this->itemInfo;
  }

  /**
   * Applies the given export.
   */
  public function import(array $export) {
    $this->importSettings($export[strtoupper($this->plugin())]);
  }

  protected function importSettings($export) {
    // Import parameter settings.
    $export += array('USING' => array(), 'PROVIDE' => array());
    foreach ($export['USING'] as $name => $param_export) {
      $this->importParameterSetting($name, $param_export);
    }
    foreach ($export['PROVIDE'] as $name => $var_export) {
      // The key of $var_export is the variable name, the value the label.
      $this->settings[$name . ':var'] = rules_array_key($var_export);
      $this->settings[$name . ':label'] = reset($var_export);
    }
  }

  protected function importParameterSetting($name, $export) {
    if (is_array($export) && isset($export['select'])) {
      $this->settings[$name . ':select'] = $export['select'];
      if (count($export) > 1) {
        // Add in processor settings.
        unset($export['select']);
        $this->settings[$name . ':process'] = $export;
      }
    }
    // Convert back the [selector] strings being an array with one entry.
    elseif (is_array($export) && count($export) == 1 && isset($export[0])) {
      $this->settings[$name . ':select'] = $export[0];
    }
    elseif (is_array($export) && isset($export['value'])) {
      $this->settings[$name] = $export['value'];
    }
    else {
      $this->settings[$name] = $export;
    }
  }

  /**
   * Exports a rule configuration.
   *
   * @param $prefix
   *   An optional prefix for each line.
   * @param $php
   *   Optional. Set to TRUE to format the export using PHP arrays. By default
   *   JSON is used.
   * @return
   *   The exported confiugration.
   *
   * @see rules_import()
   */
  public function export($prefix = '', $php = FALSE) {
    $export = $this->exportToArray();
    return $this->isRoot() ? $this->returnExport($export, $prefix, $php) : $export;
  }

  protected function exportToArray() {
    $export[strtoupper($this->plugin())] = $this->exportSettings();
    return $export;
  }

  protected function exportSettings() {
    $export = array();
    if (!$this->isRoot()) {
      foreach ($this->pluginParameterInfo() as $name => $info) {
        if (($return = $this->exportParameterSetting($name, $info)) !== NULL) {
          $export['USING'][$name] = $return;
        }
      }
      foreach ($this->providesVariables() as $name => $info) {
        if (!empty($info['source name'])) {
          $export['PROVIDE'][$info['source name']][$name] = $info['label'];
        }
      }
    }
    return $export;
  }

  protected function exportParameterSetting($name, $info) {
    if (isset($this->settings[$name]) && (empty($info['optional']) || !isset($info['default value']) || $this->settings[$name] != $info['default value'])) {
      // In case of an array-value wrap the value into another array, such that
      // the value cannot be confused with an exported data selector.
      return is_array($this->settings[$name]) ? array('value' => $this->settings[$name]) : $this->settings[$name];
    }
    elseif (isset($this->settings[$name . ':select'])) {
      if (isset($this->settings[$name . ':process']) && $processor = $this->settings[$name . ':process']) {
        $export['select'] = $this->settings[$name . ':select'];
        $export += $processor instanceof RulesDataProcessor ? $processor->getChainSettings() : $processor;
        return $export;
      }
      // If there is no processor use a simple array to abbreviate this usual
      // case. In JSON this turns to a nice [selector] string.
      return array($this->settings[$name . ':select']);
    }
  }

  /**
   * Finalizies the configuration export by adding general attributes regarding
   * the configuration and returns it in the right format.
   */
  protected function returnExport($export, $prefix = '', $php = FALSE) {
    $this->ensureNameExists();
    if (!empty($this->label) && $this->label != t('unlabeled')) {
      $export_cfg[$this->name]['LABEL'] = $this->label;
    }
    $export_cfg[$this->name]['PLUGIN'] = $this->plugin();
    if (!empty($this->weight)) {
      $export_cfg[$this->name]['WEIGHT'] = $this->weight;
    }
    if (isset($this->active) && !$this->active) {
      $export_cfg[$this->name]['ACTIVE'] = FALSE;
    }
    if (!empty($this->tags)) {
      $export_cfg[$this->name]['TAGS'] = $this->tags;
    }
    if ($modules = $this->dependencies()) {
      $export_cfg[$this->name]['REQUIRES'] = $modules;
    }
    if (!empty($this->access_exposed)) {
      $export_cfg[$this->name]['ACCESS_EXPOSED'] = $this->access_exposed;
    };
    $export_cfg[$this->name] += $export;
    return $php ? entity_var_export($export_cfg, $prefix) : entity_var_json_export($export_cfg, $prefix);
  }

  /**
   * Resets any internal static caches.
   *
   * This function does not reset regular caches as retrieved via
   * rules_get_cache(). Usually, it's invoked automatically when a Rules
   * configuration is modified.
   *
   * Static caches are reset for the element and any elements down the tree. To
   * clear static caches of the whole configuration, invoke the function at the
   * root.
   *
   * @see RulesPlugin::availableVariables()
   */
  public function resetInternalCache() {
    $this->availableVariables = NULL;
  }
}

/**
 * Defines a common base class for so called "Abstract Plugins" like actions.
 * Thus modules have to provide the concrete plugin implementation.
 */
abstract class RulesAbstractPlugin extends RulesPlugin {

  protected $elementName;
  protected $info = array('parameter' => array(), 'provides' => array());
  protected $infoLoaded = FALSE;

  /**
   * @param $name
   *   The plugin implementation's name.
   * @param $info
   *   Further information provided about the plugin. Optional.
   * @throws RulesException
   *   If validation of the passed settings fails RulesExceptions are thrown.
   */
  function __construct($name = NULL, $settings = array()) {
    $this->elementName = $name;
    $this->settings = (array) $settings + array('#_needs_processing' => TRUE);
    $this->setUp();
  }

  protected function setUp() {
    parent::setUp();
    if (isset($this->cache[$this->itemName . '_info'][$this->elementName])) {
      $this->info = $this->cache[$this->itemName . '_info'][$this->elementName];
      // Remember that the info has been correctly setup.
      // @see self::forceSetup().
      $this->infoLoaded = TRUE;

      // Add in per-plugin implementation callbacks if any.
      if (!empty($this->info['faces_cache'])) {
        foreach ($this->info['faces_cache'] as $face => $data) {
          list($methods, $file_names) = $data;
          foreach ($methods as $method => $callback) {
            $this->facesMethods[$method] = $callback;
          }
          foreach ((array) $file_names as $method => $name) {
            $this->facesIncludes[$method] = array('module' => $this->info['module'], 'name' => $name);
          }
        }
        // Invoke the info_alter callback, but only if it has been implemented.
        if ($this->facesMethods['info_alter'] != $this->itemInfo['faces_cache'][0]['info_alter']) {
          $this->__call('info_alter', array(&$this->info));
        }
      }
    }
    elseif (!empty($this->itemInfo['faces_cache']) && function_exists($this->elementName)) {
      // We don't have any info, so just add the name as execution callback.
      $this->override(array('execute' => $this->elementName));
    }
  }

  public function forceSetUp() {
    if (!isset($this->cache) || (!empty($this->itemInfo['faces_cache']) && !$this->faces)) {
      $this->setUp();
    }
    // In case we have element specific information, which is not loaded yet,
    // do so now. This might happen if the element has been initially loaded
    // with an incomplete cache, i.e. during cache rebuilding.
    elseif (!$this->infoLoaded && isset($this->cache[$this->itemName . '_info'][$this->elementName])) {
      $this->setUp();
    }
  }

  /**
   * Returns the label of the element.
   */
  public function label() {
    $info = $this->info();
    return isset($info['label']) ? $info['label'] : t('@plugin "@name"', array('@name' => $this->elementName, '@plugin' => $this->plugin()));
  }

  public function access() {
    $info = $this->info();
    $this->loadBasicInclude();
    if (!empty($info['access callback']) && !call_user_func($info['access callback'], $this->itemName, $this->getElementName())) {
      return FALSE;
    }
    return parent::access() && $this->__call('access');
  }

  public function integrityCheck() {
    // Do the usual integrity check first so the implementation's validation
    // handler can rely on that already.
    parent::integrityCheck();
    // Make sure the element is known.
    $this->forceSetUp();
    if (!isset($this->cache[$this->itemName . '_info'][$this->elementName])) {
      throw new RulesIntegrityException(t('Unknown @plugin %name.', array('@plugin' => $this->plugin(), '%name' => $this->elementName)));
    }
    $this->validate();
    return $this;
  }

  public function processSettings($force = FALSE) {
    // Process if not done yet.
    if ($force || !empty($this->settings['#_needs_processing'])) {
      $this->resetInternalCache();
      // In case the element implements the info alteration callback, (re-)run
      // the alteration so that any settings depending info alterations are
      // applied.
      if ($this->facesMethods && $this->facesMethods['info_alter'] != $this->itemInfo['faces_cache'][0]['info_alter']) {
        $this->__call('info_alter', array(&$this->info));
      }
      // First let the plugin implementation do processing, so data types of the
      // parameters are fixed when we process the settings.
      $this->process();
      parent::processSettings($force);
    }
  }

  public function pluginParameterInfo() {
    // Ensure the info alter callback has been executed.
    $this->forceSetup();
    return parent::pluginParameterInfo();
  }

  public function pluginProvidesVariables() {
    // Ensure the info alter callback has been executed.
    $this->forceSetup();
    return parent::pluginProvidesVariables();
  }

  public function info() {
    // Ensure the info alter callback has been executed.
    $this->forceSetup();
    return $this->info;
  }

  protected function variableInfoAssertions() {
    // Get the implementation's assertions and map them to the variable names.
    if ($assertions = $this->__call('assertions')) {
      foreach ($assertions as $param_name => $data) {
        $name = isset($this->settings[$param_name . ':select']) ? $this->settings[$param_name . ':select'] : $param_name;
        $return[$name] = $data;
      }
      return $return;
    }
  }

  public function import(array $export) {
    // The key is the element name and the value the actual export.
    $this->elementName = rules_array_key($export);
    $export = reset($export);

    // After setting the element name, setup the element again so the right
    // element info is loaded.
    $this->setUp();

    if (!isset($export['USING']) && !isset($export['PROVIDES']) && !empty($export)) {
      // The export has been abbreviated to skip "USING".
      $export = array('USING' => $export);
    }
    $this->importSettings($export);
  }

  protected function exportToArray() {
    $export = $this->exportSettings();
    if (!$this->providesVariables()) {
      // Abbreviate the export making "USING" implicit.
      $export = isset($export['USING']) ? $export['USING'] : array();
    }
    return array($this->elementName => $export);
  }

  public function dependencies() {
    $modules = array_flip(parent::dependencies());
    $modules += array_flip((array) $this->__call('dependencies'));
    return array_keys($modules + (isset($this->info['module']) ? array($this->info['module'] => 1) : array()));
  }

  public function executeByArgs($args = array()) {
    $replacements = array('%label' => $this->label(), '@plugin' => $this->itemName);
    rules_log('Executing @plugin %label.', $replacements, RulesLog::INFO, $this, TRUE);
    $this->processSettings();
    // If there is no element info, just pass through the passed arguments.
    // That way we support executing actions without any info at all.
    if ($this->info()) {
      $state = $this->setUpState($args);
      module_invoke_all('rules_config_execute', $this);

      $result = $this->evaluate($state);
      $return = $this->returnVariables($state, $result);
    }
    else {
      rules_log('Unable to execute @plugin %label.', $replacements, RulesLog::ERROR, $this);
    }
    $state->cleanUp();
    rules_log('Finished executing of @plugin %label.', $replacements, RulesLog::INFO, $this, FALSE);
    return $return;
  }

  /**
   * Execute the configured execution callback and log that.
   */
  abstract protected function executeCallback(array $args, RulesState $state = NULL);


  public function evaluate(RulesState $state) {
    $this->processSettings();
    try {
      // Get vars as needed for execute and call it.
      return $this->executeCallback($this->getExecutionArguments($state), $state);
    }
    catch (RulesEvaluationException $e) {
      rules_log($e->msg, $e->args, $e->severity);
      rules_log('Unable to evaluate %name.', array('%name' => $this->getPluginName()), RulesLog::WARN, $this);
    }
    // Catch wrapper exceptions that might occur due to failures loading an
    // entity or similar.
    catch (EntityMetadataWrapperException $e) {
      rules_log('Unable to get a data value. Error: !error', array('!error' => $e->getMessage()), RulesLog::WARN);
      rules_log('Unable to evaluate %name.', array('%name' => $this->getPluginName()), RulesLog::WARN, $this);
    }
  }

  public function __sleep() {
    return parent::__sleep() + array('elementName' => 'elementName');
  }

  public function getPluginName() {
    return $this->itemName ." ". $this->elementName;
  }

  /**
   * Gets the name of the configured action or condition.
   */
  public function getElementName() {
    return $this->elementName;
  }

  /**
   * Add in the data provided by the info hooks to the cache.
   */
  public function rebuildCache(&$itemInfo, &$cache) {
    parent::rebuildCache($itemInfo, $cache);

    // Include all declared files so we can find all implementations.
    self::includeFiles();

    // Get the plugin's own info data.
    $cache[$this->itemName .'_info'] = rules_fetch_data($this->itemName .'_info');
    foreach ($cache[$this->itemName .'_info'] as $name => &$info) {
      $info += array(
        'parameter' => isset($info['arguments']) ? $info['arguments'] : array(),
        'provides' => isset($info['new variables']) ? $info['new variables'] : array(),
        'base' => $name,
        'callbacks' => array(),
      );
      unset($info['arguments'], $info['new variables']);
      $info['callbacks'] += array('execute' => $info['base']);

      // Build up the per plugin implementation faces cache.
      foreach ($this->faces as $interface) {
        $methods = $file_names = array();
        $includes = self::getIncludeFiles($info['module']);

        foreach (get_class_methods($interface) as $method) {
          if (isset($info['callbacks'][$method]) && ($function = $info['callbacks'][$method])) {
            $methods[$method][0] = $function;
            $file_names[$method] = $this->getFileName($function, $includes);
          }
          elseif (function_exists($function = $info['base'] . '_' . $method)) {
            $methods[$method][0] = $function;
            $file_names[$method] = $this->getFileName($function, $includes);
          }
        }
        // Cache only the plugin implementation specific callbacks.
        $info['faces_cache'][$interface] = array($methods, array_filter($file_names));
      }
      // Filter out interfaces with no overriden methods.
      $info['faces_cache'] = rules_filter_array($info['faces_cache'], 0, TRUE);
      // We don't need that any more.
      unset($info['callbacks'], $info['base']);
    }
  }

  /**
   * Makes sure the providing modules' .rules.inc file is included as diverse
   * callbacks may reside in that file.
   */
  protected function loadBasicInclude() {
    static $included = array();

    if (isset($this->info['module']) && !isset($included[$this->info['module']])) {
      $module = $this->info['module'];
      module_load_include('inc', $module, $module . '.rules');
      $included[$module] = TRUE;
    }
  }

  /**
   * Make sure all supported destinations are included.
   */
  protected static function includeFiles() {
    static $included;

    if (!isset($included)) {
      foreach (module_implements('rules_file_info') as $module) {
        // rules.inc are already included thanks to the rules_hook_info() group.
        foreach (self::getIncludeFiles($module, FALSE) as $name) {
          module_load_include('inc', $module, $name);
        }
      }
      $included = TRUE;
    }
  }

  /**
   * Returns all include files for a module. If $all is set to FALSE the
   * $module.rules.inc file isn't added.
   */
  protected static function getIncludeFiles($module, $all = TRUE) {
    $files = (array)module_invoke($module, 'rules_file_info');
    // Automatically add "$module.rules_forms.inc" and "$module.rules.inc".
    $files[] = $module . '.rules_forms';
    if ($all) {
      $files[] = $module . '.rules';
    }
    return $files;
  }

  protected function getFileName($function, $includes) {
    $reflector = new ReflectionFunction($function);
    // On windows the path contains backslashes instead of slashes, fix that.
    $file = str_replace('\\', '/', $reflector->getFileName());
    foreach ($includes as $include) {
      $pos = strpos($file, $include . '.inc');
      // Test whether the file ends with the given filename.inc.
      if ($pos !== FALSE && strlen($file) - $pos == strlen($include) + 4) {
        return $include;
      }
    }
  }
}

/**
 * Interface for objects that can be used as action.
 */
interface RulesActionInterface {

  /**
   * @return As specified.
   *
   * @throws RulesEvaluationException
   *   Throws an exception if not all necessary arguments have been provided.
   */
  public function execute();
}

/**
 * Interface for objects that can be used as condition.
 */
interface RulesConditionInterface {

  /**
   * @return Boolean.
   *
   * @throws RulesEvaluationException
   *   Throws an exception if not all necessary arguments have been provided.
   */
  public function execute();

  /**
   * Negate the result.
   */
  public function negate($negate = TRUE);

  /**
   * Returns whether the element is configured to negate the result.
   */
  public function isNegated();
}

interface RulesTriggerableInterface {

  /**
   * Returns a reference on the array of event names associated with this
   * object.
   */
  public function &events();

  /**
   * Adds the specified event.
   *
   * @return RulesTriggerableInterface
   */
  public function event($event);
}

/**
 * Provides the interface used for implementing an abstract plugin by using
 * the Faces extension mechanism.
 */
interface RulesPluginImplInterface {

  /**
   * Execute the action or condition making use of the parameters as specified.
   */
  public function execute();

  /**
   * Validates $settings independent from a form submission.
   *
   * @throws RulesIntegrityException
   *   In case of validation errors, RulesIntegrityExceptions are thrown.
   */
  public function validate();

  /**
   * Processes settings independent from a form submission.
   *
   * Processing results may be stored and accessed on execution time in $settings.
   */
  public function process();

  /**
   * Allows altering of the element's action/condition info.
   *
   * Note that this method is also invoked on evaluation time, thus any costly
   * operations should be avoided.
   *
   * @param $element_info
   *   A reference on the element's info as returned by RulesPlugin::info().
   */
  public function info_alter(&$element_info);

  /**
   * Checks whether the user has access to configure this element.
   *
   * Note that this only covers access for already created elements. In order to
   * control access for creating or using elements specify an 'access callback'
   * in the element's info array.
   *
   * @see hook_rules_action_info()
   */
  public function access();

  /**
   * Returns an array of required modules.
   */
  public function dependencies();

  /**
   * Alter the generated configuration form of the element.
   *
   * Validation and processing of the settings should be untied from the form
   * and implemented in validate() and process() wherever it makes sense.
   * For the remaining cases where form tied validation and processing is needed
   * make use of the form API #element_validate and #value_callback properties.
   */
  public function form_alter(&$form, $form_state, $options);


  /**
   * Optionally returns an array of info assertions for the specified
   * parameters. This allows conditions to assert additional metadata, such as
   * info about the fields of a bundle.
   *
   * @see RulesPlugin::variableInfoAssertions()
   */
  public function assertions();

}

/**
 * Interface for optimizing evaluation.
 *
 * @see RulesContainerPlugin::optimize()
 */
interface RulesOptimizationInterface {
  /**
   * Optimizes a rule configuration in order to speed up evaluation.
   */
  public function optimize();
}

/**
 * Class providing default implementations of the methods of the RulesPluginImplInterface.
 *
 * If a plugin implementation does not provide a function for a method, the
 * default method of this class will be invoked.
 *
 * @see RulesPluginImplInterface
 * @see RulesAbstractPlugin
 */
class RulesAbstractPluginDefaults extends FacesExtender implements RulesPluginImplInterface {

  public function execute() {
    throw new RulesEvaluationException($this->object->getPluginName() .": Execution implementation is missing.", array(), $this->object, RulesLog::ERROR);
  }

  /**
   * Implements RulesPluginImplInterface.
   */
  public function access() {
    return TRUE;
  }

  public function validate() {}
  public function process() {}
  public function info_alter(&$element_info) {}
  public function dependencies() {}
  public function form_alter(&$form, $form_state, $options) {}
  public function assertions() {}
}

/**
 * A RecursiveIterator for rule elements.
 */
class RulesRecursiveElementIterator extends ArrayIterator implements RecursiveIterator {

   public function getChildren() {
     return $this->current()->getIterator();
   }

   public function hasChildren() {
      return $this->current() instanceof IteratorAggregate;
   }
}

/**
 * Base class for ContainerPlugins like Rules, Logical Operations or Loops.
 */
abstract class RulesContainerPlugin extends RulesPlugin implements IteratorAggregate {

  protected $children = array();

  public function __construct($variables = array()) {
    $this->setUp();
    if (!empty($variables) && $this->isRoot()) {
      $this->info['variables'] = $variables;
    }
  }

  /**
   * Returns the specified variables, in case the plugin is used as component.
   */
  public function &componentVariables() {
    if ($this->isRoot()) {
      $this->info += array('variables' => array());
      return $this->info['variables'];
    }
    // We have to return a reference in any case.
    $return = NULL;
    return $return;
  }

  /**
   * Allow access to the children through the iterator.
   *
   * @return RulesRecursiveElementIterator
   */
  public function getIterator() {
    return new RulesRecursiveElementIterator($this->children);
  }

  /**
   * @return RulesContainerPlugin
   */
  public function integrityCheck() {
    if (!empty($this->info['variables']) && !$this->isRoot()) {
      throw new RulesIntegrityException(t('%plugin: Specifying state variables is not possible for child elements.', array('%plugin' => $this->getPluginName())), $this);
    }
    parent::integrityCheck();
    foreach ($this->children as $child) {
      $child->integrityCheck();
    }
    return $this;
  }

  public function dependencies() {
    $modules = array();
    foreach ($this->children as $child) {
      $modules += array_flip($child->dependencies());
    }
    return array_keys($modules);
  }

  public function parameterInfo($optional = FALSE) {
    $params = parent::parameterInfo($optional);
    if (isset($this->info['variables'])) {
      foreach ($this->info['variables'] as $name => $var_info) {
        if (empty($var_info['handler']) && (!isset($var_info['parameter']) || $var_info['parameter'])) {
          $params[$name] = $var_info;
          // For lists allow empty variables by default.
          if (entity_property_list_extract_type($var_info['type'])) {
            $params[$name] += array('allow null' => TRUE);
          }
        }
      }
    }
    return $params;
  }

  public function availableVariables() {
    if (!isset($this->availableVariables)) {
      if ($this->isRoot()) {
        $this->availableVariables = RulesState::defaultVariables();
        if (isset($this->info['variables'])) {
          $this->availableVariables += $this->info['variables'];
        }
      }
      else {
        $this->availableVariables = $this->parent->stateVariables($this);
      }
    }
    return $this->availableVariables;
  }

  /**
   * Returns info about variables available in the evaluation state for any
   * children elements or if given for a special child element.
   *
   * @param $element
   *   The element for which the available state variables should be returned.
   *   If NULL is given, the variables available before any children are invoked
   *   are returned. If set to TRUE, the variables available after evaluating
   *   all children will be returned.
   */
  protected function stateVariables($element = NULL) {
    $vars = $this->availableVariables();
    if (isset($element)) {
      // Add in variables provided by siblings executed before the element.
      foreach ($this->children as $child) {
        if ($child === $element) {
          break;
        }
        $vars += $child->providesVariables();
        // Take variable info assertions into account.
        if ($assertions = $child->variableInfoAssertions()) {
          $vars = RulesData::addMetadataAssertions($vars, $assertions);
        }
      }
    }
    return $vars;
  }

  protected function variableInfoAssertions() {
    $assertions = array();
    foreach ($this->children as $child) {
      if ($add = $child->variableInfoAssertions()) {
        $assertions = rules_update_array($assertions, $add);
      }
    }
    return $assertions;
  }

  protected function setUpVariables() {
    return isset($this->info['variables']) ? parent::parameterInfo(TRUE) + $this->info['variables'] : $this->parameterInfo(TRUE);
  }

  /**
   * Condition containers just return a boolean while action containers return
   * the configured provided variables as an array of variables.
   */
  public function executeByArgs($args = array()) {
    $replacements = array('%label' => $this->label(), '@plugin' => $this->itemName);
    rules_log('Executing @plugin %label.', $replacements, RulesLog::INFO, $this, TRUE);
    $this->processSettings();
    $state = $this->setUpState($args);

    // Handle recursion prevention.
    if ($state->isBlocked($this)) {
      return rules_log('Not evaluating @plugin %label to prevent recursion.', array('%label' => $this->label(), '@plugin' => $this->plugin()), RulesLog::INFO);
    }
    // Block the config to prevent any future recursion.
    $state->block($this);

    module_invoke_all('rules_config_execute', $this);
    $result = $this->evaluate($state);
    $return = $this->returnVariables($state, $result);

    $state->unblock($this);
    $state->cleanUp();
    rules_log('Finished executing of @plugin %label.', $replacements, RulesLog::INFO, $this, FALSE);
    return $return;
  }

  public function access() {
    foreach ($this->children as $key => $child) {
      if (!$child->access()) {
        return FALSE;
      }
    }
    return TRUE;
  }

  public function destroy() {
    foreach ($this->children as $key => $child) {
      $child->destroy();
    }
    parent::destroy();
  }

  /**
   * By default we do a deep clone.
   */
  public function __clone() {
    parent::__clone();
    foreach ($this->children as $key => $child) {
      $this->children[$key] = clone $child;
      $this->children[$key]->parent = $this;
    }
  }

  /**
   * Override delete to keep the children alive, if possible.
   */
  public function delete($keep_children = TRUE) {
    if (isset($this->parent) && $keep_children) {
      foreach ($this->children as $child) {
        $child->setParent($this->parent);
      }
    }
    parent::delete();
  }

  public function __sleep() {
    return parent::__sleep() + array('children' => 'children', 'info' => 'info');
  }

  /**
   * Sorts all child elements by their weight.
   *
   * @param $deep
   *   If enabled a deep sort is performed, thus the whole element tree below
   *   this element is sorted.
   */
  public function sortChildren($deep = FALSE) {
    // Make sure the array order is kept in case two children have the same
    // weight by ensuring later childrens would have higher weights.
    foreach (array_values($this->children) as $i => $child) {
      $child->weight += $i / 1000;
    }
    usort($this->children, array('RulesPlugin', 'compare'));

    // Fix up the weights afterwards to be unique integers.
    foreach (array_values($this->children) as $i => $child) {
      $child->weight = $i * 2;
    }

    if ($deep) {
      foreach (new ParentIterator($this->getIterator()) as $child) {
        $child->sortChildren(TRUE);
      }
    }
    $this->resetInternalCache();
  }

  protected function exportChildren($key = NULL) {
    $key = isset($key) ? $key : strtoupper($this->plugin());
    $export[$key] = array();
    foreach ($this->children as $child) {
      $export[$key][] = $child->export();
    }
    return $export;
  }

  /**
   * Determines whether the element should be exported in flat style. Flat style
   * means that the export keys are written directly into the export array,
   * whereas else the export is written into a sub-array.
   */
  protected function exportFlat() {
    // By default we always use flat style for plugins without any parameters
    // or provided variables, as then only children have to be exported. E.g.
    // this applies to the OR and AND plugins.
    return $this->isRoot() || (!$this->pluginParameterInfo() && !$this->providesVariables());
  }

  protected function exportToArray() {
    $export = array();
    if (!empty($this->info['variables'])) {
      $export['USES VARIABLES'] = $this->info['variables'];
    }
    if ($this->exportFlat()) {
      $export += $this->exportSettings() + $this->exportChildren();
    }
    else {
      $export[strtoupper($this->plugin())] = $this->exportSettings() + $this->exportChildren();
    }
    return $export;
  }

  public function import(array $export) {
    if (!empty($export['USES VARIABLES'])) {
      $this->info['variables'] = $export['USES VARIABLES'];
    }
    // Care for exports having the export array nested in a sub-array.
    if (!$this->exportFlat()) {
      $export = reset($export);
    }
    $this->importSettings($export);
    $this->importChildren($export);
  }

  protected function importChildren($export, $key = NULL) {
    $key = isset($key) ? $key : strtoupper($this->plugin());
    foreach ($export[$key] as $child_export) {
      $plugin = _rules_import_get_plugin(rules_array_key($child_export), $this instanceof RulesActionInterface ? 'action' : 'condition');
      $child = rules_plugin_factory($plugin);
      $child->setParent($this);
      $child->import($child_export);
    }
  }

  public function resetInternalCache() {
    $this->availableVariables = NULL;
    foreach ($this->children as $child) {
      $child->resetInternalCache();
    }
  }
}

/**
 * Base class for all action containers.
 */
abstract class RulesActionContainer extends RulesContainerPlugin implements RulesActionInterface {

  public function __construct($variables = array(), $providesVars = array()) {
    parent::__construct($variables);
    // The provided vars of a component are the names of variables, which should
    // be provided to the caller. See rule().
    if ($providesVars) {
      $this->info['provides'] = $providesVars;
    }
  }

  /**
   * Add an action. Pass either an instance of the RulesActionInterface
   * or the arguments as needed by rules_action().
   *
   * @return RulesActionContainer
   *   Returns $this to support chained usage.
   */
  public function action($name, $settings = array()) {
    $action = (is_object($name) && $name instanceof RulesActionInterface) ? $name : rules_action($name, $settings);
    $action->setParent($this);
    return $this;
  }

  /**
   * Evaluate, whereas by default new vars are visible in the parent's scope.
   */
  public function evaluate(RulesState $state) {
    foreach ($this->children as $action) {
      $action->evaluate($state);
    }
  }

  public function pluginProvidesVariables() {
    return array();
  }

  public function providesVariables() {
    $provides = parent::providesVariables();
    if (isset($this->info['provides']) && $vars = $this->componentVariables()) {
      // Determine the full variable info for the provided variables. Note that
      // we only support providing variables list in the component vars.
      $provides += array_intersect_key($vars, array_flip($this->info['provides']));
    }
    return $provides;
  }

  /**
   * Returns an array of variable names, which are provided by passing through
   * the provided variables of the children.
   */
  public function &componentProvidesVariables() {
    $this->info += array('provides' => array());
    return $this->info['provides'];
  }

  protected function exportToArray() {
    $export = parent::exportToArray();
    if (!empty($this->info['provides'])) {
      $export['PROVIDES VARIABLES'] = $this->info['provides'];
    }
    return $export;
  }

  public function import(array $export) {
    parent::import($export);
    if (!empty($export['PROVIDES VARIABLES'])) {
      $this->info['provides'] = $export['PROVIDES VARIABLES'];
    }
  }
}

/**
 * Base class for all condition containers.
 */
abstract class RulesConditionContainer extends RulesContainerPlugin implements RulesConditionInterface {

  protected $negate = FALSE;

  /**
   * Add a condition. Pass either an instance of the RulesConditionInterface
   * or the arguments as needed by rules_condition().
   *
   * @return RulesConditionContainer
   *   Returns $this to support chained usage.
   */
  public function condition($name, $settings = array()) {
    $condition = (is_object($name) && $name instanceof RulesConditionInterface) ? $name : rules_condition($name, $settings);
    $condition->setParent($this);
    return $this;
  }

  /**
   * Negate this condition.
   *
   * @return RulesConditionContainer
   */
  public function negate($negate = TRUE) {
    $this->negate = (bool) $negate;
    return $this;
  }

  public function isNegated() {
    return $this->negate;
  }

  public function __sleep() {
    return parent::__sleep() + array('negate' => 'negate');
  }

  /**
   * Just return the condition container's result.
   */
  protected function returnVariables(RulesState $state, $result = NULL) {
    return $result;
  }

  protected function exportChildren($key = NULL) {
    $key = isset($key) ? $key : strtoupper($this->plugin());
    return parent::exportChildren($this->negate ? 'NOT ' . $key : $key);
  }

  protected function importChildren($export, $key = NULL) {
    $key = isset($key) ? $key : strtoupper($this->plugin());
    // Care for negated elements.
    if (!isset($export[$key]) && isset($export['NOT ' . $key])) {
      $this->negate = TRUE;
      $key = 'NOT ' . $key;
    }
    parent::importChildren($export, $key);
  }

  /**
   * Overridden to exclude variable assertions of negated conditions.
   */
  protected function stateVariables($element = NULL) {
    $vars = $this->availableVariables();
    if (isset($element)) {
      // Add in variables provided by siblings executed before the element.
      foreach ($this->children as $child) {
        if ($child === $element) {
          break;
        }
        $vars += $child->providesVariables();
        // Take variable info assertions into account.
        if (!$this->negate && !$child->isNegated() && ($assertions = $child->variableInfoAssertions())) {
          $vars = RulesData::addMetadataAssertions($vars, $assertions);
        }
      }
    }
    return $vars;
  }
}

/**
 * The rules default logging class.
 */
class RulesLog {

  const INFO  = 1;
  const WARN  = 2;
  const ERROR = 3;

  static protected $logger;

  /**
   * @return RulesLog
   *   Returns the rules logger instance.
   */
  static function logger() {
    if (!isset(self::$logger)) {
      $class = __CLASS__;
      self::$logger = new $class(variable_get('rules_log_level', self::INFO));
    }
    return self::$logger;
  }

  protected $log = array();
  protected $logLevel, $line = 0;

  /**
   * This is a singleton.
   */
  protected function __construct($logLevel = self::WARN) {
    $this->logLevel = $logLevel;
  }

  public function __clone() {
    throw new Exception("Cannot clone the logger.");
  }

  /**
   * Logs a log message.
   *
   * @see rules_log()
   */
  public function log($msg, $args = array(), $logLevel = self::INFO, $scope = NULL, $path = NULL) {
    if ($logLevel >= $this->logLevel) {
      $this->log[] = array($msg, $args, $logLevel, microtime(TRUE), $scope, $path);
    }
  }

  /**
   * Checks the log and throws an exception if there were any problems.
   */
  function checkLog($logLevel = self::WARN) {
    foreach ($this->log as $entry) {
      if ($entry[2] >= $logLevel) {
        throw new Exception($this->render());
      }
    }
  }

  /**
   * Checks the log for (error) messages with a log level equal or higher than the given one.
   *
   * @return
   *   Whether the an error has been logged.
   */
  public function hasErrors($logLevel = self::WARN) {
    foreach ($this->log as $entry) {
      if ($entry[2] >= $logLevel) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * Gets an array of logged messages.
   */
  public function get() {
    return $this->log;
  }

  /**
   * Renders the whole log.
   */
  public function render() {
    $line = 0;
    $output = array();
    while (isset($this->log[$line])) {
      $vars['head'] = t($this->log[$line][0], $this->log[$line][1]);
      $vars['log'] = $this->renderHelper($line);
      $output[] = theme('rules_debug_element', $vars);
      $line++;
    }
    return implode('', $output);
  }

  /**
   * Renders the log of one event invocation.
   */
  protected function renderHelper(&$line = 0) {
    $startTime = isset($this->log[$line][3]) ? $this->log[$line][3] : 0;
    $output = array();
    while ($line < count($this->log)) {
      if ($output && !empty($this->log[$line][4])) {
        // The next entry stems from another evaluated set, add in its log
        // messages here.
        $vars['head'] = t($this->log[$line][0], $this->log[$line][1]);
        if (isset($this->log[$line][5])) {
          $vars['link'] = '[' . l('edit', $this->log[$line][5]) . ']';
        }
        $vars['log'] = $this->renderHelper($line);
        $output[] = theme('rules_debug_element', $vars);
      }
      else {
        $formatted_diff = round(($this->log[$line][3] - $startTime) * 1000, 3) .' ms';
        $msg = $formatted_diff .' '. t($this->log[$line][0], $this->log[$line][1]);
        if ($this->log[$line][2] >= RulesLog::WARN) {
          $level = $this->log[$line][2] == RulesLog::WARN ? 'warn' : 'error';
          $msg = '<span class="rules-debug-' . $level . '">'. $msg .'</span>';
        }
        if (isset($this->log[$line][5]) && !isset($this->log[$line][4])) {
          $msg .= ' [' . l('edit', $this->log[$line][5]) . ']';
        }
        $output[] = $msg;

        if (isset($this->log[$line][4]) && !$this->log[$line][4]) {
          // This was the last log entry of this set.
          return theme('item_list', array('items' => $output));
        }
      }
      $line++;
    }
    return theme('item_list', array('items' => $output));
  }

  /**
   * Clears the logged messages.
   */
  public function clear() {
    $this->log = array();
  }
}

/**
 * A common exception for Rules.
 *
 * This class can be used to catch all exceptions thrown by Rules.
 */
abstract class RulesException extends Exception {}

/**
 * An exception that is thrown during evaluation.
 *
 * Messages are prepared to be logged to the watchdog, thus not yet translated.
 *
 * @see watchdog()
 */
class RulesEvaluationException extends RulesException {

  public $msg, $args, $severity, $element, $keys = array();

  /**
   * @param $msg
   *   The exception message containing placeholder as t().
   * @param $args
   *   Replacement arguments such as for t().
   * @param $element
   *   The element of a configuration causing the exception or an array
   *   consisting of the element and keys specifying a setting value causing
   *   the exception.
   * @param $severity
   *   The RulesLog severity. Defaults to RulesLog::WARN.
   */
  function __construct($msg, array $args = array(), $element = NULL, $severity = RulesLog::WARN) {
    $this->element = is_array($element) ? array_shift($element) : $element;
    $this->keys = is_array($element) ? $element : array();
    $this->msg = $msg;
    $this->args = $args;
    $this->severity = $severity;
    // If an error happened, run the integrity check on the rules configuration
    // and mark it as dirty if it the check fails.
    if ($severity == RulesLog::ERROR && isset($this->element)) {
      $rules_config = $this->element->root();
      rules_config_update_dirty_flag($rules_config);
      // If we discovered a broken configuration, exclude it in future.
      if ($rules_config->dirty) {
        rules_clear_cache();
      }
    }
    // @todo fix _drupal_decode_exception() to use __toString() and override it.
    $this->message = t($this->msg, $this->args);
  }
}

/**
 * An exception that is thrown for Rules configurations that fail the integrity check.
 *
 * @see RulesPlugin::integrityCheck()
 */
class RulesIntegrityException extends RulesException {

  public $msg, $element, $keys = array();

  /**
   * @param string $msg
   *   The exception message, already translated.
   * @param $element
   *   The element of a configuration causing the exception or an array
   *   consisting of the element and keys specifying a parameter or provided
   *   variable causing the exception, e.g.
   *   @code array($element, 'parameter', 'node') @endcode.
   */
  function __construct($msg, $element = NULL) {
    $this->element = is_array($element) ? array_shift($element) : $element;
    $this->keys = is_array($element) ? $element : array();
    parent::__construct($msg);
  }
}

/**
 * An exception that is thrown for missing module dependencies.
 */
class RulesDependencyException extends RulesIntegrityException {}

/**
 * Determines the plugin to be used for importing a child element.
 *
 * @param $key
 *   The key to look for, e.g. 'OR' or 'DO'.
 * @param $default
 *   The default to return if no special plugin can be found.
 */
function _rules_import_get_plugin($key, $default = 'action') {
  $map = &drupal_static(__FUNCTION__);
  if (!isset($map)) {
    $cache = rules_get_cache();
    foreach ($cache['plugin_info'] as $name => $info) {
      if (!empty($info['embeddable'])) {
        $info += array('import keys' => array(strtoupper($name)));
        foreach ($info['import keys'] as $k) {
          $map[$k] = $name;
        }
      }
    }
  }
  // Cut of any leading NOT from the key.
  if (strpos($key, 'NOT ') === 0) {
    $key = substr($key, 4);
  }
  if (isset($map[$key])) {
    return $map[$key];
  }
  return $default;
}
