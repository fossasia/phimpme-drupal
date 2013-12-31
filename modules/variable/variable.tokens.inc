<?php

/**
 * @file
 * Builds placeholder replacement tokens system-wide data.
 *
 * This file handles tokens for the global 'variable' token type.
 */

/**
 * Implements hook_token_info().
 */
function variable_token_info() {
  $types['variable'] = array(
    'name' => t("Variables"),
    'description' => t("Tokens for variable values."),
  );
  $variable = array();
  foreach (variable_get_info() as $name => $info) {
    if (!empty($info['token'])) {
      $variable[$name] = array(
        'name' => $info['title'],
        'description' => !empty($info['description']) ? $info['description'] : t('Value of variable !name', array('!name' => $info['title'])),
      );
    }
  }
  return array(
    'types' => $types,
    'tokens' => array(
      'variable' => $variable,
    ),
  );  
}

/**
 * Implements hook_tokens().
 */
function variable_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  if ($type == 'variable') {
    foreach ($tokens as $name => $original) {
      $variable = variable_get_info($name, $options);
      if ($variable && !empty($variable['token'])) {
        $replacements[$original] = variable_format_value($variable, $options); 
      }
    }
  }

  return $replacements;
}