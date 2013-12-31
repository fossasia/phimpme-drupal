<?php

/**
 * @file Contains data type related forms.
 */


/**
 * Interface for data types providing a direct input form.
 */
interface RulesDataDirectInputFormInterface {

  /**
   * Constructs the direct input form.
   *
   * @return Array
   *  The direct input form.
   */
  public static function inputForm($name, $info, $settings, RulesPlugin $element);

  /**
   * Render the configured value.
   *
   * @return Array
   *   A renderable array.
   */
  public static function render($value);

}

/**
 * Default UI related class for data types.
 */
class RulesDataUI {

  /**
   * Specifies the default input mode per data type.
   */
  public static function getDefaultMode() {
    return 'selector';
  }

  /**
   * Provides the selection form for a parameter.
   */
  public static function selectionForm($name, $info, $settings, RulesPlugin $element) {
    if (!isset($settings[$name . ':select'])) {
      $settings[$name . ':select'] = '';
      $vars = $element->availableVariables();
      // Default to variables with the same name as the parameter.
      if (isset($vars[$name])) {
        $settings[$name . ':select'] = $name;
      }
      // If there is only one match, use it by default.
      elseif (count($matches = RulesData::matchingDataSelector($vars, $info, '', 1, FALSE)) == 1) {
        $settings[$name . ':select'] = rules_array_key($matches);
      }
    }
    $form[$name . ':select'] = array(
      '#type' => 'rules_data_selection',
      '#title' => t('Data selector'),
      '#default_value' => $settings[$name . ':select'],
      '#required' => empty($info['optional']),
      '#autocomplete_path' => RulesPluginUI::path($element->root()->name, 'autocomplete' . '/' . $name),
      // Make the autocomplete textfield big enough so that it can display
      // descriptions without word wraps.
      '#size' => 75,
      '#description' => t("The data selector helps you drill down into the data available to Rules. <em>To make entity fields appear in the data selector, you may have to use the condition 'entity has field' (or 'content is of type').</em> More useful tips about data selection is available in <a href='@url'>the online documentation</a>.",
        array('@url' => rules_external_help('data-selection'))),
    );
    $form['help'] = array(
      '#theme' => 'rules_data_selector_help',
      '#variables' => $element->availableVariables(),
      '#parameter' => $info,
    );

    // Add data processor.
    $settings += array($name . ':process' => array());
    $form[$name . ':process'] = array();
    RulesDataProcessor::attachForm($form[$name . ':process'], $settings[$name . ':process'], $info, $element->availableVariables());
    return $form;
  }

  /**
   * Renders the value by making use of the label if an options list is available.
   *
   * Used for data UI classes implementing the
   * RulesDataDirectInputFormInterface.
   *
   * In case an options list is available, the the usual render() method won't
   * be invoked, instead the selected entry is rendered via this method.
   *
   * @todo for Drupal 8: Refactor to avoid implementations have to care about
   * option lists when generating the form, but not when rendering values.
   */
  public static function renderOptionsLabel($value, $name, $info, RulesPlugin $element) {
    if (!empty($info['options list'])) {
      $element->call('loadBasicInclude');
      $options = entity_property_options_flatten($info['options list']($element, $name));
      if (!is_array($value) && isset($options[$value])) {
        $value = $options[$value];
      }
      elseif (is_array($value)) {
        foreach ($value as $key => $single_value) {
          if (isset($options[$single_value])) {
            $value[$key] = $options[$single_value];
          }
        }
        $value = implode(', ', $value);
      }
      return array(
        'content' => array('#markup' => check_plain($value)),
        '#attributes' => array('class' => array('rules-parameter-options-entry')),
      );
    }
  }
}

/**
 * UI for textual data.
 */
class RulesDataUIText extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    if (!empty($info['options list'])) {
      // Make sure the .rules.inc of the providing module is included as the
      // options list callback may reside there.
      $element->call('loadBasicInclude');
      $form[$name] = array(
        '#type' => 'select',
        '#options' => call_user_func($info['options list'], $element, $name),
      );
    }
    else {
      $form[$name] = array(
        '#type' => 'textarea',
      );
      RulesDataInputEvaluator::attachForm($form, $settings, $info, $element->availableVariables());
    }
    $settings += array($name => isset($info['default value']) ? $info['default value'] : NULL);
    $form[$name] += array(
      '#title' => t('Value'),
      '#default_value' => $settings[$name],
      '#required' => empty($info['optional']),
      '#after_build' => array('rules_ui_element_fix_empty_after_build'),
      '#rows' => 3,
    );
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain($value)),
      '#attributes' => array('class' => array('rules-parameter-text')),
    );
  }
}

