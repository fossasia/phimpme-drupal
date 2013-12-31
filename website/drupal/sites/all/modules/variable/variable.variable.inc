<?php 
/**
 * @file
 * Variable module hook implementations
 */

/**
 * Implements hook_variable_group_info().
 */
function variable_variable_group_info() {
  // Group for variable that have no group
  $groups['default'] = array(
    'title' => t('Other'),
    'description' => t("Variables that don't belong to any other group."),
  );
  $groups['debug'] = array(
    'title' => t('Debug'),
    'description' => t('Debug and development options.'),
  );
  return $groups;
}

/**
 * Implements hook_variable_type_info().
 */
function variable_variable_type_info() {
  // Array of values
  $type['array'] = array(
    'title' => t('Array'),
    'element' => array('#type' => 'fieldset', '#tree' => TRUE),
    // Properties for each array item
    'repeat' => array(
      'element' => array('#type' => 'textfield'),
    ),
    'format callback' => 'variable_format_array',
    'element callback' => 'variable_form_element_array',
    'default' => array(),
  );
  // TRUE / FALSE value, checkbox
  $type['boolean'] = array(
    'title' => t('Boolean'),
    'element' => array('#type' => 'checkbox'),
    'format callback' => 'variable_format_boolean',
  );
  // Default type for variables with no other type
  $type['default'] = array(
    'title' => t('Default'),
    'element' => array('#type' => 'textfield'),
    'access' => 'administer site configuration',
  );
  // Enable/Disable 
  $type['enable'] = array(
    'title' => t('Enable'),
    'options' => array(t('Disabled'), t('Enabled')),
    'default' => 0,
    'element' => array('#type' => 'radios'),
    'format callback' => 'variable_format_selection',
  );
  // Multiple variable that will spawn into multiple elements
  $type['multiple'] = array(
    'title' => t('Multiple'),
    'element' => array('#type' => 'fieldset'),
    'build callback' => 'variable_build_multiple',
    'format callback' => 'variable_format_multiple',
    'element callback' => 'variable_form_element_multiple',
    'value callback' => 'variable_multiple_get_value',
    'default callback' => 'variable_multiple_get_default',
  );
  $type['mail_address'] = array(
    'title' => t('E-mail address'),
    'element' => array('#type' => 'textfield'),
    'token' => TRUE,
  );
  $type['mail_text'] = array(
    'title' => t('Mail text'),
    'multiple' => array('subject' => t('Subject'), 'body' => t('Body')),
    'build callback' => 'variable_build_mail_text',
    'localize' => TRUE,
    'type' => 'multiple',
  );
  $type['number'] = array(
    'title' => t('Number'),
    'element' => array('#type' => 'textfield', '#size' => 15, '#maxlength' => 10),
  );
  // Select multiple options from multiple choices
  $type['options'] = array(
    'title' => t('Options'),
    'options' => TRUE,
    'element' => array('#type' => 'checkboxes'),
    'element callback' => 'variable_form_element_options',
    'format callback' => 'variable_format_options',
  );
  // Select single option from multiple choices
  $type['select'] = array(
    'title' => t('Select'),
    'options' => TRUE,
    // This will become radios or drop-down depending on the number of options
    'element callback' => 'variable_form_element_options',
    'format callback' => 'variable_format_selection',
  );
  // Select number from array of values. Options array that can be list of numbers will be converted to a value => value
  $type['select_number'] = array(
    'title' => t('Select'),
    'options' => TRUE,
    'element callback' => 'variable_form_element_options',
    'options callback' => 'variable_options_select_number',
  );
  $type['string'] = array(
    'title' => t('String'),
    'element' => array('#type' => 'textfield'),
    'localize' => TRUE,
    'token' => TRUE,
  );
  $type['text'] = array(
    'title' => t('Text'),
    'element' => array('#type' => 'textarea'),
    'localize' => TRUE,
    'format callback' => 'variable_format_text',
    'token' => TRUE,
  );
  // Default type for variables with no other type
  $type['unknown'] = array(
    'title' => t('Unknown'),
    'access' => 'administer site configuration',
    'format' => 'variable_format_unknown',
    'element callback' => 'variable_form_element_unknown',
    'element' => array('#type' => 'item'),
  );
  $type['url'] = array(
    'title' => t('URL'),
    'element' => array('#type' => 'textfield', '#size' => 30, '#maxlength' => 255),
  );
  $type['mail_part'] = array(
    'title' => t('Mail parts'),
    'options' => array('subject' => t('Subject'), 'body' => t('Body')),
  );
  return $type;
}

/**
 * Build multiple mail variable
 */
function variable_build_mail_text($variable, $options = array()) {
  $name = str_replace('[mail_part]', '', $variable['name']);
  // For mail text, children have different types
  $variable['children'][$name . 'subject']['type'] = 'string';
  $variable['children'][$name . 'body']['type'] = 'text';
  $variable = variable_build_multiple($variable, $options);
  return $variable;
}

/**
 * Format select variable
 */
function variable_format_selection($variable, $options = array()) {
  $variable = variable_build_options($variable, $options);
  if (isset($variable['value'])) {
    return isset($variable['options'][$variable['value']]) ? $variable['options'][$variable['value']] : t('<Invalid option>');
  }
  else {
    return variable_format_empty($variable);
  }
}

/**
 * Format options variable. Value is an array of options.
 */
function variable_format_options($variable, $options = array()) {
  $variable = variable_build_options($variable, $options);
  $names = array();
  if (isset($variable['value']) && $variable['value']) {
    if (is_array($variable['value'])) {
      foreach ($variable['value'] as $index => $value) {
         $names[$index] = isset($variable['options'][$value]) ? $variable['options'][$value] : t('<Invalid option>');
      }
      return implode(', ', $names);
    }
    else {
      return t('<Invalid value>');
    }
  }
  else {
    return variable_format_empty($variable);
  }
}

/**
 * Format array variable, handling nested arrays
 */
function variable_format_array($variable = NULL, $options = array()) {
  if (empty($variable['value'])) {
    return variable_format_empty($variable);
  }
  else {
    $elements = array();
    foreach ($variable['value'] as $item) {
      $elements[] = is_array($item) ? '(' . variable_format_array(array('value' => $item), $options) . ')' : check_plain($item);
    }
    return implode(', ', $elements);
  }
}

/**
 * Format boolean variable
 */
function variable_format_boolean($variable, $options = array()) {
  if (isset($variable['value'])) {
    return $variable['value'] ? t('True') : t('False');
  }
  else {
    return t('Undefined');
  }
}

/**
 * Format variable empty value
 */
function variable_format_empty($variable) {
  return isset($variable['empty']) ? $variable['empty'] : t('Empty');
}

/**
 * Format text variable
 */
function variable_format_text($variable, $options = array()) {
  return isset($variable['value']) ? filter_xss_admin($variable['value']) : variable_format_empty($variable);
}

/**
 * Format mail variable
 */
function variable_format_mail_text($variable, $options = array()) {
  return 'Mail text'; //check_plain($variable['value']['subject']);
}

/**
 * Format unknown variable
 */
function variable_format_unknown($variable, $options = array()) {
  return '<pre>' . check_plain(print_r($variable['value'], TRUE)) . '</pre>';
}
/**
 * Options callback for numeric select
 */
function variable_options_select_number($variable, $options = array()) {
  return drupal_map_assoc($variable['options']);
}