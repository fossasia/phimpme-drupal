<?php

/**
 * @file
 * Overrides the user profile display at user/%user.
 *
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function page_manager_user_edit_page_manager_tasks() {
  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',
    'title' => t('User Edit Template'),
    'admin title' => t('User edit template'),
    'admin description' => t('When enabled, this overrides the default Drupal behavior for displaying user edit form at <em>user/%user/edit</em>.'),
    'admin path' => 'user/%user/edit',

    // Callback to add items to the page managertask administration form:
    'task admin' => 'page_manager_user_edit_task_admin',

    'hook menu' => 'page_manager_user_edit_menu',
    'hook menu alter' => 'page_manager_user_edit_menu_alter',

    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context', // handler type -- misnamed
    'get arguments' => 'page_manager_user_edit_get_arguments',
    'get context placeholders' => 'page_manager_user_edit_get_contexts',

    // Allow this to be enabled or disabled:
    'disabled' => variable_get('page_manager_user_edit_disabled', TRUE),
    'enable callback' => 'page_manager_user_edit_enable',
    'access callback' => 'page_manager_user_edit_access_check',
  );
}

/**
 * Callback defined by page_manager_user_view_page_manager_tasks().
 *
 * Alter the user view input so that user view comes to us rather than the
 * normal user view process.
 */
function page_manager_user_edit_menu_alter(&$items, $task) {
  if (variable_get('page_manager_user_edit_disabled', TRUE)) {
    return;
  }

  // Override the user view handler for our purpose.
  if ($items['user/%user/edit']['page callback'] == 'drupal_get_form' || variable_get('page_manager_override_anyway', FALSE)) {
    $items['user/%user/edit']['page callback'] = 'page_manager_user_edit_page';
    $items['user/%user/edit']['page arguments'] = array(1);
    $items['user/%user/edit']['file path'] = $task['path'];
    $items['user/%user/edit']['file'] = $task['file'];
  }
  else {
    // automatically disable this task if it cannot be enabled.
    variable_set('page_manager_user_edit_disabled', TRUE);
    if (!empty($GLOBALS['page_manager_enabling_user_edit'])) {
      drupal_set_message(t('Page manager module is unable to enable user/%user/edit because some other module already has overridden with %callback.', array('%callback' => $items['user/%user']['page callback'])), 'error');
    }
  }
}

/**
 * Entry point for our overridden user view.
 *
 * This function asks its assigned handlers who, if anyone, would like
 * to run with it. If no one does, it passes through to Drupal core's
 * user edit, which is drupal_get_form('user_profile_form',$account).
 */
function page_manager_user_edit_page($account) {

  // Load my task plugin:
  $task = page_manager_get_task('user_edit');

  // Load the account into a context.
  ctools_include('context');
  ctools_include('context-task-handler');
  $contexts = ctools_context_handler_get_task_contexts($task, '', array($account));
  // Build content. @todo -- this may not be right.
  user_build_content($account);

  $output = ctools_context_handler_render($task, '', $contexts, array($account->uid));
  if (is_array($output)) {
    $output = drupal_render($output);
  }  
  if ($output != FALSE) {
    return $output;
  }

  $function = 'drupal_get_form';
  foreach (module_implements('page_manager_override') as $module) {
    $call = $module . '_page_manager_override';
    if (($rc = $call('user_edit')) && function_exists($rc)) {
      $function = $rc;
      break;
    }
  }

  // Otherwise, fall back.
  if ($function == 'drupal_get_form') {

    //In order to ajax fields to work we need to run form_load_include.
    //Hence we eschew drupal_get_form and manually build the info and
    //call drupal_build_form.
    $form_state = array();
    $form_id = 'user_profile_form';
    $args = array($account);
    $form_state['build_info']['args'] = $args;
    form_load_include($form_state, 'inc', 'user', 'user.pages');
    $output = drupal_build_form($form_id, $form_state);
    return $output;
  }
  //fire off "view" op so that triggers still work
  // @todo -- this doesn't work anymore, and the alternatives seem bad.
  // will have to figure out how to fix this.
  // user_module_invoke('view', $array = array(), $account);
  return $function($account);
}

/**
 * Callback to get arguments provided by this task handler.
 *
 * Since this is the node view and there is no UI on the arguments, we
 * create dummy arguments that contain the needed data.
 */
function page_manager_user_edit_get_arguments($task, $subtask_id) {
  return array(
    array(
      'keyword' => 'user',
      'identifier' => t('User being edited'),
      'id' => 1,
      'name' => 'user_edit',
      'settings' => array(),
    ),
  );
}

/**
 * Callback to get context placeholders provided by this handler.
 */
function page_manager_user_edit_get_contexts($task, $subtask_id) {
  return ctools_context_get_placeholders_from_argument(page_manager_user_edit_get_arguments($task, $subtask_id));
}

/**
 * Callback to enable/disable the page from the UI.
 */
function page_manager_user_edit_enable($cache, $status) {
  variable_set('page_manager_user_edit_disabled', $status);
  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['page_manager_enabling_user_edit'] = TRUE;
  }
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
function page_manager_user_edit_access_check($task, $subtask_id, $contexts) {
  $context = reset($contexts);
  return user_edit_access($context->data);
}