/**
 * UI for text tokens.
 */
class RulesDataUITextToken extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#element_validate'][] = 'rules_ui_element_token_validate';
    $form[$name]['#description'] = t('May only contain lowercase letters, numbers, and underscores and has to start with a letter.');
    $form[$name]['#rows'] = 1;
    return $form;
  }
}

/**
 * UI for formatted text.
 */
class RulesDataUITextFormatted extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $settings += array($name => isset($info['default value']) ? $info['default value'] : array('value' => NULL, 'format' => NULL));

    $form[$name]['#type'] = 'text_format';
    $form[$name]['#base_type'] = 'textarea';
    $form[$name]['#default_value'] = $settings[$name]['value'];
    $form[$name]['#format'] = $settings[$name]['format'];
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain($value['value'])),
      '#attributes' => array('class' => array('rules-parameter-text-formatted')),
    );
  }
}



/**
 * UI for decimal data.
 */
class RulesDataUIDecimal extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';
    }
    $form[$name]['#element_validate'][] = 'rules_ui_element_decimal_validate';
    $form[$name]['#rows'] = 1;
    return $form;
  }
}

/**
 * UI for integers.
 */
class RulesDataUIInteger extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';
    }
    $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
    return $form;
  }
}

/**
 * UI for boolean data.
 */
class RulesDataUIBoolean extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : NULL);
    // Note: Due to the checkbox even optional parameter always receive a value.
    $form[$name] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($info['label']),
      '#default_value' => $settings[$name],
    );
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => !empty($value) ? t('true') : t('false')),
      '#attributes' => array('class' => array('rules-parameter-boolean')),
    );
  }
}

/**
 * UI for dates.
 */
class RulesDataUIDate extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : (empty($info['optional']) ? gmdate('Y-m-d H:i:s', time()) : NULL));

    // Convert any configured timestamp into a readable format.
    if (is_numeric($settings[$name])) {
      $settings[$name] = gmdate('Y-m-d H:i:s', $settings[$name]);
    }
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#element_validate'][] = 'rules_ui_element_date_validate';
    // Note that the date input evaluator takes care for parsing dates using
    // strtotime() into a timestamp, which is the internal date format.
    $form[$name]['#description'] = t('The date in GMT. You may enter a fixed time (like %format) or any other values in GMT known by the PHP !strtotime function (like "+1 day"). Relative dates like "+1 day" or "now" relate to the evaluation time.',
      array('%format' => gmdate('Y-m-d H:i:s', time() + 86400),
            '!strtotime' => l('strtotime()', 'http://php.net/strtotime')));

    //TODO: Leverage the jquery datepicker+timepicker once a module providing
    //the timpeicker is available.
    return $form;
  }

  public static function render($value) {
    $value = is_numeric($value) ? format_date($value, 'short') : check_plain($value);
    return array(
      'content' => array('#markup' => $value),
      '#attributes' => array('class' => array('rules-parameter-date')),
    );
  }
}

/**
 * UI for duration type parameter.
 */
class RulesDataUIDuration extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'rules_duration';
    $form[$name]['#after_build'][] = 'rules_ui_element_duration_after_build';
    return $form;
  }

  public static function render($value) {
    $value = is_numeric($value) ? format_interval($value) : check_plain($value);
    return array(
      'content' => array('#markup' => $value),
      '#attributes' => array('class' => array('rules-parameter-duration')),
    );
  }
}

/**
 * UI for the URI type parameter.
 */
class RulesDataUIURI extends RulesDataUIText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#rows'] = 1;
    $form[$name]['#description'] = t('You may enter relative URLs like %url as well as absolute URLs like %absolute-url.', array('%url' => 'user/login?destination=node', '%absolute-url' => 'http://drupal.org'));
    return $form;
  }
}

/**
 * UI for lists of textual data.
 */
class RulesDataUIListText extends RulesDataUIText {

  public static function getDefaultMode() {
    return 'input';
  }

