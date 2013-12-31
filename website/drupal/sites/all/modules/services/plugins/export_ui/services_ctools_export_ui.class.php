<?php

/**
 * @file
 * Export-ui handler for the Services module.
 */

class services_ctools_export_ui extends ctools_export_ui {

  /**
   * Page callback for the resources page.
   */
  function resources_page($js, $input, $item) {
    drupal_set_title($this->get_page_title('resources', $item));
    return drupal_get_form('services_edit_form_endpoint_resources', $item);
  }

  /**
   * Page callback for the server page.
   */
  function server_page($js, $input, $item) {
    drupal_set_title($this->get_page_title('server', $item));
    return drupal_get_form('services_edit_form_endpoint_server', $item);
  }


  /**
   * Page callback for the authentication page.
   */
  function authentication_page($js, $input, $item) {
    drupal_set_title($this->get_page_title('authentication', $item));
    return drupal_get_form('services_edit_form_endpoint_authentication', $item);
  }
  // Avoid standard submit of edit form by ctools.
  function edit_save_form($form_state) { }
    
  function set_item_state($state, $js, $input, $item) {
    ctools_export_set_object_status($item, $state);

    menu_rebuild();
    if (!$js) {
      drupal_goto(ctools_export_ui_plugin_base_path($this->plugin));
    }
    else {
      return $this->list_page($js, $input);
    }
  }
}

/**
 * Endpoint authentication configuration form.
 */
function services_edit_form_endpoint_authentication($form, &$form_state) {
  list($endpoint) = $form_state['build_info']['args'];
  // Loading runtime include as needed by services_authentication_info().
  module_load_include('runtime.inc', 'services');

  $auth_modules = module_implements('services_authentication_info');

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );
  if (empty($auth_modules)) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('Authentication'),
      '#description'   => t('No authentication modules are installed, all requests will be anonymous.'),
    );
    return $form;
  }
  if (empty($endpoint->authentication)) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('Authentication'),
      '#description'   => t('No authentication modules are enabled, all requests will be anonymous.'),
    );
    return $form;
  }
  // Add configuration fieldsets for the authentication modules
  foreach ($endpoint->authentication as $module => $settings) {
    $info = services_authentication_info($module);
    if (empty($info)) {
      continue;
    }
    $form[$module] = array(
      '#type' => 'fieldset',
      '#title' => isset($info['title']) ? $info['title'] : $module,
      '#tree' => TRUE,
    );
    $module_settings_form = services_auth_invoke($module, 'security_settings', $settings);

    if (!empty($module_settings_form) && $module_settings_form !== TRUE && $settings == $module || is_array($settings)) {
      $form[$module] += $module_settings_form;
    }
    else {
      $form[$module]['message'] = array(
        '#type'   => 'item',
        '#markup'  => t('@module has no settings available.', array('@module' => drupal_ucfirst($module))),
      );
    }
  }

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

function services_edit_form_endpoint_authentication_submit($form, $form_state) {
  $endpoint = $form_state['values']['endpoint_object'];

  foreach (array_keys($endpoint->authentication) as $module) {
    if (isset($form_state['values'][$module])) {
      $endpoint->authentication[$module] = $form_state['values'][$module];
    }
  }

  drupal_set_message(t('Your authentication options have been saved.'));
  services_endpoint_save($endpoint);
}

function services_edit_form_endpoint_server($form, &$form_state) {
  list($endpoint) = $form_state['build_info']['args'];
  $servers = services_get_servers();

  $server = !empty($servers[$endpoint->server]) ? $servers[$endpoint->server] : FALSE;

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  if (!$server) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('Unknown server @name', array('@name' => $endpoint->server)),
      '#description'   => t('No server matching the one used in the endpoint.'),
    );
  }
  else if (empty($server['settings'])) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('@name has no settings', array('@name' => $endpoint->server)),
      '#description'   => t("The server doesn't have any settings that needs to be configured."),
    );
  }
  else {
    $definition = $server['settings'];

    $settings = isset($endpoint->server_settings[$endpoint->server]) ? $endpoint->server_settings[$endpoint->server] : array();

    if (!empty($definition['file'])) {
      call_user_func_array('module_load_include', $definition['file']);
    }

    $form[$endpoint->server] = array(
      '#type' => 'fieldset',
      '#title' => $server['name'],
      '#tree' => TRUE,
    );
    call_user_func_array($definition['form'], array(&$form[$endpoint->server], $endpoint, $settings));

    $form['submit'] = array(
      '#type'  => 'submit',
      '#value' => 'Save',
    );
  }

  return $form;
}

function services_edit_form_endpoint_server_submit($form, $form_state) {
  $endpoint = $form_state['values']['endpoint_object'];
  $servers = services_get_servers();
  $definition = $servers[$endpoint->server]['settings'];

  $values = $form_state['values'][$endpoint->server];

  // Allow the server to alter the submitted values before they're stored
  // as settings.
  if (!empty($definition['submit'])) {
    if (!empty($definition['file'])) {
      call_user_func_array('module_load_include', $definition['file']);
    }
    $values = call_user_func_array($definition['submit'], array($endpoint, &$values));
  }

  // Store the settings in the endpoint
  $endpoint->server_settings[$endpoint->server] = $values;
  services_endpoint_save($endpoint);

  drupal_set_message(t('Your server settings have been saved.'));
}

/**
 * services_edit_endpoint_resources function.
 *
 * Edit Resources endpoint form
 * @param object $endpoint
 * @return string  The form to be displayed
 */
function services_edit_endpoint_resources($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  if ($endpoint && !empty($endpoint->title)) {
    drupal_set_title($endpoint->title);
  }
  return drupal_get_form('services_edit_form_endpoint_resources', $endpoint);
}

