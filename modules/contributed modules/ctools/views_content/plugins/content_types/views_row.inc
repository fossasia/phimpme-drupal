<?php

/**
 * @file
 * Allow a view context to display individual rows.
 */

$plugin = array(
  'title' => t('View row'),
  'category' => t('View context'),
  'icon' => 'icon_views_page.png',
  'description' => t('Display all or a specific amount of rows from a loaded view context.'),
  'required context' => new ctools_context_required(t('View'), 'view'),
  'defaults' => array(
    'rows' => array(),
    'use_fields' => array(),
    'fields' => array(),
  ),
  'add form' => array(
    'views_content_views_row_content_type_edit_form' => t('Select context'),
    'views_content_views_row_edit' => t('Configure rows'),
  ),
  'edit form' => array(
    'views_content_views_row_content_type_edit_form' => t('Select context'),
    'views_content_views_row_edit' => t('Configure rows'),
  ),
);

/**
 * Render the node_terms content type.
 */
function views_content_views_row_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return;
  }

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'views_row';
  $block->delta   = $context->argument;
  $block->title   = '';
  $block->content = '';

  // This guarantees the view is rendered normally which must happen.
  $view = views_content_context_get_view($context);
  $output = views_content_context_get_output($context);

  $sets = array();
  $plugin = $view->style_plugin;

  // If all rows have to be displayed then simply get the key of all rows.
  $row_indexes = array();
  if (empty($conf['rows'])) {
    if (is_array($output['rows'])) {
      $row_indexes = array_keys($output['rows']);
    }
  }
  else {
    // If a subset of rows is requested collect the list of row keys.
    foreach ($conf['rows'] as $index) {
      $row_indexes[] = $index - 1;
    }
  }

  if (empty($conf['use_fields']) || empty($plugin->row_plugin)) {
    foreach ($row_indexes as $row_index) {
      if (isset($output['rows'][$row_index])) {
        $sets[$plugin->groups[$row_index]][$row_index] = $output['rows'][$row_index];
      }
    }
  }
  else {
    // If we're using specific fields, go through and poke the 'exclude' flag.
    foreach ($view->field as $id => $field) {
      $view->field[$id]->options['exclude'] = empty($conf['fields'][$id]);
    }

    // Rerender just the rows we need.
    foreach ($row_indexes as $row_index) {
      $view->row_index = $row_index;
      if (!empty($view->result[$view->row_index])) {
        $sets[$plugin->groups[$view->row_index]][$view->row_index] = $plugin->row_plugin->render($view->result[$view->row_index]);
      }
      unset($view->row_index);
    }
  }

  foreach ($sets as $title => $rows) {
    $block->content .= theme($plugin->theme_functions(),
      array(
        'view' => $view,
        'options' => $plugin->options,
        'rows' => $rows,
        'title' => $title
      )
    );
  }

  return $block;
}

function views_content_views_row_content_type_edit_form($form, &$form_state) {
  // This form does nothing; it exists to let the main form select the view context.
  return $form;
}

function views_content_views_row_content_type_edit_form_submit($form, &$form_state) {
}

function views_content_views_row_edit($form, &$form_state) {
  $conf = $form_state['conf'];
  $contexts = $form_state['contexts'];

  if (empty($contexts[$conf['context']])) {
    $form['markup'] = array('#markup' => '<p>' . t('Invalid context selected.') . '</p>');
    return $form;
  }

  if (!isset($contexts[$conf['context']]->argument)) {
    $name = $contexts[$conf['context']]->placeholder['conf']['name'];
    list($plugin, $view_data) = explode(':', $name);
    list($view_name, $display_id) = explode('-', $view_data);
  }
  else {
    $view_data = $contexts[$conf['context']]->argument;
    list($view_name, $display_id) = explode(':', $view_data);
  }
  $contexts[$conf['context']]->data['name'] = $view_name;
  $contexts[$conf['context']]->data['display'] = $display_id;
  $view = views_content_context_get_view($contexts[$conf['context']]);
  if (empty($view)) {
    $form['markup'] = array('#markup' => '<p>' . t('Context contains an invalid view.') . '</p>');
    return $form;
  }

  ctools_include('dependent');
  $form['limit_rows'] = array(
    '#type' => 'checkbox',
    '#title' => t('Limit rows'),
    '#default_value' => (int) !empty($conf['rows']),
  );

  $view->init_pager();
  $rows = $view->get_items_per_page();

  if (!empty($rows)) {
    foreach (range(1, $rows) as $row) {
      $options[$row] = t('Row @number', array('@number' => $row));
    }
    $form['rows'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Select rows'),
      '#options' => $options,
      '#default_value' => $conf['rows'],
      '#dependency' => array('edit-limit-rows' => array(TRUE)),
    );
  }
  else {
    $form['rows'] = array('#markup' => '<p>' . t('The view must have a maximum number of items set to use this setting.') . '</p>');
    return $form;
  }

  if ($view->display_handler->uses_fields()) {
    $form['use_fields'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display specific fields'),
      '#default_value' => $conf['use_fields'],
    );

    $form['fields'] = array(
      '#type' => 'checkboxes',
      '#options' => $view->display_handler->get_field_labels(),
      '#default_value' => $conf['fields'],
      '#prefix' => '<div id="edit-fields-wrapper"><div id="edit-fields">',
      '#suffix' => '</div></div>',
      '#dependency' => array('edit-use-fields' => array(TRUE)),
    );
  }
  return $form;
}

function views_content_views_row_edit_validate(&$form, &$form_state) {
}

function views_content_views_row_edit_submit(&$form, &$form_state) {
  $form_state['conf']['rows'] = array_filter($form_state['values']['rows']);
  $form_state['conf']['use_fields'] = $form_state['values']['use_fields'];
  $form_state['conf']['fields'] = array_filter($form_state['values']['fields']);
}

function views_content_views_row_content_type_admin_info($subtype, $conf, $contexts) {
  // Go through this route to make sure we catch changes in configuration
  // that can happen.
  $plugin = ctools_get_content_type('views_row');
  $context = ctools_content_select_context($plugin, $subtype, $conf, $contexts);

  $block = new stdClass();
  $block->title = t('Row information');

  if (!empty($conf['use_fields'])) {
    $display_fields = array();
    $view = views_content_context_get_view($context);
    if (empty($view)) {
      $block->title = t('Broken view');
      return $block;
    }
    $fields = $view->display_handler->get_field_labels();

    foreach ($conf['fields'] as $field) {
      if (!empty($fields[$field])) {
        $display_fields[$field] = '"<em>' . check_plain($fields[$field]) . '</em>"';
      }
    }

    if ($display_fields) {
      $block->content = t('Displaying: !fields', array('!fields' => implode(', ', $display_fields)));
    }
    else {
      $block->content = t('Displaying no fields due to misconfiguration.');
    }
  }
  else {
    $block->content = t('Displaying the configured row.');
  }

  return $block;
}

function views_content_views_row_content_type_admin_title($subtype, $conf, $context) {
  $rows = array_filter($conf['rows']);
  $rows = empty($rows) ? t('Show all') : implode(', ', $rows);
  return format_plural(count($rows),
    '"@context" row @rows',
    '"@context" rows @rows',
    array('@context' => $context->identifier, '@rows' => $rows)
  );
}
