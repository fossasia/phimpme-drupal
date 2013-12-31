<?php

if (module_exists('search')) {
  /**
   * Plugins are described by creating a $plugin array which will be used
   * by the system that includes this file.
   */
  $plugin = array(
    'single' => TRUE,
    'title' => t('Advanced search form'),
    'icon' => 'icon_search.png',
    'description' => t('A search form with advanced options.'),
    'required context' => new ctools_context_optional(t('Keywords'), 'string'),
    'category' => t('Widgets'),
    'defaults' => array(
      'type' => 'node',
      'form' => 'advanced',
      'path_type' => 'default',
      'path' => '',
      'override_prompt' => FALSE,
      'prompt' => '',
    ),
  );
}

/**
 * Render the custom content type.
 */
function ctools_search_form_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    $keys = '';
  }
  else {
    $keys = $context->data;
  }

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'search';
  $block->delta   = 'form';
  $block->title   = '';

  switch ($conf['path_type']) {
    default:
    case 'default':
      $path = 'search/' . $conf['type'];
      break;
    case 'same':
      $path = $_GET['q'];
      $path = str_replace($keys, '', $path);
      break;
    case 'custom':
      $path = $conf['path'];
      break;
  }

  $prompt = $conf['override_prompt'] ? $conf['prompt'] : NULL;

  $form_state = array(
    'build_info' => array(
      'args' => array($path, $keys, $conf['type'], $prompt),
    ),
  );

  module_load_include('inc', 'search', 'search.pages');

  $block->content = drupal_build_form('search_form', $form_state);
  if ($conf['form'] == 'simple' && isset($block->content['advanced'])) {
    $block->content['advanced']['#access'] = FALSE;
  }

  return $block;
}

/**
 * Returns an edit form for custom type settings.
 */
function ctools_search_form_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $types = array();
  foreach (search_get_info() as $module => $info) {
    $types[$module] = $info['title'];
  }

  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Search type'),
    '#options' => $types,
    '#default_value' => $conf['type'],
  );

  $form['form'] = array(
    '#type' => 'select',
    '#title' => t('Search form'),
    '#options' => array(
      'simple' => t('Simple'),
      'advanced' => t('Advanced'),
    ),
    '#default_value' => $conf['form'],
    '#description' => t('The advanced form may have additional options based upon the search type. For example the advanced content (node) search form will allow searching by node type and taxonomy term.'),
  );

  $form['path_type'] = array(
    '#prefix' => '<div class="container-inline">',
    '#type' => 'select',
    '#title' => t('Path'),
    '#options' => array(
      'default' => t('Default'),
      'same' => t('Same page'),
      'custom' => t('Custom'),
    ),
    '#default_value' => $conf['path_type'],
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['path'],
    '#dependency' => array('edit-path-type' => array('custom')),
    '#suffix' => '</div>',
  );

  $form['override_prompt'] = array(
    '#prefix' => '<div class="container-inline">',
    '#type' => 'checkbox',
    '#default_value' => $conf['override_prompt'],
    '#title' => t('Override default prompt'),
  );

  $form['prompt'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['prompt'],
    '#dependency' => array('edit-override-prompt' => array(1)),
    '#suffix' => '</div>',
  );
  return $form;
}

/**
 * Submit handler for search form.
 */
function ctools_search_form_content_type_edit_form_submit($form, &$form_state) {
  // Copy everything from our defaults.
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

/**
 * Returns the administrative title for a type.
 */
function ctools_search_form_content_type_admin_title($subtype, $conf, $context) {
  $info = search_get_info();
  $type = isset($info[$conf['type']]['title']) ? $info[$conf['type']]['title'] : t('Missing/broken type');
  return t('@type search form', array('@type' => $type));
}
