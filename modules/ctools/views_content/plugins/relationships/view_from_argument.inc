<?php
// $Id$

/**
 * @file
 * Plugin to provide an relationship handler for a view by argument input settings.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('View From Argument'),
  'description' => t('Creates a view context from argument input settings.'),
  'context' => 'views_content_view_from_argument_context',
  'get child' => 'views_content_view_from_argument_get_child',
  'get children' => 'views_content_view_from_argument_get_children',
);

function views_content_view_from_argument_get_child($plugin, $parent, $child) {
  list($name, $id) = explode('-', $child, 2);
  $view = views_get_view($name);
  if (!$view) {
    return;
  }

  $view->set_display($id);
  if ($view->current_display != $id) {
    return;
  }

  $info = _views_content_get_context_from_display($view, $id, $parent, TRUE);
  if ($info) {
    return $info;
  }
  return;
}

function views_content_view_from_argument_get_children($plugin, $parent) {
  $types = array();

  $views = views_get_applicable_views('returns context');
  foreach ($views as $data) {
    list($view, $id) = $data;
    $info = _views_content_get_context_from_display($view, $id, $parent, TRUE);
    if ($info) {
      $types[$info['name']] = $info;
    }
  }

  return $types;
}

/**
 * Return a new context based on an existing context.
 */
function views_content_view_from_argument_context($contexts, $conf) {
  $name = $conf['name'];
  list($plugin, $view_data) = explode(':', $name);
  list($view_name, $display_id) = explode('-', $view_data);
  $keys = array_keys($conf['context']);

  if (empty($contexts[$keys[0]]->data)) {
    return ctools_context_create_empty('view', NULL);
  }

  // Load our view up.
  $data = views_get_view($view_name);
  if ($data) {
    // Set the display.
    $data->set_display($display_id);
    $context_keys = array_keys($contexts);
    foreach ($data->display_handler->get_argument_input() as $id => $argument) {
      if ($argument['type'] == 'context') {
        $key = array_shift($context_keys);
        if (isset($contexts [$key])) {
          if (strpos($argument['context'], '.')) {
            list($context, $converter) = explode('.', $argument['context'], 2);
            $args[] = ctools_context_convert_context($contexts[$key], $converter);
          }
          else {
            $args[] = $contexts[$key]->argument;
          }
        }
      }
    }
    // remove any trailing NULL arguments as these are non-args:
    while (count($args) && end($args) === NULL) {
      array_pop($args);
    }
    $data->set_arguments($args);
    if ($path = $data->display_handler->get_option('inherit_panels_path')) {
      $data->override_path = $_GET['q'];
    }
  }

  if (isset($contexts[$keys[0]]->data)) {
    return ctools_context_create('view', $data);
  }
}
