<?php

/**
 * @file
 * Builds placeholder replacement tokens for logintoboggan-related data.
 */

/**
 * Implements hook_token_info().
 */
function logintoboggan_token_info() {
  $info['tokens']['user']['validate-url'] = array(
    'name' => t('Validate URL'),
    'description' => t('The URL of the account validation page, provided by logintoboggan.'),
  );
  return $info;
}

/**
 * Implements hook_tokens().
 */
function logintoboggan_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
  }

  if ($type == 'user' && !empty($data['user'])) {
    $account = $data['user'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Validating URL.
        case 'validate-url':
          if (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) == USER_REGISTER_VISITORS) {
            $replacements[$original] = logintoboggan_eml_validate_url($account, $url_options);
          }
          break;
      }
    }
  }

  return $replacements;
}
