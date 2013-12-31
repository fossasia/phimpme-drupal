<?php
/**
 * @file
 * Variable API module - Admin UI
 */

/**
 * Variable overview, by group
 */
function variable_admin_page_group($group = NULL) {
  variable_include();
  if ($group && ($group_info = variable_get_group($group))) {
    $group_info += array('description' => '');
    drupal_set_title($group_info['title']);
    $list = variable_list_group($group);
    $build['description']['#markup'] = $group_info['description']; 
    $build['form'] = drupal_get_form('variable_edit_form', array_keys($list));
    return $build;
  }
  else {
    // Groups overview
    $build['groups'] = array(
      '#type' => 'vertical_tabs',
    );
    foreach (variable_get_group() as $group => $info) {
      $build['groups'][$group] = array(
        '#type' => 'fieldset',
        '#title' => $info['title'],
        '#collapsible' => TRUE, '#collapsed' => TRUE,
        '#description' => (!empty($info['description']) ? $info['description'] . ' ' : '') . l(t('Edit group variables'), 'admin/config/system/variable/group/' . $group),
      );
      $build['groups'][$group]['list'] = variable_admin_group_list(variable_list_group($group));
    }
    return $build;
  }
}

/**
 * Build list of variables for group
 * 
 * @return Renderable array
 */
function variable_admin_group_list($variables) {
  $list = array();
  foreach ($variables as $name => $variable) {
    $type = variable_get_type($variable['type']);
    $list[$name] = array(
      l($variable['title'], 'admin/config/system/variable/edit/' . $name),
      $type['title'],
      variable_format_value($variable), //isset($variable['description']) ? $variable['description'] : ''
    );
  }
  return array(
    '#theme' => 'table',
    '#rows' => $list,
  );
}

/**
 * Variable overview, by group
 */
function variable_admin_page_module($name = NULL) {
  variable_include();
  if ($name && (in_array($name, module_list()))) {
    $modules = system_list('module_enabled');
    $module = $modules[$name];
    drupal_set_title($module->info['name']);
    $list = variable_list_module($name);
    $build['description']['#markup'] = $module->info['description']; 
    $build['form'] = drupal_get_form('variable_edit_form', array_keys($list));
    return $build;
  }
  else {
    // Groups overview
    $build['modules'] = array(
      '#type' => 'vertical_tabs',

    );
    foreach (system_list('module_enabled') as $module) {
      if ($variables = variable_list_module($module->name)) {
        $build['modules'][$module->name] = array(
          '#type' => 'fieldset',
          '#title' => $module->info['name'],
          '#collapsible' => TRUE, '#collapsed' => TRUE,
          '#description' => $module->info['description'] . ' ' . l(t('Edit module variables'), 'admin/config/system/variable/module/' . $module->name),
          '#group' => 'modules',
        );
        $build['modules'][$module->name]['list'] = variable_admin_group_list($variables);
      }
    }
    return $build;
  }
}
