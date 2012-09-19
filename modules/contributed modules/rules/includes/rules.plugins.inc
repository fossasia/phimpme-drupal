<?php

/**
 * @file Contains plugin info and implementations not needed for rule evaluation.
 */


/**
 * Implements a rules action.
 */
class RulesAction extends RulesAbstractPlugin implements RulesActionInterface {

  protected $itemName = 'action';

  /**
   * Execute the callback and update/save data as specified by the action.
   */
  protected function executeCallback(array $args, RulesState $state = NULL) {
    rules_log('Evaluating the action %name.', array('%name' => $this->elementName), RulesLog::INFO, $this);
    $return = $this->__call('execute', empty($this->info['named parameter']) ? $args : array($args));
    // Get the (partially) wrapped arguments.
    $args = $state->currentArguments;

    if (is_array($return)) {
      foreach ($return as $name => $data) {
        // Add provided variables.
        if (isset($this->info['provides'][$name])) {
          $var_name = isset($this->settings[$name . ':var']) ? $this->settings[$name . ':var'] : $name;
          if (!$state->varInfo($var_name)) {
            $state->addVariable($var_name, $data, $this->info['provides'][$name]);
            rules_log('Added the provided variable %name of type %type', array('%name' => $var_name, '%type' => $this->info['provides'][$name]['type']), RulesLog::INFO, $this);
            if (!empty($this->info['provides'][$name]['save']) && $state->variables[$var_name] instanceof EntityMetadataWrapper) {
              $state->saveChanges($var_name, $state->variables[$var_name]);
            }
          }
        }
        // Support updating variables by returning the values.
        elseif (!isset($this->info['provides'][$name])) {
          // Update the data value using the wrapper.
          if (isset($args[$name]) && $args[$name] instanceof EntityMetadataWrapper) {
            try {
              $args[$name]->set($data);
            }
            catch (EntityMetadataWrapperException $e) {
              throw new RulesEvaluationException('Unable to update the argument for parameter %name: %error', array('%name' => $name, '%error' => $e->getMessage()), $this);
            }
          }
          elseif (array_key_exists($name, $args)) {
            // Map back to the source variable name and update it.
            $var_name = !empty($this->settings[$name . ':select']) ? str_replace('-', '_', $this->settings[$name . ':select']) : $name;
            $state->variables[$var_name] = $data;
          }
        }
      }
    }
    // Save parameters as defined in the parameter info.
    if ($return !== FALSE) {
      foreach ($this->info['parameter'] as $name => $info) {
        if (!empty($info['save']) && $args[$name] instanceof EntityMetadataWrapper) {
          if (isset($this->settings[$name . ':select'])) {
            $state->saveChanges($this->settings[$name . ':select'], $args[$name]);
          }
          else {
            // Wrapper has been configured via direct input, so just save.
            rules_log('Saved argument of type %type for parameter %name.', array('%name' => $name, '%type' => $args[$name]->type()));
            $args[$name]->save();
          }
        }
      }
    }
  }
}

/**
 * Implements a rules condition.
 */
class RulesCondition extends RulesAbstractPlugin implements RulesConditionInterface {

  protected $itemName = 'condition';
  protected $negate = FALSE;

  public function providesVariables() {
    return array();
  }

  public function negate($negate = TRUE) {
    $this->negate = (bool) $negate;
    return $this;
  }

  public function isNegated() {
    return $this->negate;
  }

  protected function executeCallback(array $args, RulesState $state = NULL) {
    $return = (bool) $this->__call('execute', empty($this->info['named parameter']) ? $args : array($args));
    rules_log('The condition %name evaluated to %bool', array('%name' => $this->elementName, '%bool' => $return ? 'TRUE' : 'FALSE'), RulesLog::INFO, $this);
    return $this->negate ? !$return : $return;
  }

  public function __sleep() {
    return parent::__sleep() + array('negate' => 'negate');
  }

  /**
   * Just return the boolean result.
   */
  protected function returnVariables(RulesState $state, $result = NULL) {
    return $result;
  }

  protected function exportToArray() {
    $not = $this->negate ? 'NOT ' : '';
    $export = $this->exportSettings();
    // Abbreviate the export making "USING" implicit.
    return array($not . $this->elementName => isset($export['USING']) ? $export['USING'] : array());
  }

