<?php

/**
 * @file
 * Internationalization rules integration.
 */

/**
 * Implements hook_rules_action_info().
 */
function rules_i18n_rules_action_info() {
  $items['rules_i18n_t'] = array(
    'label' => t('Translate a text'),
    'group' => t('Translation'),
    'parameter' => array(
      'text' => array(
        'type' => 'text',
        'label' => t('Text'),
        'description' => t('The text to translate.'),
        'translatable' => TRUE,
      ),
      'language' => array(
        'type' => 'token',
        'label' => t('Language'),
        'description' => t('The language to translate the text into.'),
        'options list' => 'entity_metadata_language_list',
        'default mode' => 'select',
      ),
    ),
    'provides' => array(
      'text' => array('type' => 'text', 'label' => t('The translated text')),
    ),
    'base' => 'rules_i18n_action_t',
    'access callback' => 'rules_i18n_rules_integration_access',
  );
  $items['rules_i18n_select'] = array(
    'label' => t('Select a translated value'),
    'group' => t('Translation'),
    'parameter' => array(
      'data' => array(
        'type' => '*',
        'label' => t('Data'),
        'description' => t('Select a translated value, e.g. a translatable field. If the selected data is not translatable, the language neutral value will be selected.'),
        'translatable' => TRUE,
        'restrict' => 'select',
      ),
      'language' => array(
        'type' => 'token',
        'label' => t('Language'),
        'description' => t('The language to translate the value into.'),
        'options list' => 'entity_metadata_language_list',
      ),
    ),
    'provides' => array(
      'data_translated' => array('type' => '*', 'label' => t('The translated value')),
    ),
    'base' => 'rules_i18n_action_select',
    'access callback' => 'rules_i18n_rules_integration_access',
  );
  return $items;
}

/**
 * Access callback for the rules i18n integration.
 */
function rules_i18n_rules_integration_access() {
  return user_access('translate interface');
}

/**
 * Action callback: Translate a text.
 */
function rules_i18n_action_t($text) {
  // Nothing to do, as our input evaluator has already translated it.
  // @see RulesI18nStringEvaluator
  return array('text' => $text);
}

/**
 * Implements the form_alter callback for the "Translate a text" action to set a default selector.
 */
function rules_i18n_action_t_form_alter(&$form, &$form_state, $options, $element) {
  if (isset($form['parameter']['language']['settings']['language:select']) && empty($element->settings['language:select'])) {
    $form['parameter']['language']['settings']['language:select']['#default_value'] = 'site:current-page:language';
  }
}

/**
 * Action callback: Select a translated value.
 */
function rules_i18n_action_select($data) {
  // Nothing to do, as Rules applies the language to the data selector for us.
  return array('data_translated' => $data);
}

/**
 * Action "Select a translated value" info_alter callback.
 */
function rules_i18n_action_select_info_alter(&$element_info, $element) {
  $element->settings += array('data:select' => NULL);
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    $info = $wrapper->info();
    // Pass through the data type of the selected data.
    $element_info['provides']['data_translated']['type'] = $wrapper->type();
  }
}

/**
 * Implements hook_rules_evaluator_info().
 */
function rules_i18n_rules_evaluator_info() {
  return array(
    'i18n' => array(
      'class' => 'RulesI18nStringEvaluator',
      'type' => array('text', 'list<text>', 'token', 'list<token>'),
      // Be sure to translate after doing PHP evaluation.
      'weight' => -8,
     ),
  );
}

/**
 * A class implementing a rules input evaluator processing tokens.
 */
class RulesI18nStringEvaluator extends RulesDataInputEvaluator {

  public static function access() {
    return user_access('translate interface');
  }

  public function prepare($text, $var_info, $param_info = NULL) {
    if (!empty($param_info['translatable'])) {
      $this->setting = TRUE;
    }
    else {
      // Else, skip this evaluator.
      $this->setting = NULL;
    }
  }

  /**
   * Prepare the i18n-context string.
   *
   * We have to use process() here instead of evaluate() because we need more
   * context than evaluate() provides.
   */
  public function process($value, $info, RulesState $state, RulesPlugin $element, $options = NULL) {
    $options = isset($options) ? $options : $this->getEvaluatorOptions($info, $state, $element);
    $value = isset($this->processor) ? $this->processor->process($value, $info, $state, $element, $options) : $value;
    if (isset($element->root()->name)) {
      $config_name = $element->root()->name;
      $id = $element->elementId();
      $name = $info['#name'];
      $options['i18n context'] = "rules:rules_config:$config_name:$id:$name";
      return $this->evaluate($value, $options, $state);
    }
    return $value;
  }

  /**
   * Translate the value.
   *
   * If the element provides a language parameter, we are using this target
   * language provided via $options['language'].
   */
  public function evaluate($value, $options, RulesState $state) {
    $langcode = isset($options['language']) ? $options['language']->language : NULL;
    if (is_array($value)) {
      foreach ($value as $key => $text) {
        $value[$key] = entity_i18n_string($options['i18n context'] . ':' . $key, $text, $langcode);
      }
    }
    else {
      $value = entity_i18n_string($options['i18n context'], $value, $langcode);
    }
    return $value;
  }

  public static function help($var_info, $param_info = array()) {
    if (!empty($param_info['translatable'])) {
      if ($param_info['custom translation language']) {
        $text = t('Translations can be provided at the %translate tab. The argument value is translated to the configured language.', array('%translate' => t('Translate')));
      }
      else {
        $text = t('Translations can be provided at the %translate tab. The argument value is translated to the current interface language.', array('%translate' => t('Translate')));
      }
      $render = array(
        '#theme' => 'rules_settings_help',
        '#text' => $text,
        '#heading' => t('Translation'),
      );
      return $render;
    }
  }
}
