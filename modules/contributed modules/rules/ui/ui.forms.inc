<?php

/**
 * @file Rules UI forms
 */

/**
 * Ajax callback for reloading the whole form.
 */
function rules_ui_form_ajax_reload_form($form, $form_state) {
  return $form;
}

/**
 * Defines #ajax properties.
 */
function rules_ui_form_default_ajax($effect = 'slide') {
  return array(
    'callback' => 'rules_ui_form_ajax_reload_form',
    'wrapper' => 'rules-form-wrapper',
    'effect' => $effect,
    'speed' => 'fast',
  );
}

/**
 * Submit handler for switching the parameter input mode.
 */
function rules_ui_parameter_replace_submit($form, &$form_state) {
  if (isset($form_state['triggering_element'])) {
    $name = $form_state['triggering_element']['#parameter'];
    $form_state['parameter_mode'][$name] = $form_state['parameter_mode'][$name] == 'selector' ? 'input' : 'selector';
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * General form submit handler, that rebuilds the form
 */
function rules_form_submit_rebuild($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Edit a rules configuration.
 */
function rules_ui_form_edit_rules_config($form, &$form_state, $rules_config, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  $form_state += array('rules_element' => $rules_config);
  // Add the rule configuration's form.
  $rules_config->form($form, $form_state, array('show settings' => TRUE, 'button' => TRUE));
  $form['#validate'] = array('rules_ui_form_rules_config_validate');
  return $form;
}

/**
 * General rules configuration form validation callback. Also populates the
 * rules configuration with the form values.
 */
function rules_ui_form_rules_config_validate($form, &$form_state) {
  $form_state['rules_element']->form_validate($form, $form_state);
}

/**
 * Edit a rules configuration form submit callback.
 */
function rules_ui_form_edit_rules_config_submit($form, &$form_state) {
  $form_state['rules_element']->form_submit($form, $form_state);
  drupal_set_message(t('Your changes have been saved.'));
  if (empty($form_state['redirect'])) {
    $form_state['redirect'] = RulesPluginUI::defaultRedirect($form_state['rules_element']);
  }
}

/**
 * Clone a rules configuration form.
 */
function rules_ui_form_clone_rules_config($form, &$form_state, $rules_config, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  $rules_config = clone $rules_config;
  $rules_config->module = 'rules';
  $rules_config->id = NULL;
  $rules_config->name = '';
  $rules_config->label .= ' (' . t('cloned') . ')';
  $rules_config->status = ENTITY_CUSTOM;

  $form['#validate'][] = 'rules_ui_form_rules_config_validate';
  $form['#submit'][] = 'rules_ui_form_edit_rules_config_submit';
  $form_state += array('rules_element' => $rules_config, 'op' => 'clone');

  // Add the rule configuration's form.
  $rules_config->form($form, $form_state, array('show settings' => TRUE, 'button' => TRUE, 'init' => TRUE));

  // Open the settings fieldset so altering the name is easier.
  $form['settings']['#collapsed'] = FALSE;
  return $form;
}

/**
 * A simple form just showing a textarea with the export.
 */
function rules_ui_form_export_rules_config($form, &$form_state, $rules_config, $base_path) {
  $form['export'] = array(
    '#type' => 'textarea',
    '#title' => t('Export'),
    '#description' => t('For importing copy the content of the text area and paste it into the import page.'),
    '#rows' => 25,
    '#default_value' => $rules_config->export(),
  );
  return $form;
}

/**
 * Configuration form to directly execute a rules configuration.
 */
function rules_ui_form_execute_rules_config($form, &$form_state, $rules_config, $base_path) {
  // Only components can be executed.
  if (!($rules_config instanceof RulesTriggerableInterface)) {
    RulesPluginUI::$basePath = $base_path;
    // Create either the appropriate action or condition element.
    $element = rules_plugin_factory($rules_config instanceof RulesActionInterface ? 'action' : 'condition', 'component_' . $rules_config->name);
    $form['exec_help'] = array(
      '#prefix' => '<p>',
      '#markup' => t('This form allows you to manually trigger the execution of the @plugin "%label". If this component requires any parameters, input the suiting execution arguments below.', array('@plugin' => $rules_config->plugin(), '%label' => $rules_config->label())),
      '#suffix' => '</p>',
    );
    $element->form($form, $form_state);

    // For conditions hide the option to negate them.
    if (isset($form['negate'])) {
      $form['negate']['#access'] = FALSE;
    }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Execute'),
      '#weight' => 20,
    );
    // Re-use the validation callback, which will also populate the action with
    // the configuration settings in the form.
    $form['#validate'] = array('rules_ui_form_rules_config_validate');
    return $form;
  }
  drupal_not_found();
  exit;
}

/**
 * Submit callback for directly executing a component.
 */
function rules_ui_form_execute_rules_config_submit($form, &$form_state) {
  $element = $form_state['rules_element'];
  $result = $element->execute();
  if ($element instanceof RulesActionInterface) {
    drupal_set_message(t('Component %label has been executed.', array('%label' => $element->label())));
  }
  else {
    drupal_set_message(t('Component %label evaluated to %result.', array('%label' => $element->label(), '%result' => $result ? 'true' : 'false')));
  }
}

/**
 * Gets the confirmation question for valid operations, or else FALSE.
 */
function rules_ui_confirm_operations($op, $rules_config) {
  $vars = array('%plugin' => $rules_config->plugin(), '%label' => $rules_config->label());

  switch ($op) {
    case 'enable':
      return array(t('Are you sure you want to enable the %plugin %label?', $vars), '');
    case 'disable':
      return array(t('Are you sure you want to disable the %plugin %label?', $vars), '');
    case 'revert':
      return array(t('Are you sure you want to revert the %plugin %label?', $vars), t('This action cannot be undone.'));
    case 'delete':
      return array(t('Are you sure you want to delete the %plugin %label?', $vars), t('This action cannot be undone.'));
    default:
      return FALSE;
  }
}

/**
 * Confirmation form for applying the operation to the config.
 */
function rules_ui_form_rules_config_confirm_op($form, &$form_state, $rules_config, $op, $base_path) {
  if (list($confirm_question, $description) = rules_ui_confirm_operations($op, $rules_config)) {
    RulesPluginUI::$basePath = $base_path;
    $form_state += array('rules_config' => $rules_config, 'op' => $op);
    return confirm_form($form, $confirm_question, $base_path, $description, t('Confirm'), t('Cancel'));
  }
  else {
    drupal_not_found();
    exit;
  }
}

/**
 * Applies the operation and returns the message to show to the user. Also the
 * operation is logged to the watchdog. Note that the string is defined two
 * times so that the translation extractor can find it.
 */
function rules_ui_confirm_operation_apply($op, $rules_config) {
  $vars = array('%plugin' => $rules_config->plugin(), '%label' => $rules_config->label());
  $edit_link = l(t('edit'), RulesPluginUI::path($rules_config->name));

  switch ($op) {
    case 'enable':
      $rules_config->active = TRUE;
      $rules_config->save();
      watchdog('rules', 'Enabled %plugin %label.', $vars, WATCHDOG_NOTICE, $edit_link);
      return t('Enabled %plugin %label.', $vars);

    case 'disable':
      $rules_config->active = FALSE;
      $rules_config->save();
      watchdog('rules', 'Disabled %plugin %label.', $vars, WATCHDOG_NOTICE, $edit_link);
      return t('Disabled %plugin %label.', $vars);

    case 'revert':
      $rules_config->delete();
      watchdog('rules', 'Reverted %plugin %label to the defaults.', $vars, WATCHDOG_NOTICE, $edit_link);
      return t('Reverted %plugin %label to the defaults.', $vars);

    case 'delete':
      $rules_config->delete();
      watchdog('rules', 'Deleted %plugin %label.', $vars);
      return t('Deleted %plugin %label.', $vars);
  }
}

/**
 * Rule config deletion form submit callback.
 */
function rules_ui_form_rules_config_confirm_op_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $msg = rules_ui_confirm_operation_apply($form_state['op'], $form_state['rules_config']);
    drupal_set_message($msg);
  }
}

