<?php

/**
 * @file
 * Contains rules core integration needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action and condition callback: Invokes a rules component.
 *
 * We do not use the execute() method, but handle executing ourself. That way
 * we can utilize the existing state for saving passed variables.
 */
function rules_element_invoke_component($arguments, RulesPlugin $element) {
  $info = $element->info();
  $state = $arguments['state'];
  $wrapped_args = $state->currentArguments;

  if ($component = rules_get_cache('comp_' . $info['#config_name'])) {
    $replacements = array('%label' => $component->label(), '@plugin' => $component->plugin());
    // Handle recursion prevention.
    if ($state->isBlocked($component)) {
      return rules_log('Not evaluating @plugin %label to prevent recursion.', $replacements, RulesLog::INFO, $component);
    }
    $state->block($component);
    rules_log('Evaluating @plugin %label.', $replacements, RulesLog::INFO, $component, TRUE);
    module_invoke_all('rules_config_execute', $component);

    // Manually create a new evaluation state and evaluate the component.
    $args = array_intersect_key($wrapped_args, $component->parameterInfo());
    $new_state = $component->setUpState($wrapped_args);
    $return = $component->evaluate($new_state);

    // Care for the right return value in case we have to provide vars.
    if ($component instanceof RulesActionInterface && !empty($info['provides'])) {
      $return = array();
      foreach ($info['provides'] as $var => $var_info) {
        $return[$var] = $new_state->get($var);
      }
    }

    // Now merge the info about to be saved variables in the parent state.
    $state->mergeSaveVariables($new_state, $component, $element->settings);
    $state->unblock($component);

    // Cleanup the state, what saves not mergable variables now.
    $new_state->cleanup();
    rules_log('Finished evaluation of @plugin %label.', $replacements, RulesLog::INFO, $component, FALSE);
    return $return;
  }
  else {
    throw new RulesEvaluationException('Unable to get the component %name', array('%name' => $info['#config_name']), $element, RulesLog::ERROR);
  }
}

/**
 * A class implementing a rules input evaluator processing date input. This is
 * needed to treat relative date inputs for strtotime right, consider "now".
 */
class RulesDateInputEvaluator extends RulesDataInputEvaluator {

  const DATE_REGEX_LOOSE = '/^(\d{4})-?(\d{2})-?(\d{2})([T\s]?(\d{2}):?(\d{2}):?(\d{2})?)?$/';

  public function prepare($text, $var_info) {
    if (is_numeric($text)) {
      // Let rules skip this input evaluators in case it's already a timestamp.
      $this->setting = NULL;
    }
  }

  public function evaluate($text, $options, RulesState $state) {
    return self::gmstrtotime($text);
  }

  /**
   * Convert a time string to a GMT (UTC) unix timestamp.
   */
  public static function gmstrtotime($date) {
    // Pass the current timestamp in UTC to ensure the retrieved time is UTC.
    return strtotime($date, time());
  }

  /**
   * Determine whether the given date string specifies a fixed date.
   */
  public static function isFixedDateString($date) {
    return is_string($date) && preg_match(self::DATE_REGEX_LOOSE, $date);
  }
}

/**
 * A class implementing a rules input evaluator processing URI inputs to make
 * sure URIs are absolute and path aliases get applied.
 */
class RulesURIInputEvaluator extends RulesDataInputEvaluator {

  public function prepare($uri, $var_info) {
    if (!isset($this->processor) && valid_url($uri, TRUE)) {
      // Only process if another evaluator is used or the url is not absolute.
      $this->setting = NULL;
    }
  }

  public function evaluate($uri, $options, RulesState $state) {
    if (!url_is_external($uri)) {
      // Extract the path and build the URL using the url() function, so URL
      // aliases are applied and query parameters and fragments get handled.
      $url = drupal_parse_url($uri);
      $url_options = array('absolute' => TRUE);
      $url_options['query'] = $url['query'];
      $url_options['fragment'] = $url['fragment'];
      return url($url['path'], $url_options);
    }
    elseif (valid_url($uri)) {
      return $uri;
    }
    throw new RulesEvaluationException('Input evaluation generated an invalid URI.', array(), NULL, RulesLog::WARN);
  }
}

/**
 * A data processor for applying date offsets.
 */
class RulesDateOffsetProcessor extends RulesDataProcessor {

  protected static function form($settings, $var_info) {
    $settings += array('value' => '');
    $form = array(
      '#type' => 'fieldset',
      '#title' => t('Add offset'),
      '#collapsible' => TRUE,
      '#collapsed' => empty($settings['value']),
      '#description' => t('Add an offset to the selected date.'),
    );
    $form['value'] = array(
      '#type' => 'rules_duration',
      '#title' => t('Offset'),
      '#description' => t('Note that you can also specify negative numbers.'),
      '#default_value' => $settings['value'],
      '#weight' => 5,
    );
    return $form;
  }

  public function process($value, $info, RulesState $state, RulesPlugin $element) {
    $value = isset($this->processor) ? $this->processor->process($value, $info, $state, $element) : $value;
    return $value + $this->setting['value'];
  }
}

/**
 * A data processor for applying numerical offsets.
 */
class RulesNumericOffsetProcessor extends RulesDataProcessor {

  protected static function form($settings, $var_info) {
    $settings += array('value' => '');
    $form = array(
      '#type' => 'fieldset',
      '#title' => t('Add offset'),
      '#collapsible' => TRUE,
      '#collapsed' => empty($settings['value']),
      '#description' => t('Add an offset to the selected number. E.g. an offset of "1" adds 1 to the number before it is passed on as argument.'),
    );
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Offset'),
      '#description' => t('Note that you can also specify negative numbers.'),
      '#default_value' => $settings['value'],
      '#element_validate' => array('rules_ui_element_integer_validate'),
      '#weight' => 5,
    );
    return $form;
  }

  public function process($value, $info, RulesState $state, RulesPlugin $element) {
    $value = isset($this->processor) ? $this->processor->process($value, $info, $state, $element) : $value;
    return $value + $this->setting['value'];
  }
}


/**
 * A custom wrapper class for vocabularies that is capable of loading vocabularies by machine name.
 */
class RulesTaxonomyVocabularyWrapper extends EntityDrupalWrapper {

  /**
   * Overridden to support identifying vocabularies by machine names.
   */
  protected function setEntity($data) {
    if (isset($data) && $data !== FALSE && !is_object($data) && !is_numeric($data)) {
      // The vocabulary name has been passed.
      parent::setEntity(taxonomy_vocabulary_machine_name_load($data));
    }
    else {
      parent::setEntity($data);
    }
  }

  /**
   * Overriden to permit machine names as values.
   */
  public function validate($value) {
    if (isset($value) && is_string($value)) {
      return TRUE;
    }
    return parent::validate($value);
  }
}
