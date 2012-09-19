<?php

// Menu callback that shows menu item.
function devel_menu_item() {
  $item = menu_get_item($_GET['path']);
  return kdevel_print_object($item);
}

/**
 * Returns a list of all currently defined user functions in the current
 * request lifecycle, with links their documentation.
 */
function devel_function_reference() {
  $functions = get_defined_functions();
  $version = devel_get_core_version(VERSION);
  $ufunctions = $functions['user'];
  sort($ufunctions);
  $api = variable_get('devel_api_url', 'api.drupal.org');
  foreach ($ufunctions as $function) {
    $links[] = l($function, "http://$api/api/$version/function/$function");
  }
  return theme('item_list', array('items' => $links));
}

/**
 * Menu callback; clears all caches, then redirects to the previous page.
 */
function devel_cache_clear() {
  drupal_flush_all_caches();

  drupal_set_message('Cache cleared.');

  drupal_goto();
}

// A menu callback. Called by the AJAX link in query log.
function devel_querylog_explain($request_id, $qid) {
  if (!is_numeric($request_id)) {
    return MENU_ACCESS_DENIED;
  }

  $path = "temporary://devel_querylog/$request_id.txt";
  $path = file_stream_wrapper_uri_normalize($path);
  $queries = json_decode(file_get_contents($path));
  $query = $queries[$qid];
  $result = db_query('EXPLAIN ' . $query->query, (array)$query->args)->fetchAllAssoc('table');
  $i = 1;
  foreach ($result as $row) {
    $row = (array)$row;
    if ($i == 1) {
      $header = array_keys($row);
    }
    $rows[] = array_values($row);
    $i++;
  }
  // Print and return nothing thus avoiding page wrapper.
  $output = theme('table', array('header' => $header, 'rows' => $rows));
  print $output;
  $GLOBALS['devel_shutdown'] = FALSE;
}

// A menu callback. Called by the AJAX link in query log.
function devel_querylog_arguments($request_id, $qid) {
  if (!is_numeric($request_id)) {
    return MENU_ACCESS_DENIED;
  }

  $path = "temporary://devel_querylog/$request_id.txt";
  $path = file_stream_wrapper_uri_normalize($path);
  $queries = json_decode(file_get_contents($path));
  $query = $queries[$qid];
  $conn = Database::getConnection();
  $quoted = array();
  foreach ((array)$query->args as $key => $val) {
    $quoted[$key] = $conn->quote($val);
  }
  $output = strtr($query->query, $quoted);

  // print and return nothing thus avoiding page wrapper.
  print $output;
  $GLOBALS['devel_shutdown'] = FALSE;
}

/**
 * Menu callback; clear the database, resetting the menu to factory defaults.
 */
function devel_menu_rebuild() {
  menu_rebuild();
  drupal_set_message(t('The menu router has been rebuilt.'));
  drupal_goto();
}

/**
 * Display a dropdown of installed modules with the option to reinstall them.
 */
function devel_reinstall($form, &$form_state) {
  $output = '';
  $modules = module_list();
  sort($modules);
  $options = drupal_map_assoc($modules);
  $form['list'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#description' => t('Uninstall and then install the selected modules. <code>hook_uninstall()</code> and <code>hook_install()</code> will be executed and the schema version number will be set to the most recent update number. You may have to manually clear out any existing tables first if the module doesn\'t implement <code>hook_uninstall()</code>.'),
  );
  $form['submit'] = array(
    '#value' => t('Reinstall'),
    '#type' => 'submit',
  );
  return $form;
}

/**
 * Process reinstall menu form submissions.
 */
function devel_reinstall_submit($form, &$form_state) {
  // require_once './includes/install.inc';
  $modules = array_filter($form_state['values']['list']);
  module_disable($modules, FALSE);
  drupal_uninstall_modules($modules);
  module_enable($modules, FALSE);
  drupal_set_message(t('Uninstalled and installed: %names.', array('%names' => implode(', ', $modules))));
}

// Menu callback.
function devel_theme_registry() {
  drupal_theme_initialize();
  $hooks = theme_get_registry();
  ksort($hooks);
  return kprint_r($hooks, TRUE);
}

// Menu callback. $entity_type argument not currently used in the UI.
function devel_entity_info_page($entity_type = NULL) {
  $info = entity_get_info($entity_type);
  ksort($info);
  return kprint_r($info, TRUE);
}

// Menu callback.
function devel_field_info_page() {
  $info = field_info_fields();
  $output = kprint_r($info, TRUE, t('Fields'));
  $info = field_info_instances();
  $output .= kprint_r($info, TRUE, t('Instances'));
  $info = field_info_bundles();
  $output .= kprint_r($info, TRUE, t('Bundles'));
  return $output;
}