  public function import(array $export) {
    $this->elementName = rules_array_key($export);
    if (strpos($this->elementName, 'NOT ') === 0) {
      $this->elementName = substr($this->elementName, 4);
      $this->negate = TRUE;
    }
    // After setting the element name, setup the element again so the right
    // element info is loaded.
    $this->setUp();

    // Re-add 'USING' which has been removed for abbreviation.
    $this->importSettings(array('USING' => reset($export)));
  }

  public function label() {
    $label = parent::label();
    return $this->negate ? t('NOT @condition', array('@condition' => $label)) : $label;
  }
}

/**
 * An actual rule.
 * Note: A rule also implements the RulesActionInterface (inherited).
 */
class Rule extends RulesActionContainer {

  protected $conditions = NULL;
  protected $itemName = 'rule';

  public $label = 'unlabeled';

  public function __construct($variables = array(), $providesVars = array()) {
    parent::__construct($variables, $providesVars);

    // Initialize the conditions container.
    if (!isset($this->conditions)) {
      $this->conditions = rules_and();
      // Don't use setParent() to avoid having it added to the children.
      $this->conditions->parent = $this;
    }
  }

  /**
   * Get an iterator over all contained conditions. Note that this iterator also
   * implements the ArrayAcces interface.
   *
   * @return RulesRecursiveElementIterator
   */
  public function conditions() {
    return $this->conditions->getIterator();
  }

  /**
   * Returns the "And" condition container, which contains all conditions of
   * this rule.
   *
   * @return RulesAnd
   */
  public function conditionContainer() {
    return $this->conditions;
  }

  public function __sleep() {
    return parent::__sleep() + drupal_map_assoc(array('conditions', 'label'));
  }

  /**
   * Get an iterator over all contained actions. Note that this iterator also
   * implements the ArrayAcces interface.
   *
   * @return RulesRecursiveElementIterator
   */
  public function actions() {
    return parent::getIterator();
  }

  /**
   * Add a condition. Pass either an instance of the RulesConditionInterface
   * or the arguments as needed by rules_condition().
   *
   * @return Rule
   *   Returns $this to support chained usage.
   */
  public function condition($name, $settings = array()) {
    $this->conditions->condition($name, $settings);
    return $this;
  }

  public function sortChildren($deep = FALSE) {
    $this->conditions->sortChildren($deep);
    parent::sortChildren($deep);
  }

  public function evaluate(RulesState $state) {
    rules_log('Evaluating conditions of rule %label.', array('%label' => $this->label), RulesLog::INFO, $this);
    if ($this->conditions->evaluate($state)) {
      rules_log('Rule %label fires.', array('%label' => $this->label), RulesLog::INFO, $this, TRUE);
      parent::evaluate($state);
      rules_log('Rule %label has fired.', array('%label' => $this->label), RulesLog::INFO, $this, FALSE);
    }
  }

  /**
   * Fires the rule, i.e. evaluates the rule without checking its conditions.
   *
   * @see RulesPlugin::evaluate()
   */
  public function fire(RulesState $state) {
    rules_log('Firing rule %label.', array('%label' => $this->label), RulesLog::INFO, $this);
    parent::evaluate($state);
  }

  public function integrityCheck() {
    parent::integrityCheck();
    $this->conditions->integrityCheck();
    return $this;
  }

  public function access() {
    return (!isset($this->conditions) || $this->conditions->access()) && parent::access();
  }

  public function dependencies() {
    return array_keys(array_flip($this->conditions->dependencies()) + array_flip(parent::dependencies()));
  }

  public function destroy() {
    $this->conditions->destroy();
    parent::destroy();
  }

  /**
   * @return RulesRecursiveElementIterator
   */
  public function getIterator() {
    $array = array_merge(array($this->conditions), $this->children);
    return new RulesRecursiveElementIterator($array);
  }

  protected function stateVariables($element = NULL) {
    // Don't add in provided action variables for the conditions.
    if (isset($element) && $element === $this->conditions) {
      return $this->availableVariables();
    }
    $vars = parent::stateVariables($element);
    // Take variable info assertions of conditions into account.
    if ($assertions = $this->conditions->variableInfoAssertions()) {
      $vars = RulesData::addMetadataAssertions($vars, $assertions);
    }
    return $vars;
  }

  protected function exportFlat() {
    return $this->isRoot();
  }

  protected function exportToArray() {
    $export = parent::exportToArray();
    if (!$this->isRoot()) {
      $export[strtoupper($this->plugin())]['LABEL'] = $this->label;
    }
    return $export;
  }

