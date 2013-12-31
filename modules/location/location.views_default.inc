<?php

/**
 * @file
 * Default views for location module.
 */

/**
 * Implementation of hook_default_view_views().
 */
function location_views_default_views() {
  $view = new view;
  $view->name = 'location_table';
  $view->description = 'Node location table.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Title',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'street' => array(
      'label' => 'Street',
      'style' => 'both',
      'exclude' => 0,
      'id' => 'street',
      'table' => 'location',
      'field' => 'street',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'city' => array(
      'label' => 'City',
      'exclude' => 0,
      'id' => 'city',
      'table' => 'location',
      'field' => 'city',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'province' => array(
      'label' => 'Province',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'province',
      'table' => 'location',
      'field' => 'province',
      'relationship' => 'none',
    ),
    'postal_code' => array(
      'label' => 'Postal Code',
      'exclude' => 0,
      'id' => 'postal_code',
      'table' => 'location',
      'field' => 'postal_code',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'country' => array(
      'label' => 'Country',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'country',
      'table' => 'location',
      'field' => 'country',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'lid' => array(
      'operator' => 'not empty',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'lid',
      'table' => 'location',
      'field' => 'lid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view node location table',
  ));
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'street' => 'street',
      'city' => 'city',
      'province' => 'province',
      'postal_code' => 'postal_code',
      'country' => 'country',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'street' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'city' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'province' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'postal_code' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'country' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'location/views');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Location table',
    'weight' => '0',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;


  $view = new view;
  $view->name = 'location_directory';
  $view->description = 'Location directory filtered by arguments in the url.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => 'Title',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'street' => array(
      'label' => 'Street',
      'style' => 'both',
      'exclude' => 0,
      'id' => 'street',
      'table' => 'location',
      'field' => 'street',
      'relationship' => 'none',
    ),
    'city' => array(
      'label' => 'City',
      'exclude' => 0,
      'id' => 'city',
      'table' => 'location',
      'field' => 'city',
      'relationship' => 'none',
    ),
    'province' => array(
      'label' => 'Province',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'province',
      'table' => 'location',
      'field' => 'province',
      'relationship' => 'none',
    ),
    'postal_code' => array(
      'label' => 'Postal Code',
      'exclude' => 0,
      'id' => 'postal_code',
      'table' => 'location',
      'field' => 'postal_code',
      'relationship' => 'none',
    ),
    'country' => array(
      'label' => 'Country',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'country',
      'table' => 'location',
      'field' => 'country',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'country' => array(
      'default_action' => 'summary asc',
      'style_plugin' => 'default_summary',
      'style_options' => array(
        'count' => 1,
        'override' => 0,
        'items_per_page' => '25',
      ),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'id' => 'country',
      'table' => 'location',
      'field' => 'country',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'page' => 0,
        'story' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_php' => '',
    ),
    'province' => array(
      'default_action' => 'summary asc',
      'style_plugin' => 'default_summary',
      'style_options' => array(
        'count' => 1,
        'override' => 0,
        'items_per_page' => '25',
      ),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%2',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'id' => 'province',
      'table' => 'location',
      'field' => 'province',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'page' => 0,
        'story' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_php' => '',
    ),
    'city' => array(
      'default_action' => 'summary asc',
      'style_plugin' => 'default_summary',
      'style_options' => array(
        'count' => 1,
        'override' => 0,
        'items_per_page' => '25',
      ),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%3',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 1,
      'limit' => '1',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'city',
      'table' => 'location',
      'field' => 'city',
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'page' => 0,
        'story' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'relationship' => 'none',
    ),
    'lid' => array(
      'operator' => 'not empty',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'lid',
      'table' => 'location',
      'field' => 'lid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view location directory',
  ));
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'street' => 'street',
      'city' => 'city',
      'province' => 'province',
      'postal_code' => 'postal_code',
      'country' => 'country',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'street' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'city' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'province' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'postal_code' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'country' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('title', 'Locations');
  $handler->override_option('path', 'location/directory');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Locations',
    'weight' => '0',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;


  $view = new view;
  $view->name = 'user_location_table';
  $view->description = 'User location table.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'users';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE;
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'name' => array(
      'label' => 'Name',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'street' => array(
      'label' => 'Street',
      'style' => 'both',
      'exclude' => 0,
      'id' => 'street',
      'table' => 'location',
      'field' => 'street',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'city' => array(
      'label' => 'City',
      'exclude' => 0,
      'id' => 'city',
      'table' => 'location',
      'field' => 'city',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'province' => array(
      'label' => 'Province',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'province',
      'table' => 'location',
      'field' => 'province',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'postal_code' => array(
      'label' => 'Postal Code',
      'exclude' => 0,
      'id' => 'postal_code',
      'table' => 'location',
      'field' => 'postal_code',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'country' => array(
      'label' => 'Country',
      'style' => 'name',
      'exclude' => 0,
      'id' => 'country',
      'table' => 'location',
      'field' => 'country',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'users',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'lid' => array(
      'operator' => 'not empty',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'lid',
      'table' => 'location',
      'field' => 'lid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view user location table',
  ));
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'name' => 'name',
      'street' => 'street',
      'city' => 'city',
      'province' => 'province',
      'postal_code' => 'postal_code',
      'country' => 'country',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'street' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'city' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'province' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'postal_code' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'country' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'location/users');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'User location table',
    'weight' => '0',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;


  return $views;
}
