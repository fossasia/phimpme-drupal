<?php
/*
 * Plugin definition for Ctools Export UI
 */
$plugin = array(
  'schema' => 'services_endpoint',
  'menu' => array(
    'menu item' => 'services',
    'menu description' => 'Manage Services',
    // Add services specific own menu callbacks.
    'items' => array(
      'resources' => array(
        'path' => 'list/%ctools_export_ui/resources',
        'title' => 'Resources',
        'page callback' => 'ctools_export_ui_switcher_page',
        'page arguments' => array('services_ctools_export_ui', 'resources', 4),
        'load arguments' => array('services_ctools_export_ui'),
        'access arguments' => array('administer services'),
        'type' => MENU_LOCAL_TASK,

      ),
      'server' => array(
        'path' => 'list/%ctools_export_ui/server',
        'title' => 'Server',
        'page callback' => 'ctools_export_ui_switcher_page',
        'page arguments' => array('services_ctools_export_ui', 'server', 4),
        'load arguments' => array('services_ctools_export_ui'),
        'access arguments' => array('administer services'),
        'type' => MENU_LOCAL_TASK,
        'weight' => -1,
      ),
      'authentication' => array(
        'path' => 'list/%ctools_export_ui/authentication',
        'title' => 'Authentication',
        'page callback' => 'ctools_export_ui_switcher_page',
        'page arguments' => array('services_ctools_export_ui', 'authentication', 4),
        'load arguments' => array('services_ctools_export_ui'),
        'access arguments' => array('administer services'),
        'type' => MENU_LOCAL_TASK,
        'weight' => -1,
      ),
    ),
  ),
  // Add our custom operations.
  'allowed operations' => array(
    'resources'  => array('title' => t('Edit Resources')),
    'server'  => array('title' => t('Edit Server')),
    'authentication' => array('title' => t('Edit Authentication')),
  ),
  'form' => array(
    'settings' => 'services_ctools_export_ui_form',
    'validate' => 'services_ctools_export_ui_form_validate',
    'submit' => 'services_ctools_export_ui_form_submit',
  ),
  'handler' => array(
    'class' => 'services_ctools_export_ui',
    'parent' => 'ctools_export_ui',
  ),

  'title' => t('Services'),

  'title singular' => t('endpoint'),
  'title plural' => t('endpoints'),
  'title singular proper' => t('Endpoint'),
  'title plural proper' => t('Endpoints'),
);

/**
 * Form to edit the settings of an endpoint.
 */
function services_ctools_export_ui_form(&$form, &$form_state) {
  // Loading runtime include as needed by services_auth_info().
  module_load_include('runtime.inc', 'services');
  $endpoint = $form_state['item'];
  $form['eid'] = array(
    '#type'  => 'value',
    '#value' => isset($endpoint->eid) ? $endpoint->eid : '',
  );

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  $servers = services_get_servers();
  $server_opts = array(
    '' => t('-- Select a server'),
  );
  foreach ($servers as $server => $info) {
    $server_opts[$server] = $info['name'];
  }
  $form['server'] = array(
    '#type'          => 'select',
    '#options'       => $server_opts,
    '#default_value' => $endpoint->server,
    '#title'         => t('Server'),
    '#description'   => t('Select a the server that should be used to handle requests to this endpoint.'),
    '#required'      => TRUE,
  );

  $form['path'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => $endpoint->path,
    '#title'         => t('Path to endpoint'),
    '#required'      => TRUE,
  );

  $form['debug'] = array(
    '#type'          => 'checkbox',
    '#default_value' => $endpoint->debug,
    '#title'         => t('Debug mode enabled'),
    '#description'   => t('Useful for developers. Do not enable on production environments'),
    '#required'      => FALSE,
  );
  $auth_modules = module_implements('services_authentication_info');

  if (!empty($auth_modules)) {
    $auth_options = array();
    foreach ($auth_modules as $module) {
      $info = services_authentication_info($module);
      $auth_options[$module] = $info['title'];
    }
    $default_values = array();
    foreach ($endpoint->authentication as $auth_module => $settings) {
      if (!empty($settings)) {
        $default_values[] = $auth_module;
      }
    }
    
    $form['authentication'] = array(
      '#type'          => 'checkboxes',
      '#options'       => $auth_options,
      '#default_value' => $default_values,
      '#title'         => t('Authentication'),
      '#description'   => t('Choose which authentication schemes that should ' .
        'be used with your endpoint. If no authentication method is selected ' .
        'all requests will be done by an anonymous user.'),
    );
  }
  else {
    $form['authentication'] = array(
      '#type'          => 'item',
      '#title'         => t('Authentication'),
      '#description'   => t('No authentication modules are installed, all ' .
        'requests will be anonymous.'),
    );
  }

  return $form;

}

/**
 * Validate submission of the preset edit form.
 */
function services_ctools_export_ui_form_validate(&$form, &$form_state) {
  // Test characters of name:
  if (preg_match("/[^a-z0-9_]/", $form_state['values']['name'])) {
    form_error($form['info']['name'], t('The endpoint name can only consist of lowercase letters, underscores, and numbers.'));
  }
  else {
    //Validate Name
    $query = db_select('services_endpoint', 'e');
    $query->addField('e', 'eid');
    $query->condition('name', $form_state['values']['name']);

    if (!empty($form_state['values']['eid']) && is_numeric($form_state['values']['eid'])) {
      $query->condition('eid', $form_state['values']['eid'], '!=');
    }

    $res = $query->execute()->fetchField();
    if (!empty($res)) {
      form_error($form['info']['name'], t('Endpoint name must be unique.'));
    }

    //Validate path
    $query = db_select('services_endpoint', 'e');
    $query->addField('e', 'eid');
    $query->condition('path', $form_state['values']['path']);

    if (!empty($form_state['values']['eid']) && is_numeric($form_state['values']['eid'])) {
      $query->condition('eid', $form_state['values']['eid'], '!=');
    }

    $res = $query->execute()->fetchField();
    if (!empty($res)) {
      form_error($form['path'], t('Endpoint path must be unique.'));
    }
  }
  //TODO: More validation? Eg. validate path? Transliteration etc?
}

/**
 * Submit handler for endpoint.
 */
function services_ctools_export_ui_form_submit(&$form, &$form_state) {
  $endpoint = $form_state['values']['endpoint_object'];

  $endpoint->name   = $form_state['values']['name'];
  $endpoint->server = $form_state['values']['server'];
  $endpoint->path   = $form_state['values']['path'];
  $endpoint->debug  = $form_state['values']['debug'];

  // Set the authentication modules, and preserve the settings for modules
  // that already exist.
  $auth = array();
  if (isset($form_state['values']['authentication'])) {
    foreach (array_keys($form_state['values']['authentication']) as $module) {
      //if module's checkbox is checked, add to empty
      $auth_module = $form_state['values']['authentication'][$module];
      if ($module === $auth_module) {
        //If existing settings are set, persist them
        if (isset($endpoint->authentication[$module]) &&  is_array($endpoint->authentication[$module])) {
          $auth[$module] = $endpoint->authentication[$module];
        } 
        else {
          $auth[$module] = $auth_module;
        }
      } 
      elseif ($auth_module == 0) {
        unset($auth[$module]);
      }
    }
  }
  $endpoint->authentication = $auth;
  services_endpoint_save($endpoint);
}