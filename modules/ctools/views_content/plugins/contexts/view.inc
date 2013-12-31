<?php

/**
 * @file
 *
 * Plugin to provide a node context. A node context is a node wrapped in a
 * context object that can be utilized by anything that accepts contexts.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("View"),
  'description' => t('Loads a view result into a context that can then be displayed across a panel or turned into other contexts.'),
  'context' => 'views_content_context_view_create',

  'edit form' => 'views_content_context_view_settings_form',
  'edit form validate' => 'views_content_context_view_settings_form_validate',
  'edit form submit' => 'views_content_context_view_settings_form_submit',

  'defaults' => array('view' => ''),

  'keyword' => 'view',
  'context name' => 'view',

  'get child' => 'views_content_context_view_get_child',
  'get children' => 'views_content_context_view_get_children',
);

function views_content_context_view_get_child($plugin, $parent, $child) {
  list($name, $id) = explode('-', $child, 2);
  $view = views_get_view($name);
  if (!$view) {
    return;
  }

  $view->set_display($id);
  if ($view->current_display != $id) {
    return;
  }

  $info = _views_content_get_context_from_display($view, $id, $parent, FALSE);
  if ($info) {
    return $info;
  }
  return;
}

function views_content_context_view_get_children($plugin, $parent) {
  $types = array(
    'view' => $plugin,
  );

  // We're keeping the 'view' context around for legacy reasons but
  // we want to disable the UI so you can't add it that way anymore.
  $types['view']['no ui'] = TRUE;

  $views = views_get_applicable_views('returns context');
  foreach ($views as $data) {
    list($view, $id) = $data;
    $info = _views_content_get_context_from_display($view, $id, $parent, FALSE);
    if ($info) {
      $info['no required context ui'] = TRUE;
      $types[$info['name']] = $info;
    }
  }

  return $types;
}

function views_content_context_view_create($empty, $data = NULL, $conf = FALSE, $plugin = array()) {
  $context = new ctools_context('view');
  $context->plugin = 'view';

  if ($empty) {
    return $context;
  }

  if ($conf) {
    if (is_array($data) && !empty($data['view'])) {
      // This code is left in for backward compatibility. Will not be used
      // with child plugins.
      list($name, $display_id) = explode(':', $data['view'], 2);
      $data = views_get_view($name);
      if ($data) {
        $data->set_display($display_id);
      }
    }
    else if (!empty($plugin['view name'])) {
      $data = views_get_view($plugin['view name']);
      $data->set_display($plugin['view display id']);
    }
  }

  if (is_object($data) && $data->current_display != 'default') {
    // We don't store the loaded view as we don't want the view object
    // cached. However, in order to extract it you can use:
    // @code
    // $output = views_content_context_get_output($context);
    // $view = $output['view'];
    // @endcode
    $context->data     = array(
      'name' => $data->name,
      'display' => $data->current_display,
      'args' => $data->args,
    );

    // At runtime, this can get populated. Once it is populated this
    // object should not be cached.
    $context->view     = NULL;
    $context->title    = $data->get_title();
    $context->argument = $data->name . ':' . $data->current_display;

    $context->restrictions['base'] = array($data->base_table);

    return $context;
  }
}

function views_content_context_view_settings_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $views = views_get_applicable_views('returns context');
  foreach ($views as $data) {
    list($view, $id) = $data;
    $title = views_content_get_display_title($view, $id, 'admin_title');
    $options[$view->name . ':' . $id] = $title;
  }

  if (!empty($options)) {
    natcasesort($options);
    $form['view'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('View'),
    );
  }
  else {
    $form['view'] = array(
      '#value' => '<p>' . t('There are currently no views with Context displays enabled. You should go to the view administration and add a Context display to use a view as a context.') . '</p>',
    );
  }

  return $form;
}

/**
 * Validate a node.
 */
function views_content_context_view_settings_form_validate($form, &$form_state) {
  if (empty($form_state['values']['view'])) {
    form_error($form['view'], t('You must select a view.'));
  }
}

/**
 * Provide a list of ways that this context can be converted to a string.
 */
function views_content_context_view_convert_list() {
  $list = array(
  );

  return $list;
}

/**
 * Convert a context into a string.
 */
function views_content_context_view_convert($context, $type) {
  switch ($type) {
  }
}

