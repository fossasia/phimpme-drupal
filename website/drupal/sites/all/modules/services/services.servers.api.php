<?php

/**
 * @file
 * Hooks provided by Services for the definition of servers.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Identifies a server implementation to Services.
 *
 * @return
 *   An associative array with the following keys.
 *
 *   - name: The display name of this server.
 *	 - settings: an assoc array containing settings information per endpoint that this server is enabled.
 */
function hook_server_info() {
  return array(
    'name' => 'REST',
    'path' => 'rest',
    'settings' => array(
      'file' => array('inc', 'rest_server'),
      'form' => '_rest_server_settings',
      'submit' => '_rest_server_settings_submit',
    ),
  );
}

/**
 * Acts on requests to the server defined in hook_server_info().
 *
 * This is the main entry point to your server implementation.
 * Need to get some more description about the best way to implement
 * servers.
 */
function hook_server() {
  $endpoint_path = services_get_server_info('endpoint_path', 'services/rest');
  $canonical_path = trim(drupal_substr($_GET['q'], drupal_strlen($endpoint_path)), '/');
  $canonical_path = explode('/', $_GET['q']);
  $endpoint_path_count = count(explode('/', $endpoint_path));
  for ($x = 0; $x < $endpoint_path_count; $x++) {
    array_shift($canonical_path);
  }
  $canonical_path = implode('/', $canonical_path);
  if (empty($canonical_path)) {
    return '';
  }
  //Handle server based on $canonical_path
}