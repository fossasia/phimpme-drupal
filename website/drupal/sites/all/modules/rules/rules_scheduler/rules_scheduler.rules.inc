<?php

/**
 * @file
 * Rules integration for the rules scheduler module.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_action_info().
 */
function rules_scheduler_rules_action_info() {
  $items['schedule'] = array(
    'label' => t('Schedule component evaluation'),
    'group' => t('Rules scheduler'),
    'base' => 'rules_scheduler_action_schedule',
    'named parameter' => TRUE,
    'parameter' => array(
      'component' => array(
        'type' => 'text',
        'label' => t('Component'),
        'options list' => 'rules_scheduler_component_options_list',
        'restriction' => 'input',
        'description' => 'Select the component to schedule. Only components containing actions are available – no condition sets.',
      ),
      'date' => array(
        'type' => 'date',
        'label' => t('Scheduled evaluation date'),
      ),
      'identifier' => array(
        'type' => 'text',
        'label' => t('Identifier'),
        'description' => t('A string used for identifying this task. Any existing tasks for this component with the same identifier will be replaced.'),
        'optional' => TRUE,
      ),
      // Further needed parameter by the component are added during processing.
    ),
  );
  // Add action to delete scheduled tasks.
  $items['schedule_delete'] = array(
    'label' => t('Delete scheduled tasks'),
    'group' => t('Rules scheduler'),
    'base' => 'rules_scheduler_action_delete',
    'parameter' => array(
      'component' => array(
        'type' => 'text',
        'label' => t('Component'),
        'options list' => 'rules_scheduler_component_options_list',
        'description' => t('The component for which scheduled tasks will be deleted.'),
        'optional' => TRUE,
      ),
      'task' => array(
        'type' => 'text',
        'label' => t('Task identifier'),
        'description' => t('All tasks that are annotated with the given identifier will be deleted.'),
        'optional' => TRUE,
      ),
    ),
  );
  return $items;
}

/**
 * Options list callback returning a list of action components.
 */
function rules_scheduler_component_options_list() {
  return rules_get_components(TRUE, 'action');
}

/**
 * Base action implementation for scheduling components.
 */
function rules_scheduler_action_schedule($args, $element) {
  $state = $args['state'];
  if ($component = rules_get_cache('comp_' . $args['component'])) {
    // Manually create a new evaluation state for scheduling the evaluation.
    $new_state = new RulesState();

    // Register all parameters as variables.
    foreach ($element->pluginParameterInfo() as $name => $info) {
      if (strpos($name, 'param_') === 0) {
        // Remove the parameter name prefix 'param_'.
        $var_name = substr($name, 6);
        $new_state->addVariable($var_name, $state->currentArguments[$name], $info);
      }
    }
    rules_scheduler_schedule_task(array(
      'date' => $args['date'],
      'config' => $args['component'],
      'state' => $new_state,
      'identifier' => $args['identifier'],
    ));
  }
  else {
    throw new RulesEvaluationException('Unable to get the component %name', array('%name' => $args['component']), $element, RulesLog::ERROR);
  }
}

/**
 * Info alteration callback for the schedule action.
 */
function rules_scheduler_action_schedule_info_alter(&$element_info, RulesPlugin $element) {
  if (isset($element->settings['component'])) {
    // If run during a cache rebuild the cache might not be instantiated yet,
    // so fail back to loading the component from database.
    if (($component = rules_get_cache('comp_' . $element->settings['component'])) || $component = rules_config_load($element->settings['component'])) {
      // Add in the needed parameters.
      foreach ($component->parameterInfo() as $name => $info) {
        $element_info['parameter']['param_' . $name] = $info;
      }
    }
  }
}

/**
 * Validate callback for the schedule action to make sure the component exists and is not dirty.
 *
 * @see rules_element_invoke_component_validate()
 */
function rules_scheduler_action_schedule_validate(RulesPlugin $element) {
  $info = $element->info();
  $component = rules_config_load($element->settings['component']);
  if (!$component) {
    throw new RulesIntegrityException(t('The component %config does not exist.', array('%config' => $element->settings['component'])), $element);
  }
  // Check if the component is marked as dirty.
  rules_config_update_dirty_flag($component);
  if (!empty($component->dirty)) {
    throw new RulesIntegrityException(t('The utilized component %config fails the integrity check.', array('%config' => $element->settings['component'])), $element);
  }
}

/**
 * Help for the schedule action.
 */
function rules_scheduler_action_schedule_help() {
  return t("Note that component evaluation is triggered by <em>cron</em> – make sure cron is configured correctly by checking your site's !status. The scheduling time accuracy depends on your configured cron interval. See <a href='@url'>the online documentation</a> for more information on how to schedule evaluation of components.",
    array('!status' => l('Status report', 'admin/reports/status'),
          '@url' => rules_external_help('scheduler')));
}

/**
 * Form alter callback for the schedule action.
 */
function rules_scheduler_action_schedule_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  $first_step = empty($element->settings['component']);
  $form['reload'] = array(
    '#weight' => 5,
    '#type' => 'submit',
    '#name' => 'reload',
    '#value' => $first_step ? t('Continue') : t('Reload form'),
    '#limit_validation_errors' => array(array('parameter', 'component')),
    '#submit' => array('rules_action_type_form_submit_rebuild'),
    '#ajax' => rules_ui_form_default_ajax(),
  );
  // Use ajax and trigger as the reload button.
  $form['parameter']['component']['settings']['type']['#ajax'] = $form['reload']['#ajax'] + array(
    'event' => 'change',
    'trigger_as' => array('name' => 'reload'),
  );

  if ($first_step) {
    // In the first step show only the component select.
    foreach (element_children($form['parameter']) as $key) {
      if ($key != 'component') {
        unset($form['parameter'][$key]);
      }
    }
    unset($form['submit']);
    unset($form['provides']);
  }
  else {
    // Hide the reload button in case js is enabled and it's not the first step.
    $form['reload']['#attributes'] = array('class' => array('rules-hide-js'));
  }
}

/**
 * Action: Delete scheduled tasks.
 */
function rules_scheduler_action_delete($component_name = NULL, $task_identifier = NULL) {
  $query = db_delete('rules_scheduler');
  if (!empty($component_name)) {
    $query->condition('config', $component_name);
  }
  if (!empty($task_identifier)) {
    $query->condition('identifier', $task_identifier);
  }
  $query->execute();
}

/**
 * Cancel scheduled task action validation callback.
 */
function rules_scheduler_action_delete_validate($element) {
  if (empty($element->settings['task']) && empty($element->settings['task:select']) &&
      empty($element->settings['component']) && empty($element->settings['component:select'])) {

    throw new RulesIntegrityException(t('You have to specify at least either a component or a task identifier.'), $element);
  }
}

/**
 * Help for the cancel action.
 */
function rules_scheduler_action_delete_help() {
  return t('This action allows you to delete scheduled tasks that are waiting for future execution.') .' '. t('They can be addressed by an identifier or by the component name, whereas if both are specified only tasks fulfilling both requirements will be deleted.');
}

/**
 * @}
 */
