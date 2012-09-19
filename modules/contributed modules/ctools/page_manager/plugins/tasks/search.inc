<?php

/**
 * @file
 * Handle the 'node view' override task.
 *
 * This plugin overrides node/%node and reroutes it to the page manager, where
 * a list of tasks can be used to service this request based upon criteria
 * supplied by access plugins.
 */

/**
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function page_manager_search_page_manager_tasks() {
  if (!module_exists('search')) {
    return;
  }

  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',
    'title' => t('Search'),

    // There are multiple search pages, let's override each of them
    // separately.
    'subtasks' => TRUE,
    'subtask callback' => 'page_manager_search_subtask',
    'subtasks callback' => 'page_manager_search_subtasks',

    // Menu hooks so that we can alter the node/%node menu entry to point to us.
    'hook menu alter' => 'page_manager_search_menu_alter',

    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context',
    'get arguments' => 'page_manager_search_get_arguments',
    'get context placeholders' => 'page_manager_search_get_contexts',
    'access callback' => 'page_manager_search_access_check',

  );
}

/**
 * Callback defined by page_manager_search_page_manager_tasks().
 *
 * Alter the search tabs to work with page manager. The search flow is
 * quite odd, and tracing through the code takes hours to realize
 * that the tab you click on does not normally actually handle
 * the search. This tries to account for that.
 *
 * Note to module authors: This tends to work a lot better with modules
 * that override their own search pages if their _alter runs *before*
 * this one.
 */
function page_manager_search_menu_alter(&$items, $task) {
  // We are creating two sets of tabs. One set is for searching without
  // keywords. A second set is for searching *with* keywords. This
  // is necessary because search/node/% and search/node need to be
  // different due to the way the search menu items function.

  $default_info = search_get_default_module_info();
  if (empty($default_info)) {
    // Nothing to do.
    return;
  }

  // Go through each search module item.
  foreach (search_get_info() as $module => $info) {
    if (variable_get('page_manager_search_disabled_' . $module, TRUE)) {
      continue;
    }

    $path = 'search/' . $info['path'];
    $callback = $items["$path/%menu_tail"]['page callback'];

    if ($callback == 'search_view' || variable_get('page_manager_override_anyway', FALSE)) {
      $items["$path"]['page callback'] = 'page_manager_search_page';
      $items["$path"]['file path'] = $task['path'];
      $items["$path"]['file'] = $task['file'];

      $items["$path/%menu_tail"]['page callback'] = 'page_manager_search_page';
      $items["$path/%menu_tail"]['file path'] = $task['path'];
      $items["$path/%menu_tail"]['file'] = $task['file'];
    }
    else {
      // automatically disable this task if it cannot be enabled.
      variable_set('page_manager_search_disabled_' . $module, TRUE);
      if (!empty($GLOBALS['page_manager_enabling_search'])) {
        drupal_set_message(t('Page manager module is unable to enable @path because some other module already has overridden with %callback.', array('%callback' => $callback, '@path' => $path)), 'error');
      }
    }
  }
}

/**
 * Entry point for our overridden search page.
 *
 */
function page_manager_search_page($type) {
  ctools_include('menu');
//  menu_set_active_trail(ctools_get_menu_trail('search/' . $type));

  // Get the arguments and construct a keys string out of them.
  $args = func_get_args();

  // We have to remove the $type.
  array_shift($args);

  // And implode() it all back together.
  $keys = $args ? implode('/', $args) : '';

  // Allow other modules to alter the search keys
  drupal_alter(array('search_keys', 'search_'. $type .'_keys'), $keys);

  // Load my task plugin
  $task = page_manager_get_task('search');
  $subtask = page_manager_get_task_subtask($task, $type);

  // Load the node into a context.
  ctools_include('context');
  ctools_include('context-task-handler');
  $contexts = ctools_context_handler_get_task_contexts($task, $subtask, array($keys));

  $output = ctools_context_handler_render($task, $subtask, $contexts, array($keys));
  if ($output !== FALSE) {
    return $output;
  }

  $function = 'search_view';
  foreach (module_implements('page_manager_override') as $module) {
    $call = $module . '_page_manager_override';
    if (($rc = $call('search')) && function_exists($rc)) {
      $function = $rc;
      break;
    }
  }

  // Otherwise, fall back.

  // Put the $type back on the arguments.
  module_load_include('inc', 'search', 'search.pages');
  array_unshift($args, $type);
  return call_user_func_array($function, $args);
}

/**
 * Callback to get arguments provided by this task handler.
 *
 * Since this is the node view and there is no UI on the arguments, we
 * create dummy arguments that contain the needed data.
 */
function page_manager_search_get_arguments($task, $subtask_id) {
  return array(
    array(
      'keyword' => 'keywords',
      'identifier' => t('Keywords'),
      'id' => 1,
      'name' => 'string',
      'settings' => array('use_tail' => TRUE),
    ),
  );
}

/**
 * Callback to get context placeholders provided by this handler.
 */
function page_manager_search_get_contexts($task, $subtask_id) {
  return ctools_context_get_placeholders_from_argument(page_manager_search_get_arguments($task, $subtask_id));
}

/**
 * Callback to enable/disable the page from the UI.
 */
function page_manager_search_enable($cache, $status) {
  variable_set('page_manager_search_disabled_' . $cache->subtask_id, $status);

  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['page_manager_enabling_search'] = TRUE;
  }
}

/**
 * Task callback to get all subtasks.
 *
 * Return a list of all subtasks.
 */
function page_manager_search_subtasks($task) {
  $return = array();
  foreach (search_get_info() as $module => $info) {
    if ($info['path']) {
      // We don't pass the $info because the subtask build could be called
      // singly without the $info when just the subtask needs to be built.
      $return[$module] = page_manager_search_build_subtask($task, $module);
    }
  }

  return $return;
}

/**
 * Callback to return a single subtask.
 */
function page_manager_search_subtask($task, $subtask_id) {
  return page_manager_search_build_subtask($task, $subtask_id);
}

/**
 * Build a subtask array for a given page.
 */
function page_manager_search_build_subtask($task, $module) {
  $search_info = search_get_info();
  $info = $search_info[$module];
  $path = 'search/' . $info['path'];
  $subtask = array(
    'name' => $module,
    'admin title' => $info['title'],
    'admin path' => "$path/!keywords",
    'admin description' => t('Search @type', array('@type' => $info['title'])),
    'admin type' => t('System'),
    'row class' => empty($page->disabled) ? 'page-manager-enabled' : 'page-manager-disabled',
    'storage' => t('In code'),
    'disabled' => variable_get('page_manager_search_disabled_' . $module, TRUE),
    // This works for both enable AND disable
    'enable callback' => 'page_manager_search_enable',
  );

  return $subtask;
}

/**
 * Callback to determine if a page is accessible.
 *
 * @param $task
 *   The task plugin.
 * @param $subtask_id
 *   The subtask id
 * @param $contexts
 *   The contexts loaded for the task.
 * @return
 *   TRUE if the current user can access the page.
 */
function page_manager_search_access_check($task, $subtask_id, $contexts) {
  $context = reset($contexts);
  return _search_menu_access($context->data);
}