  protected function exportChildren($key = NULL) {
    $export = array();
    if ($this->conditions->children) {
      $export = $this->conditions->exportChildren('IF');
    }
    return $export + parent::exportChildren('DO');
  }

  public function import(array $export) {
    if (!$this->isRoot() && isset($export[strtoupper($this->plugin())]['LABEL'])) {
      $this->label = $export[strtoupper($this->plugin())]['LABEL'];
    }
    parent::import($export);
  }

  protected function importChildren($export, $key = NULL) {
    if (!empty($export['IF'])) {
      $this->conditions->importChildren($export, 'IF');
    }
    parent::importChildren($export, 'DO');
  }

  public function __clone() {
    parent::__clone();
    $this->conditions = clone $this->conditions;
    $this->conditions->parent = $this;
  }

  /**
   * Rules may not provided any variable info assertions, as Rules are only
   * conditionally executed.
   */
  protected function variableInfoAssertions() {
    return array();
  }

  /**
   * Overridden to ensure the whole Rule is deleted at once.
   */
  public function delete($keep_children = FALSE) {
    parent::delete($keep_children);
  }

  /**
   * Overriden to expose the variables of all actions for embedded rules.
   */
  public function providesVariables() {
    $provides = parent::providesVariables();
    if (!$this->isRoot()) {
      foreach ($this->actions() as $action) {
        $provides += $action->providesVariables();
      }
    }
    return $provides;
  }

  public function resetInternalCache() {
    parent::resetInternalCache();
    $this->conditions->resetInternalCache();
  }
}

/**
 * Represents rules getting triggered by events.
 */
class RulesReactionRule extends Rule implements RulesTriggerableInterface {

  protected $itemName = 'reaction rule';
  protected $events = array();

  /**
   * Returns the array of events associated with that Rule.
   */
  public function &events() {
    return $this->events;
  }

  /**
   * Removes an event from the rule configuration.
   *
   * @param $event
   *   The name of the event to remove.
   * @return RulesReactionRule
   */
  public function removeEvent($event) {
    if (($id = array_search($event, $this->events)) !== FALSE) {
      unset($this->events[$id]);
    }
    return $this;
  }

  /**
   * @return RulesReactionRule
   */
  public function event($event) {
    $this->events[] = $event;
    return $this;
  }

  /**
   * Reaction rules can't add variables to the parent scope, so clone $state.
   */
  public function evaluate(RulesState $state) {
    // Implement recursion prevention for reaction rules.
    if ($state->isBlocked($this)) {
      return rules_log('Not evaluating @plugin %label to prevent recursion.', array('%label' => $this->label(), '@plugin' => $this->plugin()), RulesLog::INFO, $this);
    }
    $state->block($this);
    $copy = clone $state;
    parent::evaluate($copy);
    $state->unblock($this);
  }

  public function access() {
    $event_info = rules_fetch_data('event_info');
    foreach ($this->events as $event) {
      if (!empty($event_info[$event]['access callback']) && !call_user_func($event_info[$event]['access callback'], 'event', $event)) {
        return FALSE;
      }
    }
    return parent::access();
  }

  public function dependencies() {
    $modules = array_flip(parent::dependencies());
    $event_info = rules_fetch_data('event_info');
    foreach ($this->events as $event) {
      if (isset($event_info[$event]['module'])) {
        $modules[$event_info[$event]['module']] = TRUE;
      }
    }
    return array_keys($modules);
  }

  public function providesVariables() {
    return array();
  }

  public function parameterInfo($optional = FALSE) {
    // If executed directly, all variables as defined by the event need to
    // be passed.
    return rules_filter_array($this->availableVariables(), 'handler', FALSE);
  }

  public function availableVariables() {
    if (!isset($this->availableVariables)) {
      if (isset($this->parent)) {
        // Return the event variables provided by the event set, once cached.
        $this->availableVariables = $this->parent->stateVariables();
      }
      else {
        // The intersection of the variables provided by the events are
        // available.
        $event_info = rules_fetch_data('event_info');
        $events = array_intersect($this->events, array_keys($event_info));
        foreach ($events as $event) {
          $event_info[$event] += array('variables' => array());
          if (isset($this->availableVariables)) {
            $this->availableVariables = array_intersect_key($this->availableVariables, $event_info[$event]['variables']);
          }
          else {
            $this->availableVariables = $event_info[$event]['variables'];
          }
        }
        $this->availableVariables = isset($this->availableVariables) ? RulesState::defaultVariables() + $this->availableVariables : RulesState::defaultVariables();
      }
    }
    return $this->availableVariables;
  }

