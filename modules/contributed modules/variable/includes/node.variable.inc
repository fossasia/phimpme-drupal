<?php
/**
 * @file
 * Variable API module. Definition for Drupal core variables
 */

/**
 * Implements hook_variable_info().
 */
function node_variable_info($options) {
  // Per content type options. Group 'node_type_settings'.
  $variables['teaser_length_[node_type]'] = array(
    'type' => 'multiple',
    'title' => t('Length of trimmed posts'),
    'repeat' => array(
      'type' => 'select',
      'default' => 600,
      'options' => 'text_length',
    ),
    'description' => t("The maximum number of characters used in the trimmed version of a post. Drupal will use this setting to determine at which offset long posts should be trimmed. The trimmed version of a post is typically used as a teaser when displaying the post on the main page, in XML feeds, etc. To disable teasers, set to 'Unlimited'. Note that this setting will only affect new or updated content and will not affect existing teasers.", array(), $options),
    'group' => 'node_type_settings',
  );
  $variables['node_preview_[node_type]'] = array(
    'type' => 'multiple',
    'title' => t('Preview before submitting'),
    'repeat' => array(
      'type' => 'select',
      'default' => DRUPAL_OPTIONAL,
      'options callback' => 'node_variable_option_list',
    ),
    'description' => t('Must users preview posts before submitting?', array(), $options),
    'group' => 'node_type_settings',
  );
  $variables['node_options_[node_type]'] = array(
    'type' => 'multiple',
    'title' => t('Default options', array(), $options),
    'repeat' => array(
      'type' => 'options',
      'default' => array('status', 'promote'),
      'options callback' => 'node_variable_option_list',
    ),
    'description' => t('Users with the <em>administer nodes</em> permission will be able to override these options.', array(), $options),
    'group' => 'node_type_settings',
  );
  $variables['node_submitted_[node_type]'] = array(
    'type' => 'multiple',
    'title' => t('Display author and date information.', array(), $options),
    'repeat' => array(
      'default' => TRUE,
      'type' => 'boolean',
    ),
    'description' => t('Author username and publish date will be displayed.', array(), $options),
    'group' => 'node_type_settings',
  );
  return $variables;
}


/**
 * Implements hook_variable_group_info().
 */
function node_variable_group_info() {
  $groups['node_type_settings'] = array(
    'title' => t('Node type settings'),
    'description' => t('Settings for each node type.'),
    'access' => 'administer nodes',
    'path' => 'admin/structure/types', 
  );
  return $groups;
}

/**
 * Implements hook_variable_type_info()
 */
function node_variable_type_info() {
  $type['node_type'] = array(
    'title' => t('Node type'),
    'options callback' => 'node_type_get_names',
    'type' => 'select',
  );
  $type['text_length'] = array(
    'title' => t('Text length'),
    'options callback' => 'node_variable_option_text_length',
    'type' => 'select',
  );
  return $type;
}

/**
 * Callback for node variable options
 */
function node_variable_option_text_length($variable, $options = array()) {
  return array(
    0 => t('Unlimited', array(), $options),
    200 => t('200 characters', array(), $options),
    400 => t('400 characters', array(), $options),
    600 => t('600 characters', array(), $options),
    800 => t('800 characters', array(), $options),
    1000 => t('1000 characters', array(), $options),
    1200 => t('1200 characters', array(), $options),
    1400 => t('1400 characters', array(), $options),
    1600 => t('1600 characters', array(), $options),
    1800 => t('1800 characters', array(), $options),
    2000 => t('2000 characters', array(), $options),
  );  
}

/**
 * Callback for variable options
 */
function node_variable_option_list($variable, $options = array()) {
  switch ($variable['parent']) {
    case 'node_preview_[node_type]':
      return array(
        DRUPAL_DISABLED => t('Disabled', array(), $options),
        DRUPAL_OPTIONAL => t('Optional', array(), $options),
        DRUPAL_REQUIRED => t('Required', array(), $options),
      );
    case 'node_options_[node_type]':
      return array(
        'status' => t('Published', array(), $options),
        'promote' => t('Promoted to front page', array(), $options),
        'sticky' => t('Sticky at top of lists', array(), $options),
        'revision' => t('Create new revision', array(), $options),     
      );
  }
}

/**
 * Build subform for variables for node type
 * 
 * @param $type
 *   Node type name
 * @param $list
 *   List of variables to include
 */
function node_variable_type_subform($type, $list) {
  module_load_include('form.inc', 'variable');
  foreach ($list as $name) {
    $variable = variable_get_child($name . '_[node_type]', $type);
    $form[$name] = variable_form_element($variable);  
  }
  return $form;
}