/**
 * Menu callback; display all variables.
 */
function devel_variable_page() {
  // We return our own $page so as to avoid blocks.
  $output = drupal_get_form('devel_variable_form');
  drupal_set_page_content($output);
  $page = element_info('page');
  return $page;
}

function devel_variable_form() {
  $header = array(
    'name' => array('data' => t('Name'), 'field' => 'name', 'sort' => 'asc'),
    'value' => array('data' => t('Value'), 'field' => 'value'),
    'length' => array('data' => t('Length'), 'field' => 'length'),
    'edit' => array('data' => t('Operations')),
  );
  // TODO: we could get variables out of $conf but that would include hard coded ones too. ideally i would highlight overrridden/hard coded variables
  $query = db_select('variable', 'v')->extend('TableSort');
  $query->fields('v', array('name', 'value'));
  switch (db_driver()) {
    case 'mssql':
      $query->addExpression("LEN(v.value)", 'length');
      break;
    default:
      $query->addExpression("LENGTH(v.value)", 'length');
      break;
  }
  $result = $query
    ->orderByHeader($header)
    ->execute();

  foreach ($result as $row) {
    // $variables[$row->name] = '';
    $options[$row->name]['name'] = check_plain($row->name);
    if (merits_krumo($row->value)) {
      $value = krumo_ob(variable_get($row->name, NULL));
    }
    else {
      if (drupal_strlen($row->value) > 70) {
        $value = check_plain(drupal_substr($row->value, 0, 65)) .'...';
      }
      else {
        $value = check_plain($row->value);
      }
    }
    $options[$row->name]['value'] = $value;
    $options[$row->name]['length'] = $row->length;
    $options[$row->name]['edit'] = l(t('Edit'), "devel/variable/edit/$row->name");
  }
  $form['variables'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No variables.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

    // krumo($form);
  return $form;
}

function devel_variable_form_submit($form, &$form_state) {
  $deletes = array_filter($form_state['values']['variables']);
  array_walk($deletes, 'variable_del');
  if (count($deletes)) {
    drupal_set_message(format_plural(count($deletes), 'One variable deleted.', '@count variables deleted.'));
  }
}

function devel_variable_edit($form, &$form_state, $name) {
  $value = variable_get($name, 'not found');
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $name
  );
  $form['value'] = array(
    '#type' => 'item',
    '#title' => t('Old value'),
    '#markup' => dpr($value, TRUE),
  );
  if (is_string($value) || is_numeric($value)) {
    $form['new'] = array(
      '#type' => 'textarea',
      '#title' => t('New value'),
      '#default_value' => $value
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  }
  else {
    $api = variable_get('devel_api_url', 'api.drupal.org');
    $form['new'] = array(
      '#type' => 'item',
      '#title' => t('New value'),
      '#value' => t('Sorry, complex variable types may not be edited yet. Use the <em>Execute PHP</em> block and the <a href="@variable-set-doc">variable_set()</a> function.', array('@variable-set-doc' => "http://$api/api/HEAD/function/variable_set"))
    );
  }
  drupal_set_title($name);
  return $form;
}

function devel_variable_edit_submit($form, &$form_state) {
  variable_set($form_state['values']['name'], $form_state['values']['new']);
  drupal_set_message(t('Saved new value for %name.', array('%name' => $form_state['values']['name'])));
   'devel/variable';
}

/**
 * Menu callback: display the session.
 */
function devel_session() {
  global $user;
  $output = kprint_r($_SESSION, TRUE);
  $headers = array(t('Session name'), t('Session ID'));
  $output .= theme('table', array('headers' => $headers, 'rows' => array(array(session_name(), session_id()))));
  return $output;
}

/**
 * Menu callback; prints the loaded structure of the current node/user.
 */
function devel_load_object($type, $object, $name = NULL) {
  $name = isset($name) ? $name : $type;
  return kdevel_print_object($object, '$'. $name .'->');
}

/**
 * Menu callback; prints the render structure of the current object (currently node or user).
 */
function devel_render_object($type, $object, $name = NULL) {
  $name = isset($name) ? $name : $type;
  $function = $type . '_view';
  $build = $function($object);
  return kdevel_print_object($build, '$'. $name .'->');
}

function devel_elements_page() {
  return kdevel_print_object(module_invoke_all('element_info'));
}

function devel_phpinfo() {
  print phpinfo();
  drupal_exit();
}
