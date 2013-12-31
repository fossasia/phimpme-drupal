<?php
/**
 * @file
 * Variable API module. Definition for Drupal core variables
 */

/**
 * Implements hook_variable_info().
 */
function menu_variable_info($options) {
  $variables['menu_main_links_source'] = array(
    'type' => 'select',
    'title' => t('Source for the Main links'),
    'options' => 'menu',
    'default' => 'main-menu',
    'element' => array('#empty_option' => t('No Main links')),
    'description' => t('Select what should be displayed as the Main links (typically at the top of the page).', array(), $options),
    'group' => 'menu_settings'
  );
  $variables['menu_secondary_links_source'] = array(
    'type' => 'select',
    'title' => t('Source for the Secondary links'),
    'options' => 'menu',  
    'default' => 'user-menu',
    'element' => array('#empty_option' => t('No Secondary links')),
    'description' => t('Select the source for the Secondary links.', array() , $options),
    'group' => 'menu_settings'
  );
  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function menu_variable_group_info() {
  $groups['menu_settings'] = array(
    'title' => t('Menu settings'),
    'access' => 'administer menu',
    'path' => 'admin/structure/menu/settings',
  );
  return $groups;
}

/**
 * Implements hook_variable_type_info()
 */
function menu_variable_type_info() {
  $type['menu'] = array(
    'title' => t('Menu'),
    'type' => 'select',
    'options callback' => 'menu_variable_menu_list',
  );
  return $type;
}

/**
 * Menu option list
 */
function menu_variable_menu_list($variable, $options) {
  return menu_get_menus();
}