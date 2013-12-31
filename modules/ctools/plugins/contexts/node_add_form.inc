<?php

/**
 * @file
 *
 * Plugin to provide a node_add_form context
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Node add form'),
  'description' => t('A node add form.'),
  'context' => 'ctools_context_create_node_add_form',
  'edit form' => 'ctools_context_node_add_form_settings_form',
  'defaults' => array('type' => ''),
  'keyword' => 'node_add',
  'context name' => 'node_add_form',
  'convert list' => array('type' => t('Node type')),
  'convert' => 'ctools_context_node_add_form_convert',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' => t('Enter the node type this context.'),
  ),
);

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function ctools_context_create_node_add_form($empty, $data = NULL, $conf = FALSE) {
  static $creating = FALSE;
  $context = new ctools_context(array('form', 'node_add', 'node_form', 'node', 'entity:node'));
  $context->plugin = 'node_add_form';

  if ($empty || ($creating)) {
    return $context;
  }
  $creating = TRUE;

  if ($conf && (isset($data['types']) || isset($data['type']))) {
    // Holdover from typo'd config.
    $data = isset($data['types']) ? $data['types'] : $data['type'];
  }

  if (!empty($data)) {
    $types = node_type_get_types();
    $type = str_replace('-', '_', $data);

    // Validate the node type exists.
    if (isset($types[$type]) && node_access('create', $type)) {
      // Initialize settings:
      global $user;
      $node = (object) array(
        'uid' => $user->uid,
        'name' => (isset($user->name) ? $user->name : ''),
        'type' => $type,
        'language' => LANGUAGE_NONE,
      );

      $form_id = $type . '_node_form';

      $form_state = array(
        'want form' => TRUE,
        'build_info' => array(
          'args' => array($node)
        )
      );

      // Use module_load_include so that caches and stuff can know to load this.
      form_load_include($form_state, 'inc', 'node', 'node.pages');

      $form = drupal_build_form($form_id, $form_state);

      // In a form, $data is the object being edited.
      $context->data     = $node;
      $context->title    = $types[$type]->name;
      $context->argument = $type;

      // These are specific pieces of data to this form.
      // All forms should place the form here.
      $context->form       = $form;
      $context->form_id    = $form_id;
      $context->form_title = t('Submit @name', array('@name' => $types[$type]->name));
      $context->node_type  = $type;
      $context->restrictions['type'] = array($type);
      $context->restrictions['form'] = array('form');

      $creating = FALSE;
      return $context;
    }
  }
  $creating = FALSE;
}

function ctools_context_node_add_form_settings_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['type'] = array(
    '#title' => t('Node type'),
    '#type' => 'select',
    '#options' => node_type_get_names(),
    '#default_value' => $conf['type'],
    '#description' => t('Select the node type for this form.'),
  );

  return $form;
}

function ctools_context_node_add_form_settings_form_submit($form, &$form_state) {
  $form_state['conf']['type'] = $form_state['values']['type'];
}

/**
 * Convert a context into a string.
 */
function ctools_context_node_add_form_convert($context, $type) {
  switch ($type) {
    case 'type':
      return $context->data->type;
  }
}
