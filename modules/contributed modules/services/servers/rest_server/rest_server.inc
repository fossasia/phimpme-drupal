<?php

/**
 * @file
 * Autoload classes and server settings.
 */

/**
 * REST server settings form. Generates the form fragment for configuring the REST server
 * for an endpoint.
 *
 * @param array $form
 *  The form fragment from services that we should add our fields to.
 * @param object $endpoint
 *  The endpoint that we're configuring the REST server for.
 * @param array $settings
 *  The current settings.
 * @return void
 */
function _rest_server_settings(&$form, $endpoint, $settings) {
  $settings = rest_server_setup_settings($settings);

  $form['formatters'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Response formatters'),
    '#required'      => TRUE,
    '#description'   => t('Select the response formats you want to enable for the rest server.'),
  ) + _rest_server_settings_checkboxes_attributes($settings['formatters']);

  $form['parsers'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Request parsing'),
    '#required'      => TRUE,
    '#description'   => t('Select the request parser types you want to enable for the rest server.'),
  ) + _rest_server_settings_checkboxes_attributes($settings['parsers']);
}

/**
 * Utility function that creates attributes for a checkboxes-type form
 * element from a rest server settings array.
 *
 * @param array $settings
 * @return array
 */
function _rest_server_settings_checkboxes_attributes($settings) {
  $keys = array_keys($settings);
  $options = array_combine($keys, $keys);
  $default = array();
  foreach ($settings as $key => $enabled) {
    if ($enabled) {
      $default[] = $key;
    }
  }
  ksort($options);
  return array(
    '#options' => $options,
    '#default_value' => $default,
  );
}

/**
 * Submit handler for the services REST server settings form.
 *
 * @param object $endpoint
 *  The endpoint that's being configured.
 * @param array $values
 *  The partial form-state from services.
 * @return array
 *  The settings for the REST server in this endpoint.
 */
function _rest_server_settings_submit($endpoint, &$values) {
  $values['formatters'] = array_map('_rest_server_settings_not_zero', $values['formatters']);
  $values['parsers'] = array_map('_rest_server_settings_not_zero', $values['parsers']);
  return $values;
}

/**
 * Utility function intended for use with array_map to change everything that
 * isn't === 0 into TRUE.
 *
 * @param string $value
 *  The value to map.
 * @return bool
 *  FALSE if the $value is === 0 otherwise TRUE is returned.
 */
function _rest_server_settings_not_zero($value) {
  return $value !== 0;
}