/**
 * Add a new element a rules configuration.
 */
function rules_ui_add_element($form, &$form_state, $rules_config, $plugin_name, RulesContainerPlugin $parent, $base_path) {
  $cache = rules_get_cache();
  if (!isset($cache['plugin_info'][$plugin_name]['class'])) {
    drupal_not_found();
    exit;
  }
  RulesPluginUI::$basePath = $base_path;
  $plugin_is_abstract = in_array('RulesAbstractPlugin', class_parents($cache['plugin_info'][$plugin_name]['class']));
  // In the first step create the element and in the second step show its edit
  // form.
  if ($plugin_is_abstract && !isset($form_state['rules_element'])) {
    RulesPluginUI::formDefaults($form, $form_state);
    $form_state += array('parent_element' => $parent, 'plugin' => $plugin_name);

    $form['element_name'] = array(
      '#type' => 'select',
      '#title' => t('Select the %element to add', array('%element' => $plugin_name)),
      '#options' => RulesPluginUI::getOptions($plugin_name),
      '#ajax' => rules_ui_form_default_ajax() + array(
        'trigger_as' => array('name' => 'continue'),
      ),
    );
    $form['continue'] = array(
      '#type' => 'submit',
      '#name' => 'continue',
      '#value' => t('Continue'),
      '#ajax' => rules_ui_form_default_ajax(),
    );
  }
  elseif (!$plugin_is_abstract) {
    // Create the initial, empty element.
    $element = rules_plugin_factory($plugin_name);
    // Always add the new element at the bottom, thus set an appropriate weight.
    $iterator = $parent->getIterator();
    if ($sibling = end($iterator)) {
      $element->weight = $sibling->weight + 1;
    }
    $element->setParent($parent);
    $form_state['rules_element'] = $element;
  }

  if (isset($form_state['rules_element'])) {
    $form_state['rules_element']->form($form, $form_state, array('button' => TRUE, 'init' => TRUE));
    $form['#validate'][] = 'rules_ui_edit_element_validate';
    $form['#submit'][] = 'rules_ui_edit_element_submit';
  }
  return $form;
}

