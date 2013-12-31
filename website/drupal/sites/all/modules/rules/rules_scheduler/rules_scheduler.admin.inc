<?php

/**
 * @file
 * Admin forms for scheduling.
 */

/**
 * Schedule page with a view for the scheduled tasks.
 */
function rules_scheduler_schedule_page() {
  // Display view for all scheduled tasks
  if (module_exists('views')) {
    // We cannot use views_embed_view() here as we need to set the path for the
    // component filter form.
    $view = views_get_view('rules_scheduler');
    $view->override_path = RULES_SCHEDULER_PATH;
    $task_list = $view->preview();
  }
  else {
    $task_list = t('To display scheduled tasks you have to install the <a href="http://drupal.org/project/views">Views</a> module.');
  }
  $page['task_view'] = array(
    '#markup' => $task_list,
  );
  $form = drupal_get_form('rules_scheduler_form');
  $page['delete'] = array(
    '#markup' => drupal_render($form),
  );
  return $page;
}

/**
 * Form for deletion of tasks by component.
 */
function rules_scheduler_form($form, &$form_state) {
  $result = db_select('rules_scheduler', 'r')
    ->fields('r', array('config'))
    ->distinct()
    ->execute();
  $config_options = array_intersect_key(rules_get_components(TRUE), $result->fetchAllAssoc('config'));

  // Fieldset for canceling by component name.
  $form['delete_by_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delete tasks by component name'),
    '#disabled' => empty($config_options)
  );
  $form['delete_by_config']['config'] = array(
    '#title' => t('Component'),
    '#type' => 'select',
    '#options' => $config_options,
    '#description' => t('Select the component for which to delete all scheduled tasks.'),
    '#required' => TRUE,
  );
  $form['delete_by_config']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete tasks'),
    '#submit' => array('rules_scheduler_form_delete_by_config_submit'),
    );
  return $form;
}

/**
 * Submit handler for deleting future scheduled tasks.
 */
function rules_scheduler_form_delete_by_config_submit($form, &$form_state) {
  $config = rules_config_load($form_state['values']['config']);
  rules_action('schedule_delete')->execute($config->name);
  drupal_set_message(t('All scheduled tasks associated with %config have been deleted.', array('%config' => $config->label())));
  $form_state['redirect'] = RULES_SCHEDULER_PATH;
}

/**
 * Confirmation form for deleting single tasks.
 */
function rules_scheduler_delete_task($form, &$form_state, $task) {
  $form_state['task'] = $task;
  $config = rules_config_load($task['config']);
  $path['path'] = isset($_GET['destination']) ? $_GET['destination'] : RULES_SCHEDULER_PATH;

  $title = t('Are you sure you want to delete the scheduled task %id?', array('%id' => $task['tid']));
  if (!empty($task['identifier'])) {
    $msg = t('This task with the custom identifier %id executes component %label on %date. The action cannot be undone.', array(
      '%label' => $config->label(),
      '%id' => $task['identifier'],
      '%date' => format_date($task['date']),
    ));
  }
  else {
    $msg = t('This task executes component %label and will be executed on %date. The action cannot be undone.', array(
      '%label' => $config->label(),
      '%id' => $task['identifier'],
      '%date' => format_date($task['date']),
    ));
  }
  return confirm_form($form, $title, $path, $msg, t('Delete'), t('Cancel'));
}

/**
 * Submit handler for deleting single tasks.
 */
function rules_scheduler_delete_task_submit($form, &$form_state) {
  rules_scheduler_task_delete($form_state['task']['tid']);
  drupal_set_message(t('Task %tid has been deleted.', array('%tid' => $form_state['task']['tid'])));
  $form_state['redirect'] = RULES_SCHEDULER_PATH;
}

/**
 * Configuration form to manually schedule a rules component.
 */
function rules_scheduler_schedule_form($form, &$form_state, $rules_config, $base_path) {
  // Only components can be scheduled.
  if (!($rules_config instanceof RulesTriggerableInterface)) {
    RulesPluginUI::$basePath = $base_path;
    $form_state['component'] = $rules_config->name;
    $action = rules_action('schedule', array('component' => $rules_config->name));
    $action->form($form, $form_state);
    // The component should be fixed, so hide the paramter for it.
    $form['parameter']['component']['#access'] = FALSE;
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Schedule'),
    );
    $form['#validate'] = array('rules_ui_form_rules_config_validate');
    return $form;
  }
  drupal_not_found();
  exit;
}

/**
 * Submit callback to execute the scheduling action.
 */
function rules_scheduler_schedule_form_submit($form, &$form_state) {
  $action = $form_state['rules_element'];
  $action->execute();
  drupal_set_message(t('Component %label has been scheduled.', array('%label' => rules_config_load($form_state['component'])->label())));
  $form_state['redirect'] = RULES_SCHEDULER_PATH;
}
