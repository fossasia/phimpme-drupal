<?php

/**
 * @file
 *
 * Administrative functions for Varnish integration.
 */

/**
 * Menu callback for varnish admin settings.
 */
function varnish_admin_settings_form() {
  $form = array();

  if (!extension_loaded('sockets')) {
    drupal_set_message(t('<a href="http://php.net/manual/en/sockets.installation.php">PHP Sockets extension</a> not enabled. Varnish terminal communication configuration skipped.'), 'error');
    return system_settings_form($form);
  }

  // Begin socket-dependent configuration.

  // Decide whether or not to flush caches on cron runs.
  $form['varnish_flush_cron'] = array(
    '#type' => 'radios',
    '#title' => t('Flush page cache on cron?'),
    '#options' => array(
      0 => t('Disabled'),
      1 => t('Enabled (with respect for cache_lifetime)'),
    ),
    '#default_value' => variable_get('varnish_flush_cron', 0),
    '#description' => t('Internally Drupal will attempt to flush its page cache every time cron.php runs. This can mean too-frequent cache flushes if you have cron running frequently. NOTE: this cache flush is global!'),
  );

  $form['varnish_version'] = array(
    '#type' => 'select',
    '#title' => t('Varnish version'),
    '#default_value' => variable_get('varnish_version', 2.1),
    '#description' => t('Select your varnish version.'),
    '#options' => array(
      '2' => '2.0.x',
      '2.1' => '2.1.x',
      '3' => '3.x',
    ),
  );

  $form['varnish_control_terminal'] = array(
    '#type' => 'textfield',
    '#title' => t('Varnish Control Terminal'),
    '#default_value' => variable_get('varnish_control_terminal', '127.0.0.1:6082'),
    '#required' => TRUE,
    '#description' => t('Set this to the server IP or hostname that varnish runs on (e.g. 127.0.0.1:6082). This must be configured for Drupal to talk to Varnish. Separate multiple servers with spaces.'),
  );

  $form['varnish_control_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Varnish Control Key'),
    '#default_value' => variable_get('varnish_control_key', ''),
    '#description' => t('Optional: if you have established a secret key for control terminal access, please put it here.'),
  );
  $form['varnish_socket_timeout'] = array(
   '#type' => 'textfield',
   '#title' => t('Varnish connection timeout (milliseconds)'),
   '#default_value' => variable_get('varnish_socket_timeout', VARNISH_DEFAULT_TIMEOUT),
   '#description' => t('If Varnish is running on a different server, you may need to increase this value.'),
   '#required' => TRUE,
  );
  $form['varnish_cache_clear'] = array(
    '#type' => 'radios',
    '#title' => t('Varnish Cache Clearing'),
    '#options' => array(
      VARNISH_DEFAULT_CLEAR => t('Drupal Default'),
      VARNISH_NO_CLEAR => t('None'),
    ),
    '#default_value' => variable_get('varnish_cache_clear', VARNISH_DEFAULT_CLEAR),
    '#description' => t('What kind of cache clearing Varnish should utilize. Drupal default will clear all page caches on node updates and cache flush events. None will allow pages to persist for their full max-age; use this if you want to write your own cache-clearing logic.'),
  );

  // Detect expire module and add this option.
  if (module_exists('expire')) {
    $form['varnish_cache_clear']['#options'][VARNISH_SELECTIVE_CLEAR] = t('Selective (experimental; uses expire.module)');
    $form['varnish_cache_clear']['#description'] .= ' ' . t('Selective will clear a list of URLs you specify as well as any node urls themselves.');
  }
  else {
    $form['varnish_cache_clear']['#description'] .= ' ' . t('Installing the !link will enable "Selective" clearing.', array('!link' => '<a href="http://drupal.org/project/expire" target="_blank">' . t('Expire module') . '</a>'));
  }

  // Check status
  $form['varnish_stats'] = array(
    '#type' => 'item',
    '#title' => t('Status'),
    '#markup' => theme('varnish_status', varnish_get_status(), floatval(variable_get('varnish_version', 2.1))),
  );

  return system_settings_form($form);
}

function varnish_admin_settings_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['varnish_socket_timeout']) || $form_state['values']['varnish_socket_timeout'] < 0) {
    form_set_error('varnish_socket_timeout', t('Varnish connection timeout must be a positive number.'));
  }
  else {
    $form_state['values']['varnish_socket_timeout'] = (int) $form_state['values']['varnish_socket_timeout'];
  }
}


/**
 * Menu callback for varnish admin settings.
 */
function varnish_admin_reports_page() {
  if (floatval(variable_get('varnish_version', 2.1)) >= 3) {
    return t('Varnish 3 has removed the option to display statistics directly from the terminal. Use varnishstat instead.');
  }
  // Connect to varnish and do a full status report.
  $status = _varnish_terminal_run(array('stats'));
  $output = '';
  foreach ($status as $terminal => $stat) {
    $output .= '<pre>' . $stat['stats']['msg'] . '</pre>';
  }
  return $output;
}
