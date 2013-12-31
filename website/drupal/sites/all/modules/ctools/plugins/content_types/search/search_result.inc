<?php

if (module_exists('search')) {
  /**
   * Plugins are described by creating a $plugin array which will be used
   * by the system that includes this file.
   */
  $plugin = array(
    'single' => TRUE,
    'title' => t('Search results'),
    'icon' => 'icon_search.png',
    'description' => t('The results of a search using keywords.'),
    'required context' => new ctools_context_required(t('Keywords'), 'string'),
    'category' => t('Widgets'),
    'defaults' => array(
      'type' => 'node',
      'log' => TRUE,
      'override_empty' => FALSE,
      'empty_title' => '',
      'empty' => '',
      'empty_format' => filter_fallback_format(),
      'override_no_key' => FALSE,
      'no_key_title' => '',
      'no_key' => '',
      'no_key_format' => filter_fallback_format(),
    ),
  );
}

/**
 * Render the custom content type.
 */
function ctools_search_result_content_type_render($subtype, $conf, $panel_args, $context) {
  $search_info = search_get_info();
  if (empty($search_info[$conf['type']])) {
    return;
  }
  $info = $search_info[$conf['type']];

  $keys = NULL;
  if (!empty($context) && isset($context->data)) {
    $keys = $context->data;
  }

  $conditions =  NULL;
  if (isset($info['conditions_callback']) && function_exists($info['conditions_callback'])) {
    // Build an optional array of more search conditions.
    $conditions = $info['conditions_callback']($keys);
  }

  // Display nothing at all if no keywords were entered.
  if (empty($keys) && empty($conditions)) {
    if (!empty($conf['override_no_key'])) {
      $block->title = $conf['no_key_title'];
      $block->content = check_markup($conf['no_key'], $conf['no_key_format'], FALSE);
      return $block;
    }
    return;
  }

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'search';
  $block->delta   = 'result';

  $results = '';

  // Only search if there are keywords or non-empty conditions.
  if ($keys || !empty($conditions)) {

    // Collect the search results.
    $results = search_data($keys, $info['module'], $conditions);

    // A workaround for ApacheSolr.
    // @todo see http://drupal.org/node/1343142#comment-5495248
    // This workaround is to be removed when a better one can be written.
    if (!empty($results['search_results']['#results'])) {
      $results['#results'] = $results['search_results']['#results'];
    }
  }

  if (!empty($conf['log'])) {
    // Log the search keys:
    watchdog('search', 'Searched %type for %keys.', array('%keys' => $keys, '%type' => $info['title']), WATCHDOG_NOTICE, l(t('results'), $_GET['q']));
  }

  if (!empty($results['#results'])) {
    $output = "<ol class=\"search-results $conf[type]-results\">\n";
    foreach ($results['#results'] as $result) {
      $output .= theme('search_result', array('result' => $result, 'module' => $conf['type']));
    }
    $output .= "</ol>\n";
    $output .= theme('pager', array('tags' => NULL));

    $block->title = t('Search results');
    $block->content = $output;
  }
  else {
    if (empty($conf['override_empty'])) {
      $block->title = t('Your search yielded no results');
      $block->content = search_help('search#noresults', drupal_help_arg());
    }
    else {
      $block->title = $conf['empty_title'];
      $block->content = check_markup($conf['empty'], $conf['empty_format'], FALSE);
    }
  }

  return $block;
}

/**
 * Returns an edit form for custom type settings.
 */
function ctools_search_result_content_type_edit_form($form, &$form_state) {
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

  $form['log'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['log'],
    '#title' => t('Record a watchdog log entry when searches are made'),
  );

  $form['override_empty'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['override_empty'],
    '#title' => t('Override "no result" text'),
  );

  $form['empty_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $conf['empty_title'],
    '#dependency' => array('edit-override-empty' => array(1)),
  );

  $form['empty_field'] = array(
    '#type' => 'text_format',
    '#title' => t('No result text'),
    '#default_value' => $conf['empty'],
    '#format' => $conf['empty_format'],
    '#dependency' => array('edit-override-empty' => array(1)),
  );

  $form['override_no_key'] = array(
    '#type' => 'checkbox',
    '#default_value' => $conf['override_no_key'],
    '#title' => t('Display text if no search keywords were submitted'),
  );


  $form['no_key_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $conf['no_key_title'],
    '#dependency' => array('edit-override-no-key' => array(1)),
  );

  $form['no_key_field'] = array(
    '#type' => 'text_format',
    '#title' => t('No result text'),
    '#default_value' => $conf['no_key'],
    '#format' => $conf['no_key_format'],
    '#dependency' => array('edit-override-no-key' => array(1)),
  );

  return $form;
}

/**
 * Submit handler for search form.
 */
function ctools_search_result_content_type_edit_form_submit($form, &$form_state) {
  // Copy the text_format values over to where we normally store them.
  $form_state['values']['empty'] = $form_state['values']['empty_field']['value'];
  $form_state['values']['empty_format'] = $form_state['values']['empty_field']['format'];
  $form_state['values']['no_key'] = $form_state['values']['no_key_field']['value'];
  $form_state['values']['no_key_format'] = $form_state['values']['no_key_field']['format'];
  // Copy everything from our defaults.
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

/**
 * Returns the administrative title for a type.
 */
function ctools_search_result_content_type_admin_title($subtype, $conf, $context) {
  $info = search_get_info();
  $type = isset($info[$conf['type']]['title']) ? $info[$conf['type']]['title'] : t('Missing/broken type');
  return t('@type search result', array('@type' => $type));
}