  /**
   * @todo This does not work for inputting textual values including "\n".
   */
  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : NULL);
    $form = parent::inputForm($name, $info, $settings, $element);

    if ($form[$name]['#type'] == 'textarea') {
      // Fix up the value to be an array during after build.
      $form[$name]['#delimiter'] = "\n";
      $form[$name]['#after_build'][] = 'rules_ui_list_textarea_after_build';
      $form[$name]['#pre_render'][] = 'rules_ui_list_textarea_pre_render';
      $form[$name]['#default_value'] = !empty($settings[$name]) ? implode("\n", $settings[$name]) : NULL;
      $form[$name]['#description'] = t('A list of values, one on each line.');
    }
    else {
      $form[$name]['#multiple'] = TRUE;
    }
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => check_plain(implode(', ', $value))),
      '#attributes' => array('class' => array('rules-parameter-list')),
    );
  }
}

/**
 * UI for lists of integers.
 */
class RulesDataUIListInteger extends RulesDataUIListText {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : NULL);
    $form = parent::inputForm($name, $info, $settings, $element);

    if ($form[$name]['#type'] == 'textarea') {
      $form[$name]['#description'] = t('A list of integers, separated by commas. E.g. enter "1, 2, 3".');
      $form[$name]['#delimiter'] = ',';
      $form[$name]['#default_value'] = !empty($settings[$name]) ? implode(", ", $settings[$name]) : NULL;
      $form[$name]['#element_validate'][] = 'rules_ui_element_integer_list_validate';
      $form[$name]['#rows'] = 1;
    }
    return $form;
  }
}

/**
 * UI for lists of tokens.
 */
class RulesDataUIListToken extends RulesDataUIListInteger {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);

    if ($form[$name]['#type'] == 'textarea') {
      $form[$name]['#description'] = t('A list of text tokens, separated by commas. E.g. enter "one, two, three".');
      $form[$name]['#element_validate'] = array('rules_ui_element_token_list_validate');
    }
    return $form;
  }
}

/**
 * UI for entity-based data types.
 */
class RulesDataUIEntity extends RulesDataUIText {

  public static function getDefaultMode() {
    return 'selector';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {
      $form[$name]['#type'] = 'textfield';

      $entity_info = entity_get_info($info['type']);
      if (empty($entity_info['entity keys']['name'])) {
        $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
      }
      $form[$name]['#title'] = t('@entity identifier', array('@entity' => $entity_info['label']));
      $entity_label = strtolower($entity_info['label'][0]) . substr($entity_info['label'], 1);
      $form[$name]['#description'] = t('Specify an identfier of a @entity.', array('@entity' => $entity_label));
    }
    return $form;
  }
}

/**
 * UI for exportable entity-based data types.
 */
class RulesDataUIEntityExportable extends RulesDataUIEntity {

  public static function getDefaultMode() {
    return 'input';
  }
}

/**
 * UI for taxonomy vocabularies.
 *
 * @see RulesTaxonomyVocabularyWrapper
 */
class RulesDataUITaxonomyVocabulary extends RulesDataUIEntity {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    // Add an options list of all vocabularies if there is none yet.
    if (!isset($info['options list'])) {
      $info['options list'] = array('RulesDataUITaxonomyVocabulary', 'optionsList');
    }
    return parent::inputForm($name, $info, $settings, $element);
  }

  public static function optionsList() {
    $options = array();
    foreach (taxonomy_vocabulary_get_names() as $machine_name => $vocab) {
      $options[$machine_name] = $vocab->name;
    }
    return $options;
  }
}

/**
 * UI for lists of entity-based data types.
 */
class RulesDataUIListEntity extends RulesDataUIListInteger {

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    if (empty($info['options list'])) {

      $entity_info = entity_get_info(entity_property_list_extract_type($info['type']));
      if (!empty($entity_info['entity keys']['name'])) {
        $form[$name]['#element_validate'] = array('rules_ui_element_token_list_validate');
      }
      $form[$name]['#title'] = t('@entity identifiers', array('@entity' => $entity_info['label']));
      $entity_label = strtolower($entity_info['label'][0]) . substr($entity_info['label'], 1);
      $form[$name]['#description'] = t('Specify a comma-separated list of identfiers of @entity entities.', array('@entity' => $entity_label));
    }
    return $form;
  }
}