  public function __sleep() {
    return parent::__sleep() + drupal_map_assoc(array('events'));
  }

  protected function exportChildren($key = 'ON') {
    $export[$key] = array_values($this->events);
    return $export + parent::exportChildren();
  }

  protected function importChildren($export, $key = 'ON') {
    $this->events = $export[$key];
    parent::importChildren($export);
  }
}

/**
 * A logical AND.
 */
class RulesAnd extends RulesConditionContainer {

  protected $itemName = 'and';

  public function evaluate(RulesState $state) {
    foreach ($this->children as $condition) {
      if (!$condition->evaluate($state)) {
        rules_log('AND evaluated to FALSE.');
        return $this->negate;
      }
    }
    rules_log('AND evaluated to TRUE.');
    return !$this->negate;
  }

  public function label() {
    return !empty($this->label) ? $this->label : ($this->negate ? t('NOT AND') : t('AND'));
  }
}

/**
 * A logical OR.
 */
class RulesOr extends RulesConditionContainer {

  protected $itemName = 'or';

  public function evaluate(RulesState $state) {
    foreach ($this->children as $condition) {
      if ($condition->evaluate($state)) {
        rules_log('OR evaluated to TRUE.');
        return !$this->negate;
      }
    }
    rules_log('OR evaluated to FALSE.');
    return $this->negate;
  }

  public function label() {
    return !empty($this->label) ? $this->label : ($this->negate ? t('NOT OR') : t('OR'));
  }

  /**
   * Overridden to exclude all variable assertions as in an OR we cannot assert
   * the children are successfully evaluated.
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
      }
    }
    return $vars;
  }
}

/**
 * A loop element.
 */
class RulesLoop extends RulesActionContainer {

  protected $itemName = 'loop';
  protected $listItemInfo;

  public function __construct($settings = array(), $variables = NULL) {
    $this->setUp();
    $this->settings = (array) $settings + array(
      'item:var' => 'list_item',
      'item:label' => t('Current list item'),
    );
    if (!empty($variables)) {
      $this->info['variables'] = $variables;
    }
  }

  public function pluginParameterInfo() {
    $info['list'] = array(
      'type' => 'list',
      'restriction' => 'selector',
      'label' => t('List'),
      'description' => t('The list to loop over. The loop will step through each item in the list, allowing further actions on them. See <a href="@url"> the online handbook</a> for more information on how to use loops.',
        array('@url' => rules_external_help('loops'))),
    );
    return $info;
  }

  public function integrityCheck() {
    parent::integrityCheck();
    $this->checkVarName($this->settings['item:var']);
  }

  public function listItemInfo() {
    if (!isset($this->listItemInfo)) {
      if ($info = $this->getArgumentInfo('list')) {
        // Pass through the variable info keys like property info.
        $this->listItemInfo = array_intersect_key($info, array_flip(array('type', 'property info', 'bundle')));
        $this->listItemInfo['type'] = isset($info['type']) ? entity_property_list_extract_type($info['type']) : 'unknown';
      }
      else {
        $this->listItemInfo = array('type' => 'unknown');
      }
      $this->listItemInfo['label'] = $this->settings['item:label'];
    }
    return $this->listItemInfo;
  }

  public function evaluate(RulesState $state) {
    try {
      $param_info = $this->pluginParameterInfo();
      $list = $this->getArgument('list', $param_info['list'], $state);
      $item_var_info = $this->listItemInfo();
      $item_var_name = $this->settings['item:var'];

      if (isset($this->settings['list:select'])) {
        rules_log('Looping over the list items of %selector', array('%selector' => $this->settings['list:select']), RulesLog::INFO, $this);
      }

      // Loop over the list and evaluate the children for each list item.
      foreach ($list as $key => $item) {
        // Use a separate state so variables are available in the loop only.
        $state2 = clone $state;
        $state2->addVariable($item_var_name, $list[$key], $item_var_info);
        parent::evaluate($state2);

        // Update variables from parent scope.
        foreach ($state->variables as $var_key => &$var_value) {
          if (array_key_exists($var_key, $state2->variables)) {
            $var_value = $state2->variables[$var_key];
          }
        }
      }
    }
    catch (RulesEvaluationException $e) {
      rules_log($e->msg, $e->args, $e->severity);
      rules_log('Unable to evaluate %name.', array('%name' => $this->getPluginName()), RulesLog::WARN, $this);
    }
  }