/**
 * services_edit_form_endpoint_resources function.
 *
 * @param array &$form_state
 * @param object $endpoint
 * @return Form
 */
function services_edit_form_endpoint_resources($form, &$form_state, $endpoint) {
  module_load_include('resource_build.inc', 'services');
  $form = array();

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  $form['#attached']['js'] = array(
    'misc/tableselect.js',
    drupal_get_path('module', 'services') . '/js/services.admin.js',
  );

  $form['#attached']['css'] = array(
    drupal_get_path('module', 'services') . '/css/services.admin.css',
  );

  $ops = array(
    'create'   => t('Create'),
    'retrieve' => t('Retrieve'),
    'update'   => t('Update'),
    'delete'   => t('Delete'),
    'index'    => t('Index'),
  );

  // Call _services_build_resources() directly instead of
  // services_get_resources to bypass caching.
  $resources = _services_build_resources();
  // Apply the endpoint in a non-strict mode, so that the non-active resources
  // are preserved.
  _services_apply_endpoint($resources, $endpoint, FALSE);

  $form['resources'] = array(
    '#type' => 'fieldset',
    '#title' => t('Resources'),
    '#description' => t('Select the resource(s) or resource group(s) you would like to enable, and click <em>Save</em>.'),
   );

  $form['resources']['table'] = array(
    '#theme' => 'services_resource_table',
   );

  $ignoreArray = array('actions', 'relationships', 'endpoint', 'name', 'file', 'targeted_actions');
  // Generate the list of methods arranged by resource.
  foreach ($resources as $resource => $methods) {
    $form['resources']['table'][$resource] = array(
      '#collapsed' => TRUE,
    );

    $alias = '';
    if (isset($form_state['build_info']['args'][0]->resources[$resource]['alias'])) {
      $alias = $form_state['build_info']['args'][0]->resources[$resource]['alias'];
    }
    elseif (isset($form_state['input'][$resource . '/alias'])) {
      $alias = $form_state['input'][$resource . '/alias'];
    }

    $form['resources']['table'][$resource]['alias'] = array(
      '#type' => 'textfield',
      '#default_value' => $alias,
      '#name' => $resource .'/alias',
      '#size' => 20,
    );
    foreach ($methods as $class => $info) {
      if (!in_array($class, $ignoreArray)) {
        if (!isset($info['help'])) {
          $description = t('No description is available');
        } else {
          $description = $info['help'];
        }
        if (isset($form_state['build_info']['args'][0]->resources[$resource]['operations'][$class])) {
          $default_value = $form_state['build_info']['args'][0]->resources[$resource]['operations'][$class]['enabled'];
        }
        else {
          $default_value = 0;
        }
        $form['resources']['table'][$resource][$resource .'/'. $class] = array(
          '#type' => 'checkbox',
          '#title' => $class,
          '#description' => $description,
          '#default_value' => $default_value,
         );
      }
      elseif($class == 'actions' || $class == 'relationships' || $class == 'targeted_actions') {
        foreach($info as $key => $action) {
          if (!isset($action['help'])) {
            $description = t('No description is available');
          }
          else {
            $description = $action['help'];
          }
          if (isset($form_state['build_info']['args'][0]->resources[$resource][$class][$key])) {
            $default_value = $form_state['build_info']['args'][0]->resources[$resource][$class][$key]['enabled'];
          }
          else {
            $default_value = 0;
          }
          $form['resources']['table'][$resource][$resource .'/'. $key .'/'. $class] = array(
            '#type' => 'checkbox',
            '#title' => $key,
            '#description' => $description,
            '#default_value' => $default_value,
          );
         }
       }
     }
   }

   $form['save'] = array(
     '#type'  => 'submit',
     '#value' => t('Save'),
   );
  return $form;
}

/**
 * services_edit_form_endpoint_resources_validate function.
 *
 * @param array $form
 * @param array $form_state
 * @return void
 */
function services_edit_form_endpoint_resources_validate($form, $form_state) {
  $input = $form_state['values']['endpoint_object'];

  // Validate aliases.
  foreach ($input as $key => $value) {
    if (strpos($key, '/alias') !== FALSE && !empty($value) && !preg_match('/^[a-z-]+$/', $value)) {
      list($resource_name,) = explode('/', $key);
      // Still this doesn't highlight needed form element.
      form_set_error("resources][table][$resource_name][alias", t("The alias for the !name resource may only contain lower case a-z and dashes.", array(
        '!name' => $resource_name,
      )));
    }
  }
}

/**
 * Resources form submit function.
 *
 * @param array $form
 * @param array $form_state
 * @return void
 */
function services_edit_form_endpoint_resources_submit($form, $form_state) {
  $endpoint  = $form_state['values']['endpoint_object'];
  $existing_resources = _services_build_resources();
  // Apply the endpoint in a non-strict mode, so that the non-active resources
  // are preserved.
  _services_apply_endpoint($existing_resources, $endpoint, FALSE);
  $resources = $form_state['input'];
  $endpoint = $form_state['build_info']['args'][0];

  foreach ($resources as $path => $state) {
    if (strpos($path, '/') === FALSE || empty($state)) {
      continue;
    }
    $split_path = explode('/', $path);
    $resource = $split_path[0];
    $method = $split_path[1];
    // If method is alias.
    if ($method == 'alias') {
      $final_resource[$resource]['alias'] = $state;
      continue;
    }
    // If it is action, relationship, or targeted action.
    if (isset($split_path[2])) {
      $final_resource[$resource][$split_path[2]][$method]['enabled'] = 1;
      continue;
    }
    // If it is operation.
    $final_resource[$resource]['operations'][$method]['enabled'] = 1;
  }
  $endpoint->resources = $final_resource;
  services_endpoint_save($endpoint);
  drupal_set_message('Resources have been saved');
}