/**
 * Add element submit callback.
 * Used for "abstract plugins" to create the initial element object with the
 * given implemenation name and rebuild the form.
 */
function rules_ui_add_element_submit($form, &$form_state) {
  $element = rules_plugin_factory($form_state['plugin'], $form_state['values']['element_name']);

  // Always add the new element at the bottom, thus set an appropriate weight.
  $iterator = $form_state['parent_element']->getIterator();
  if ($sibling = end($iterator)) {
    $element->weight = $sibling->weight + 1;
  }
  // Clear the element settings so they won't be processed on serialization as
  // there is nothing to be processed yet.
  $element->settings = array();
  $element->setParent($form_state['parent_element']);

  $form_state['rules_element'] = $element;
  $form_state['rebuild'] = TRUE;
}

/**
 * Delete elements.
 */
function rules_ui_delete_element($form, &$form_state, $rules_config, $rules_element, $base_path) {
  RulesPluginUI::$basePath = $base_path;

  if (empty($form_state['rules_config'])) {
    // Before modifying the rules config we have to clone it, so any
    // modifications won't appear in the static cache of the loading controller.
    $rules_config = clone $rules_config;
    // Also get the element from the cloned config.
    $rules_element = $rules_config->elementMap()->lookup($rules_element->elementId());

    $form_state['rules_config'] = $rules_config;
    $form_state['rules_element'] = $rules_element;
    $form_state['element_parent'] = $rules_element->parentElement();
  }

  // Try deleting the element and warn the user if something breaks, but
  // save the parent for determining the right redirect target on submit.
  $removed_plugin = $form_state['rules_element']->plugin();
  $rules_element->delete();

  if (empty($rules_config->dirty) && empty($form_state['input'])) {
    try {
      $rules_config->integrityCheck();
    }
    catch (RulesIntegrityException $e) {
      $args = array(
        '@plugin' => $e->element->plugin(),
        '%label' => $e->element->label(),
        '@removed-plugin' => $removed_plugin,
        '!url' => url(RulesPluginUI::path($form_state['rules_config']->name, 'edit', $e->element)),
      );
      drupal_set_message(t('Deleting this @removed-plugin would break your configuration as some of its provided variables are utilized by the @plugin <a href="!url">%label</a>.', $args), 'warning');
    }
  }

  $confirm_question = t('Are you sure you want to delete the %element_plugin %element_name?', array('%element_plugin' => $rules_element->plugin(), '%element_name' => $rules_element->label(), '%plugin' => $rules_config->plugin(), '%label' => $rules_config->label()));
  return confirm_form($form, $confirm_question, RulesPluginUI::path($rules_config->name), t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Rule config deletion form submit callback.
 */
function rules_ui_delete_element_submit($form, &$form_state) {
  $rules_config = $form_state['rules_config'];
  $rules_config->save();
  if (empty($form_state['redirect'])) {
    $form_state['redirect'] = RulesPluginUI::defaultRedirect($form_state['element_parent']);
  }
}


/**
 * Configure a rule element.
 */
function rules_ui_edit_element($form, &$form_state, $rules_config, $element, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  $form_state += array('rules_element' => $element);
  $form_state['rules_element']->form($form, $form_state, array('button' => TRUE));
  return $form;
}

/**
 * Validate the element configuration.
 */
function rules_ui_edit_element_validate($form, &$form_state) {
  $form_state['rules_element']->form_validate($form, $form_state);
}

/**
 * Submit the element configuration.
 */
function rules_ui_edit_element_submit($form, &$form_state) {
  $form_state['rules_element']->form_submit($form, $form_state);
  drupal_set_message(t('Your changes have been saved.'));
  if (empty($form_state['redirect'])) {
    $form_state['redirect'] = RulesPluginUI::defaultRedirect($form_state['rules_element']);
  }
}

/**
 * Add a new event.
 */
function rules_ui_add_event($form, &$form_state, RulesReactionRule $rules_config, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  $form_state += array('rules_config' => $rules_config);
  $events = array_diff_key(rules_fetch_data('event_info'), array_flip($rules_config->events()));

  $form['help'] = array(
    '#markup' => t('Select the event to add. However note that all added events need to provide all variables that should be available to your rule.'),
  );
  $form['event'] = array(
    '#type' => 'select',
    '#title' => t('React on event'),
    '#options' => RulesPluginUI::getOptions('event', $events),
    '#description' => t('Whenever the event occurs, rule evaluation is triggered.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form_state['redirect'] = RulesPluginUI::path($rules_config->name);
  return $form;
}

/**
 * Submit callback that just adds the selected event.
 *
 * @see rules_admin_add_reaction_rule()
 */
function rules_ui_add_event_apply($form, &$form_state) {
  $form_state['rules_config']->event($form_state['values']['event']);
}

/**
 * Submit the event configuration.
 */
function rules_ui_add_event_submit($form, &$form_state) {
  rules_ui_add_event_apply($form, $form_state);
  $rules_config = $form_state['rules_config'];

  // Tell the user if this breaks something, but let him proceed.
  if (empty($rules_config->dirty)) {
    try {
      $rules_config->integrityCheck();
    }
    catch (RulesIntegrityException $e) {
      $warning = TRUE;
      drupal_set_message(t('Added the event, but it does not provide all variables utilized.'), 'warning');
    }
  }
  $rules_config->save();
  if (!isset($warning)) {
    $events = rules_fetch_data('event_info');
    $label = $events[$form_state['values']['event']]['label'];
    drupal_set_message(t('Added event %event.', array('%event' => $label)));
  }
}

/**
 * Form to remove a event from a rule.
 */
function rules_ui_remove_event($form, &$form_state, $rules_config, $event, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  $form_state += array('rules_config' => $rules_config, 'rules_event' => $event);
  $events = rules_fetch_data('event_info');
  $form_state['event_label'] = $events[$event]['label'];
  $confirm_question = t('Are you sure you want to remove the event?');
  return confirm_form($form, $confirm_question, RulesPluginUI::path($rules_config->name), t('You are about to remove the event %event.', array('%event' => $form_state['event_label'])), t('Remove'), t('Cancel'));
}

/**
 * Submit the event configuration.
 */
function rules_ui_remove_event_submit($form, &$form_state) {
  $rules_config = $form_state['rules_config'];
  $rules_config->removeEvent($form_state['rules_event']);
  // Tell the user if this breaks something, but let him proceed.
  if (empty($rules_config->dirty)) {
    try {
      $rules_config->integrityCheck();
    }
    catch (RulesIntegrityException $e) {
      $warning = TRUE;
      drupal_set_message(t('Removed the event, but it had provided some variables which are now missing.'), 'warning');
    }
  }
  $rules_config->save();
  if (!isset($warning)) {
    drupal_set_message(t('Event %event has been removed.', array('%event' => $form_state['event_label'])));
  }
  $form_state['redirect'] = RulesPluginUI::path($rules_config->name);
}

/**
 * Import form for rule configurations.
 */
function rules_ui_import_form($form, &$form_state, $base_path) {
  RulesPluginUI::$basePath = $base_path;
  RulesPluginUI::formDefaults($form, $form_state);
  $form['import'] = array(
    '#type' => 'textarea',
    '#title' => t('Import'),
    '#description' => t('Paste an exported Rules configuration here.'),
    '#rows' => 20,
  );
  $form['overwrite'] = array(
    '#title' => t('Overwrite'),
    '#type' => 'checkbox',
    '#description' => t('If checked, any existing configuration with the same identifier will be replaced by the import.'),
    '#default_value' => FALSE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * Validation callback for the import form.
 */
function rules_ui_import_form_validate($form, &$form_state) {
  if ($rules_config = rules_import($form_state['values']['import'], $error_msg)) {
    // Store the successfully imported entity in $form_state.
    $form_state['rules_config'] = $rules_config;
    if (!$form_state['values']['overwrite']) {
      // Check for existing entities with the same identifier.
      if (rules_config_load($rules_config->name)) {
        $vars = array('@entity' => t('Rules configuration'), '%label' => $rules_config->label());
        form_set_error('import', t('Import of @entity %label failed, a @entity with the same machine name already exists. Check the overwrite option to replace it.', $vars));
      }
    }
    try {
      $rules_config->integrityCheck();
    }
    catch (RulesIntegrityException $e) {
      form_set_error('import', t('Integrity check for the imported configuratoin failed. Error message: %message.', array('%message' => $e->getMessage())));
    }
    if (!user_access('bypass rules access') && !$rules_config->access()) {
      form_set_error('import', t('You have insufficient access permissions for importing this Rules configuration.'));
    }
  }
  else {
    form_set_error('import', t('Import failed.'));
    if ($error_msg) {
      drupal_set_message($error_msg, 'error');
    }
  }
}

/**
 * Submit callback for the import form.
 */
function rules_ui_import_form_submit($form, &$form_state) {
  $rules_config = $form_state['rules_config'];

  if ($existing_config = rules_config_load($rules_config->name)) {
    // Copy DB id and remove the new indicator to overwrite the existing record.
    $rules_config->id = $existing_config->id;
    unset($rules_config->is_new);
  }
  $rules_config->save();
  $vars = array('@entity' => t('Rules configuration'), '%label' => $rules_config->label());
  watchdog('rules_config', 'Imported @entity %label.', $vars);
  drupal_set_message(t('Imported @entity %label.', $vars));
  $form_state['redirect'] = RulesPluginUI::$basePath;
}

/**
 * FAPI process callback for the data selection widget.
 * This finalises the auto completion callback path by appending the form build
 * id.
 */
function rules_data_selection_process($element, &$form_state, $form) {
  $element['#autocomplete_path'] .= '/' . $form['#build_id'];
  $form_state['cache'] = TRUE;
  return $element;
}

/**
 * Autocomplete data selection results.
 */
function rules_ui_form_data_selection_auto_completion($parameter, $form_build_id, $string = '') {
  // Get the form and its state from the cache to get the currently edited
  // or created element.
  $form_state = form_state_defaults();
  $form = form_get_cache($form_build_id, $form_state);
  if (!isset($form_state['rules_element'])) {
    return;
  }
  $element = $form_state['rules_element'];

  $params = $element->pluginParameterInfo();
  $matches = array();
  if (isset($params[$parameter])) {
    $parts = explode(':', $string);
    // Remove the last part as it might be unfinished.
    $last_part = array_pop($parts);
    $selector = implode(':', $parts);

    // Start with the partly given selector or from scratch.
    $result = array();
    if ($selector && $wrapper = $element->applyDataSelector($selector)) {
      $result = RulesData::matchingDataSelector($wrapper, $params[$parameter], $selector . ':', 0);
    }
    elseif (!$selector) {
      $result = RulesData::matchingDataSelector($element->availableVariables(), $params[$parameter], '', 0);
    }

    foreach ($result as $selector => $info) {
      // If we have an uncomplete last part, take it into account now.
      $attributes = array();
      if (!$last_part || strpos($selector, $string) === 0) {
        $attributes['class'][] = 'rules-dsac-item';
        $attributes['title'] = isset($info['description']) ? strip_tags($info['description']) : '';
        if ($selector[strlen($selector) - 1] == ':') {
          $attributes['class'][] = 'rules-dsac-group';
          $text = check_plain($selector) . '... (' . check_plain($info['label']) . ')';
        }
        else {
          $text = check_plain($selector) . ' (' . check_plain($info['label']) . ')';
        }
        $matches[$selector] = "<div" . drupal_attributes($attributes) . ">$text</div";
      }
    }
  }
  drupal_json_output($matches);
}


/**
 * FAPI validation of an integer element. Copy of the private function
 * _element_validate_integer().
 */
function rules_ui_element_integer_validate($element, &$form_state) {;
  $value = $element['#value'];
  if (isset($value) && $value !== '' && (!is_numeric($value) || intval($value) != $value)) {
    form_error($element, t('%name must be an integer value.', array('%name' => isset($element['#title']) ? $element['#title'] : t('Element'))));
  }
}

/**
 * FAPI validation of a decimal element. Improved version of the private
 * function _element_validate_number().
 */
function rules_ui_element_decimal_validate($element, &$form_state) {
  // Substitute the decimal separator ",".
  $value = strtr($element['#value'], ',', '.');
  if ($value != '' && !is_numeric($value)) {
    form_error($element, t('%name must be a number.', array('%name' => $element['#title'])));
  }
  elseif ($value != $element['#value']) {
    form_set_value($element, $value, $form_state);
  }
}

/**
 * FAPI validation of a date element. Makes sure the specified date format is
 * correct and converts date values specifiy a fixed (= non relative) date to
 * a timestamp. Relative dates are handled by the date input evaluator.
 */
function rules_ui_element_date_validate($element, &$form_state) {
  $value = $element['#value'];
  if ($value == '' || (is_numeric($value) && intval($value) == $value)) {
    // The value is a timestamp.
    return;
  }
  elseif (is_string($value) && RulesDateInputEvaluator::gmstrtotime($value) === FALSE) {
    form_error($element, t('Wrong date format. Specify the date in the format %format.', array('%format' => gmdate('Y-m-d H:i:s', time() + 86400))));
  }
  elseif (is_string($value) && RulesDateInputEvaluator::isFixedDateString($value)) {
    // As the date string specifies a fixed format, we can convert it now.
    $value = RulesDateInputEvaluator::gmstrtotime($value);
    form_set_value($element, $value, $form_state);
  }
}

/**
 * FAPI process callback for the duration element type.
 */
function rules_ui_element_duration_process($element, &$form_state) {
  $element['value'] = array(
    '#type' => 'textfield',
    '#size' => 8,
    '#element_validate' => array('rules_ui_element_integer_validate'),
    '#default_value' => $element['#default_value'],
    '#required' => !empty($element['#required']),
  );
  $element['multiplier'] = array(
    '#type' => 'select',
    '#options' => rules_ui_element_duration_multipliers(),
    '#default_value' => 1,
  );

  // Put the child elements in a container-inline div.
  $element['value']['#prefix'] = '<div class="rules-duration container-inline">';
  $element['multiplier']['#suffix'] = '</div>';

  // Set an appropriate multiplier.
  if (!empty($element['value']['#default_value'])) {
    foreach (array_keys(rules_ui_element_duration_multipliers()) as $m) {
      if ($element['value']['#default_value'] % $m == 0) {
        $element['multiplier']['#default_value'] = $m;
      }
    }
    // Divide value by the multiplier, so the display is correct.
    $element['value']['#default_value'] /= $element['multiplier']['#default_value'];
  }
  return $element;
}

/**
 * Defines possible duration multiplier.
 */
function rules_ui_element_duration_multipliers() {
  return array(
    1 => t('seconds'),
    60 => t('minutes'),
    3600 => t('hours'),
    86400 => t('days'),
  );
}

/**
 * Helper function to determine the value for a rules duration form
 * element.
 */
function rules_ui_element_duration_value($element, $input = FALSE) {
  // This runs before child elements are processed, so we cannot calculate the
  // value here. But we have to make sure the value is an array, so the form
  // API is able to process the children to set their values in the array. Thus
  // once the form API has finished processing the element, the value is an
  // array containing the child element values. Then finally the after build
  // callback converts it back to the numeric value and sets that.
  return array();
}

/**
 * FAPI after build callback for the duration parameter type form.
 * Fixes up the form value by applying the multiplier.
 */
function rules_ui_element_duration_after_build($element, &$form_state) {
  if ($element['value']['#value'] !== '') {
    $element['#value'] = $element['value']['#value'] * $element['multiplier']['#value'];
    form_set_value($element, $element['#value'], $form_state);
  }
  else {
    $element['#value'] = NULL;
    form_set_value($element, NULL, $form_state);
  }
  return $element;
}

/**
 * FAPI after build callback to ensure empty form elements result in no value.
 */
function rules_ui_element_fix_empty_after_build($element, &$form_state) {
  if (isset($element['#value']) && $element['#value'] === '') {
    $element['#value'] = NULL;
    form_set_value($element, NULL, $form_state);
  }
  // Work-a-round for the text_format element.
  elseif ($element['#type'] == 'text_format' && !isset($element['value']['#value'])) {
    form_set_value($element, NULL, $form_state);
  }
  return $element;
}


/**
 * FAPI after build callback for specifying a list of values.
 *
 * Turns the textual value in an array by splitting the text in chunks using the
 * delimiter set at $element['#delimiter'].
 */
function rules_ui_list_textarea_after_build($element, &$form_state) {
  $element['#value'] = $element['#value'] ? explode($element['#delimiter'], $element['#value']) : array();
  $element['#value'] = array_map('trim', $element['#value']);
  form_set_value($element, $element['#value'], $form_state);
  return $element;
}

/**
 * FAPI pre render callback. Turns the value back to a string for rendering.
 *
 * @see rules_ui_list_textarea_after_build()
 */
function rules_ui_list_textarea_pre_render($element) {
  $element['#value'] = implode($element['#delimiter'], $element['#value']);
  return $element;
}

/**
 * FAPI callback to validate a list of integers.
 */
function rules_ui_element_integer_list_validate($element, &$form_state) {
  foreach ($element['#value'] as $value) {
    if ($value !== '' && (!is_numeric($value) || intval($value) != $value)) {
      form_error($element, t('Each value must be an integer.'));
    }
  }
}

/**
 * FAPI callback to validate a token.
 */
function rules_ui_element_token_validate($element) {
  $value = $element['#value'];
  if (isset($value) && $value !== '' && !entity_property_verify_data_type($value, 'token')) {
    form_error($element, t('%name may only contain lowercase letters, numbers, and underscores and has to start with a letter.', array('%name' => isset($element['#title']) ? $element['#title'] : t('Element'))));
  }
}

/**
 * FAPI callback to validate a list of tokens.
 */
function rules_ui_element_token_list_validate($element, &$form_state) {
  foreach ($element['#value'] as $value) {
    if ($value !== '' && !entity_property_verify_data_type($value, 'token')) {
      form_error($element, t('Each value may only contain lowercase letters, numbers, and underscores and has to start with a letter.'));
    }
  }
}

/**
 * FAPI callback to validate a machine readable name.
 */
function rules_ui_element_machine_name_validate($element, &$form_state) {
  if ($element['#value'] && !preg_match('!^[a-z0-9_]+$!', $element['#value'])) {
    form_error($element, t('Machine-readable names must contain only lowercase letters, numbers, and underscores.'));
  }
}

/**
 * FAPI callback to validate the form for editing variable info.
 * @see RulesPluginUI::getVariableForm()
 */
function rules_ui_element_variable_form_validate($elements, &$form_state) {
  $names = array();
  foreach (element_children($elements['items']) as $item_key) {
    $element = &$elements['items'][$item_key];
    if ($element['name']['#value'] || $element['type']['#value'] || $element['label']['#value']) {
      foreach (array('name' => t('Machine name'), 'label' => t('Label'), 'type' => t('Data type')) as $key => $title) {
        if (!$element[$key]['#value']) {
          form_error($element[$key], t('!name field is required.', array('!name' => $title)));
        }
      }
      if (isset($names[$element['name']['#value']])) {
        form_error($element['name'], t('The machine-readable name %name is already taken.', array('%name' => $element['name']['#value'])));
      }
      $names[$element['name']['#value']] = TRUE;
    }
  }
}

/**
 * Helper to sort elements by their 'weight' key.
 */
function rules_element_sort_helper($a, $b) {
  $a += array('weight' => 0);
  $b += array('weight' => 0);
  if ($a['weight'] == $b['weight']) {
    return 0;
  }
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Form after build handler to set the static base path.
 *
 * @see RulesPluginUI::formDefaults()
 */
function rules_form_after_build_restore_base_path($form, &$form_state) {
  if (isset($form_state['_rules_base_path'])) {
    RulesPluginUI::$basePath = $form_state['_rules_base_path'];
  }
  return $form;
}

/**
 * AJAX page callback to load tag suggestions.
 *
 * Largely copied from taxonomy_autocomplete().
 */
function rules_autocomplete_tags($tags_typed = '') {
  // The user enters a comma-separated list of tags. We only autocomplete the
  // last tag.
  $tags_typed = drupal_explode_tags($tags_typed);
  $tag_last = drupal_strtolower(array_pop($tags_typed));

  $tag_matches = array();
  if ($tag_last != '') {
    $query = db_select('rules_tags', 'rt');
    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('rt.tag', $tags_typed, 'NOT IN');
    }
    // Select rows that match by tag name.
    $tags_return = $query
      ->distinct()
      ->fields('rt', array('tag'))
      ->condition('rt.tag', '%' . db_like($tag_last) . '%', 'LIKE')
      ->groupBy('rt.tag')
      ->range(0, 10)
      ->execute()
      ->fetchCol('rt.tag');

    $prefix = count($tags_typed) ? drupal_implode_tags($tags_typed) . ', ' : '';

    foreach ($tags_return as $name) {
      $n = $name;
      // Tag names containing commas or quotes must be wrapped in quotes.
      if (strpos($name, ',') !== FALSE || strpos($name, '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $name) . '"';
      }
      $tag_matches[$prefix . $n] = check_plain($name);
    }
  }
  drupal_json_output($tag_matches);
}