  protected function stateVariables($element = NULL) {
    return array($this->settings['item:var'] => $this->listItemInfo()) + parent::stateVariables($element);
  }

  public function label() {
    return !empty($this->label) ? $this->label : t('Loop');
  }

  protected function exportChildren($key = 'DO') {
    return parent::exportChildren($key);
  }

  protected function importChildren($export, $key = 'DO') {
    parent::importChildren($export, $key);
  }

  protected function exportSettings() {
    $export = parent::exportSettings();
    $export['ITEM'][$this->settings['item:var']] = $this->settings['item:label'];
    return $export;
  }

  protected function importSettings($export) {
    parent::importSettings($export);
    if (isset($export['ITEM'])) {
      $this->settings['item:var'] = rules_array_key($export['ITEM']);
      $this->settings['item:label'] = reset($export['ITEM']);
    }
  }
}

/**
 * An action set component.
 */
class RulesActionSet extends RulesActionContainer {

  protected $itemName = 'action set';

}

/**
 * A set of rules to execute upon defined variables.
 */
class RulesRuleSet extends RulesActionContainer {

  protected $itemName = 'rule set';

  /**
   * @return RulesRuleSet
   */
  public function rule($rule) {
    return $this->action($rule);
  }

  protected function exportChildren($key = 'RULES') {
    return parent::exportChildren($key);
  }

  protected function importChildren($export, $key = 'RULES') {
    parent::importChildren($export, $key);
  }
}

/**
 * This class is used for caching the rules to be evaluated per event.
 */
class RulesEventSet extends RulesRuleSet {

  protected $itemName = 'event set';
  // Event sets may recurse as we block recursions on rule-level.
  public $recursion = TRUE;

  public function __construct($info = array()) {
    $this->setup();
    $this->info = $info;
  }

  public function executeByArgs($args = array()) {
    rules_log('Reacting on event %label.', array('%label' => $this->info['label']), RulesLog::INFO, NULL, TRUE);
    $state = $this->setUpState($args);
    module_invoke_all('rules_config_execute', $this);
    $this->evaluate($state);
    $state->cleanUp($this);
    rules_log('Finished reacting on event %label.', array('%label' => $this->info['label']), RulesLog::INFO, NULL, FALSE);
  }

  /**
   * Cache event-sets per event to allow efficient usage via rules_invoke_event().
   *
   * @see rules_get_cache()
   * @see rules_invoke_event()
   */
  public static function rebuildEventCache() {
    // Set up the per-event cache.
    $events = rules_fetch_data('event_info');
    $sets = array();
    // Add all rules associated with this event to an EventSet for caching.
    $rules = rules_config_load_multiple(FALSE, array('plugin' => 'reaction rule', 'active' => TRUE));

    foreach ($rules as $name => $rule) {
      foreach ($rule->events() as $event) {
        // Skip not defined events.
        if (empty($events[$event])) {
          continue;
        }
        // Create an event set if not yet done.
        if (!isset($sets[$event])) {
          $event_info = $events[$event] + array(
            'variables' => isset($events[$event]['arguments']) ? $events[$event]['arguments'] : array(),
          );
          $sets[$event] = new RulesEventSet($event_info);
          $sets[$event]->name = $event;
        }

        // If a rule is marked as dirty, check if this still applies.
        if ($rule->dirty) {
          rules_config_update_dirty_flag($rule);
        }
        if (!$rule->dirty) {
          // Clone the rule to avoid modules getting the changed version from
          // the static cache.
          $sets[$event]->rule(clone $rule);
        }
      }
    }

    // Create cache items for all created sets.
    foreach ($sets as $event => $set) {
      $set->sortChildren();
      $set->optimize();
      // Allow modules to alter the cached event set.
      drupal_alter('rules_event_set', $event, $set);
      rules_set_cache('event_' . $event, $set);
    }
    // Cache a list of empty sets so we can use it to speed up later calls.
    // See rules_get_event_set().
    $empty_events = array_keys(array_diff_key($events, $sets));
    variable_set('rules_empty_sets', array_flip($empty_events));
  }

  protected function stateVariables($element = NULL) {
    return $this->availableVariables();
  }

  /**
   * Do not save since this class is for caching purposes only.
   *
   * @see RulesPlugin::save()
   */
  public function save($name = NULL, $module = 'rules') {
    return FALSE;
  }
}
