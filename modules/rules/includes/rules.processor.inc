<?php

/**
 * @file Contains classes for data processing.
 *
 * Data processors can be used to process element arguments on evaluation time,
 * e.g. to apply input evaluators or to apply simple calculations to number
 * arguments.
 */

/**
 * Common base class for Rules data processors.
 */
abstract class RulesDataProcessor {

  /**
   * The processors' setting value.
   */
  protected $setting = NULL;

  /**
   * Allows chaining processors. If set, the next processor to invoke.
   */
  protected $processor = NULL;

  /**
   * Constructor.
   */
  protected function __construct($setting, $param_info, $var_info = array(), $processor = NULL) {
    $this->setting = $setting;
    $this->processor = $processor;
  }

  /**
   * Return $this or skip this processor by returning the next processor.
   */
  protected function getPreparedValue() {
    return isset($this->setting) && array_filter($this->setting) ? $this : $this->processor;
  }

  /**
   * Returns whether the current user has permission to edit this chain of data
   * processors.
   */
  public function editAccess() {
    return $this->access() && (!isset($this->processor) || $this->processor->editAccess());
  }


  /**
   * Prepares the processor for parameters.
   *
   * It turns the settings into a suiting processor object, which gets invoked
   * on evaluation time.
   *
   * @param $setting
   *   The processor settings which are to be prepared.
   * @param $param_info
   *   The info about the parameter to prepare the processor for.
   * @param $var_info
   *   An array of info about the available variables.
   */
  public static function prepareSetting(&$setting, $param_info, $var_info = array()) {
    $processor = NULL;
    foreach (self::processors($param_info, FALSE) as $name => $info) {
      if (!empty($setting[$name])) {
        $object = new $info['class']($setting[$name], $param_info, $var_info, $processor);
        $processor = $object->getPreparedValue();
      }
    }
    $setting = $processor;
  }

  /**
   * Attaches the form of applicable data processors.
   */
  public static function attachForm(&$form, $settings, $param_info, $var_info, $access_check = TRUE) {
    // If $settings is already prepared get the settings from the processors.
    if ($settings instanceof RulesDataProcessor) {
      $settings = $settings->getChainSettings();
    }
    foreach (self::processors($param_info, $access_check) as $name => $info) {
      $settings += array($name => array());
      $form[$name] = call_user_func(array($info['class'], 'form'), $settings[$name], $var_info);
      $form[$name]['#weight'] = $info['weight'];
    }
  }

  /**
   * Returns defined data processors applicable for the given parameter.
   * Optionally also access to the processors is checked.
   *
   * @param $param_info
   *   If given, only processors valid for this parameter are returned.
   */
  public static function processors($param_info = NULL, $access_check = TRUE, $hook = 'data_processor_info') {
    static $items = array();

    if (!isset($items[$hook]['all'])) {
      $items[$hook]['all'] = rules_fetch_data($hook);
      uasort($items[$hook]['all'], array(__CLASS__, '_item_sort'));
    }
    // Data processing isn't supported for multiple types.
    if (isset($param_info) && is_array($param_info['type'])) {
      return array();
    }
    // Filter the items by type.
    if (isset($param_info['type']) && !isset($items[$hook][$param_info['type']])) {
      $items[$hook][$param_info['type']] = array();
      foreach ($items[$hook]['all'] as $name => $info) {
        // Check whether the parameter type matches the supported types.
        $info += array('type' => 'text');
        if (RulesData::typesMatch($param_info, $info, FALSE)) {
          $items[$hook][$param_info['type']][$name] = $info;
        }
      }
    }
    // Apply the access check.
    $return = isset($param_info['type']) ? $items[$hook][$param_info['type']] : $items[$hook]['all'];
    if ($access_check) {
      foreach ($return as $base => $info) {
        if (!call_user_func(array($info['class'], 'access'))) {
          unset($return[$base]);
        }
      }
    }
    return $return;
  }

  public static function _item_sort($a, $b) {
    return $a['weight'] < $b['weight'] ? -1 : ($a['weight'] > $b['weight'] ? 1 : 0);
  }

  /**
   * Gets the settings array for this and all contained chained processors.
   */
  public function getChainSettings() {
    foreach ($this->unchain() as $name => $processor) {
      $settings[$name] = $processor->getSetting();
    }
    return isset($settings) ? $settings : array();
  }

  /**
   * Returns an array of modules which we depend on.
   */
  public function dependencies() {
    $used_processor_info = array_intersect_key($this->processors(), $this->unchain());
    $modules = array();
    foreach ($used_processor_info as $name => $info) {
      $modules[] = $info['module'];
    }
    return array_filter($modules);
  }

  /**
   * @return
   *   An array of processors keyed by processor name.
   */
  protected function unchain() {
    $processor = $this;
    while ($processor instanceof RulesDataProcessor) {
      $processors[get_class($processor)] = $processor;
      $processor = $processor->processor;
    }
    // Note: Don't use the static context to call processors() here as we need a
    // late binding to invoke the input evaluators version, if needed.
    $return = array();
    foreach ($this->processors() as $name => $info) {
      if (isset($processors[$info['class']])) {
        $return[$name] = $processors[$info['class']];
      }
    }
    return $return;
  }

