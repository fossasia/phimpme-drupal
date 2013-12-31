<?php
/**
 * @file
 * Variable API module. Definition for Drupal core variables
 */

/**
 * Implements hook_variable_info().
 */
function forum_variable_info($options) {
  $variables['forum_hot_topic'] = array(
    'title' => t('Hot topic threshold'),
    'type' => 'select_number',
    'default' => 15,
    'options' => array(5, 10, 15, 20, 25, 30, 35, 40, 50, 60, 80, 100, 150, 200, 250, 300, 350, 400, 500),
    'description' => t('The number of replies a topic must have to be considered "hot".'),
    'group' => 'forum_settings',
  );
  $variables['forum_per_page'] = array(
    'title' => t('Topics per page'),
    'type' => 'select_number',
    'default' => 25,
    'options' => array(10, 25, 50, 75, 100),
    'description' => t('Default number of forum topics displayed per page.'),
    'group' => 'forum_settings',
  );
  $forder = array(1 => t('Date - newest first'), 2 => t('Date - oldest first'), 3 => t('Posts - most active first'), 4 => t('Posts - least active first'));
  $variables['forum_order'] = array(
    'title' => t('Default order'),
    'type' => 'select',
    'default' => 1,
    'options' => $forder,
    'description' => t('Default display order for topics.'),
    'group' => 'forum_settings',
  ); 
  // Some hidden variables that we may want exposed, localized, etc..
  $variables['forum_nav_vocabulary'] = array(
    'type' => 'select',
    'options' => 'vocabulary_vid',
    'title' => t('Forum navigation vocabulary'),
    'default' => 0,
    'group' => 'forum_settings',
    'localize' => TRUE,
  );
  $variables['forum_containers'] = array(
    'type' => 'array',
    'title' => t('Forum containers'),
    'group' => 'forum_settings',    
  );
  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function forum_variable_group_info() {
  $groups['forum_settings'] = array(
    'title' => t('Forum settings'),
    'access' => 'administer forums',
    'path' => 'admin/structure/menu/settings',
  );
  return $groups;
}

