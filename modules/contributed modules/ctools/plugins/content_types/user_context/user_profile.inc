<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('User profile'),
  'icon' => 'icon_user.png',
  'description' => t('The profile of a user.'),
  'required context' => new ctools_context_required(t('User'), 'user'),
  'category' => t('User'),
  'defaults' => array(
    'view_mode' => 'full',
  ),
);

/**
 * Render the user profile content type.
 */
function ctools_user_profile_content_type_render($subtype, $conf, $panel_args, $context) {
  $account = isset($context->data) ? clone($context->data) : NULL;
  if (!$account || ($account->access == 0 && !user_access('administer users'))) {
    return NULL;
  }

  // Retrieve all profile fields and attach to $account->content.
  if (!isset($account->content)) {
    user_build_content($account, isset($conf['view_mode']) ? $conf['view_mode'] : 'full');
  }

  $build = $account->content;
  // We don't need duplicate rendering info in account->content.
  unset($account->content);

  $build += array(
    '#theme' => 'user_profile',
    '#account' => $account,
    // @todo support view mode
    '#view_mode' => isset($conf['view_mode']) ? $conf['view_mode'] : 'full',
    // @todo do we need to support this?
    '#language' => NULL,
  );

  // Allow modules to modify the structured user.
  $type = 'user';
  drupal_alter(array('user_view', 'entity_view'), $build, $type);

  $block = new stdClass();
  $block->module = 'user-profile';
  $block->title = check_plain(format_username($account));
  $block->content = $build;

  return $block;
}

/**
 * Display the administrative title for a panel pane in the drag & drop UI.
 */
function ctools_user_profile_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" user profile', array('@s' => $context->identifier));
}

function ctools_user_profile_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $entity = entity_get_info('user');
  $view_mode_options = array();
  foreach ($entity['view modes'] as $mode => $option) {
    $view_mode_options[$mode] = $option['label'];
  }

  $form['view_mode'] = array(
    '#title' => t('View mode'),
    '#type' => 'select',
    '#description' => t('Select a build mode for this user.'),
    '#options' => $view_mode_options,
    '#default_value' => isset($conf['view_mode']) ? $conf['view_mode'] : 'full',
  );

  return $form;
}

function ctools_user_profile_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['view_mode'] = $form_state['values']['view_mode'];
}