  /**
   * Gets the settings of this processor.
   */
  public function getSetting() {
    return $this->setting;
  }

  /**
   * Processes the value. If $this->processor is set, invoke this processor
   * first so chaining multiple processors is working.
   *
   * @param $value
   *   The value to process.
   * @param $info
   *   Info about the parameter for which we process the value.
   * @param $state RulesState
   *   The rules evaluation state.
   * @param $element RulesPlugin
   *   The element for which we process the value.
   * @return
   *   The processed value.
   */
  abstract public function process($value, $info, RulesState $state, RulesPlugin $element);

  /**
   * Return whether the current user has permission to use the processor.
   */
  public static function access() {
    return TRUE;
  }

  /**
   * Defines the processor form element.
   *
   * @param $settings
   *   The settings of the processor.
   * @param $var_info
   *   An array of info about the available variables.
   *
   * @return
   *   A form element structure.
   */
  protected static function form($settings, $var_info) {
    return array();
  }
}


/**
 * A base processor for use as input evaluators. Input evaluators are not listed
 * in hook_rules_data_processor_info(). Instead they use
 * hook_rules_evaluator_info() and get attached to input forms.
 */
abstract class RulesDataInputEvaluator extends RulesDataProcessor {

  /**
   * Overridden to invoke prepare().
   */
  protected function __construct($setting, $param_info, $var_info = array(), $processor = NULL) {
    $this->setting = TRUE;
    $this->processor = $processor;
    $this->prepare($setting, $var_info);
  }

  /**
   * Overridden to generate evaluator $options and invoke evaluate().
   */
  public function process($value, $info, RulesState $state, RulesPlugin $element, $options = NULL) {
    if (!isset($options)) {
      $cache = rules_get_cache();
      $languages = language_list();
      $options = array_filter(array(
        'language' => isset($element->settings['language']) && isset($languages[$element->settings['language']]) ? $languages[$element->settings['language']] : NULL,
        'callback' => isset($cache['data info'][$info['type']]['cleaning callback']) ? $cache['data info'][$info['type']]['cleaning callback'] : FALSE,
        'sanitize' => !empty($info['sanitize']),
      ));
    }
    $value = isset($this->processor) ? $this->processor->process($value, $info, $state, $element, $options) : $value;
    return $this->evaluate($value, $options, $state);
  }

  /**
   * Overriden to prepare input evaluator processors. The setting is expected
   * to be the input value to be evaluated later on and is replaced by the
   * suiting processor.
   */
  public static function prepareSetting(&$setting, $param_info, $var_info = array()) {
    $processor = NULL;
    foreach (self::evaluators($param_info, FALSE) as $name => $info) {
      $object = new $info['class']($setting, $param_info, $var_info, $processor);
      $processor = $object->getPreparedValue();
    }
    $setting = $processor;
  }

  protected function getPreparedValue() {
    return isset($this->setting) ? $this : $this->processor;
  }

  /**
   * Overriden to just attach the help() of evaluators.
   */
  public static function attachForm(&$form, $settings, $param_info, $var_info, $access_check = TRUE) {
    foreach (self::evaluators($param_info, $access_check) as $name => $info) {
      $form['help'][$name] = call_user_func(array($info['class'], 'help'), $var_info);
      $form['help'][$name]['#weight'] = $info['weight'];
    }
  }

  /**
   * Returns all input evaluators that can be applied to the parameters needed
   * type.
   */
  public static function evaluators($param_info = NULL, $access_check = TRUE) {
    return parent::processors($param_info, $access_check, 'evaluator_info');
  }

  /**
   * Overridden to default to our hook, thus being equivalent to
   * self::evaluators().
   */
  public static function processors($param_info = NULL, $access_check = TRUE, $hook = 'evaluator_info') {
    return parent::processors($param_info, $access_check, $hook);
  }

  /**
   * Prepares the evalution, e.g. to determine whether the input evaluator has
   * been used. If this evaluator should be skipped just unset $this->setting.
   *
   * @param $text
   *   The text to evaluate later on.
   * @param $variables
   *   An array of info about available variables.
   */
  abstract public function prepare($text, $variables);

  /**
   * Apply the input evaluator.
   *
   * @param $text
   *   The text to evaluate.
   * @param $options
   *   A keyed array of settings and flags to control the processing.
   *   Supported options are:
   *   - language: A language object to be used when processing.
   *   - callback: A callback function that will be used to post-process
   *     replacements that might be incorporated, so they can be cleaned in a
   *     certain way.
   *   - sanitize: A boolean flag indicating whether incorporated replacements
   *     should be sanitized.
   * @param RulesState
   *   The rules evaluation state.
   * @return
   *   The evaluated text.
   */
  abstract public function evaluate($text, $options, RulesState $state);

  /**
   * Provide some usage help for the evaluator.
   *
   * @param $variables
   *   An array of info about available variables.
   * @return
   *   A renderable array.
   */
  public static function help($variables) {
    return array();
  }

}
