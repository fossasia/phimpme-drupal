<?php

/**
 * @file General data related rules integration
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the pseudo data module.
 * @see rules_core_modules()
 */
function rules_data_file_info() {
  return array('modules/data.eval');
}

/**
 * Implements hook_rules_action_info() on behalf of the pseudo data module.
 * @see rules_core_modules()
 */
function rules_data_action_info() {
  $return['data_set'] = array(
    'label' => t('Set a data value'),
    'parameter' => array(
      'data' => array(
        'type' => '*',
        'label' => t('Data'),
        'description' => t('Specifies the data to be modified using a data selector, e.g. "node:author:name".'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
        'allow null' => TRUE,
      ),
      'value' => array(
        'type' => '*',
        'label' => t('Value'),
        'description' => t('The new value to set for the specified data.'),
        'allow null' => TRUE,
        'optional' => TRUE,
      ),
    ),
    'group' => t('Data'),
    'base' => 'rules_action_data_set',
  );
  $return['data_calc'] = array(
    'label' => t('Calculate a value'),
    'parameter' => array(
      'input_1' => array(
        'type' => array('decimal', 'date'),
        'label' => t('Input value 1'),
        'description' => t('The first input value for the calculation.'),
      ),
      'op' => array(
        'type' => 'text',
        'label' => t('Operator'),
        'description' => t('The calculation operator.'),
        'options list' => 'rules_action_data_calc_operator_options',
        'restriction' => 'input',
        'default value' => '+',
      ),
      'input_2' => array(
        'type' => 'decimal',
        'label' => t('Input value 2'),
        'description' => t('The second input value.'),
      ),
    ),
    'group' => t('Data'),
    'base' => 'rules_action_data_calc',
    'provides' => array(
      'result' => array(
        'type' => 'unknown',
        'label' => t('Calculation result'),
      ),
    ),
  );
  $return['list_add'] = array(
    'label' => t('Add an item to a list'),
    'parameter' => array(
      'list' => array(
        'type' => 'list',
        'label' => t('List', array(), array('context' => 'data_types')),
        'description' => t('The data list, to which an item is to be added.'),
        'restriction' => 'selector',
        'allow null' => TRUE,
      ),
      'item' => array(
        'type' => 'unknown',
        'label' => t('Item to add'),
      ),
      'unique' => array(
        'type' => 'boolean',
        'label' => t('Enforce uniqueness'),
        'description' => t('Only add the item to the list if it is not yet contained.'),
        'optional' => TRUE,
        'default value' => FALSE,
      ),
      'pos' => array(
        'type' => 'text',
        'label' => t('Insert position'),
        'optional' => TRUE,
        'default value' => 'end',
        'options list' => 'rules_action_data_list_add_positions',
      ),
    ),
    'group' => t('Data'),
    'base' => 'rules_action_data_list_add',
    'callbacks' => array(
      'info_alter' => 'rules_data_list_info_alter',
      'form_alter' => 'rules_data_list_form_alter',
    ),
  );
  $return['list_remove'] = array(
    'label' => t('Remove an item from a list'),
    'parameter' => array(
      'list' => array(
        'type' => 'list',
        'label' => t('List', array(), array('context' => 'data_types')),
        'description' => t('The data list for which an item is to be removed.'),
        'restriction' => 'selector',
      ),
      'item' => array(
        'type' => 'unknown',
        'label' => t('Item to remove'),
      ),
    ),
    'group' => t('Data'),
    'base' => 'rules_action_data_list_remove',
    'callbacks' => array(
      'info_alter' => 'rules_data_list_info_alter',
      'form_alter' => 'rules_data_list_form_alter',
    ),
  );
  $return['variable_add'] = array(
    'label' => t('Add a variable'),
    'named parameter' => TRUE,
    'parameter' => array(
      'type' => array(
        'type' => 'text',
        'label' => t('Type'),
        'options list' => 'rules_data_action_variable_add_options',
        'description' => t('Specifies the type of the variable that should be added.'),
        'restriction' => 'input',
      ),
      'value' => array(
        'type' => 'unknown',
        'label' => t('Value'),
        'optional' => TRUE,
        'description' => t('Optionally, specify the initial value of the variable.')
      ),
    ),
    'provides' => array(
      'variable_added' => array(
        'type' => 'unknown',
        'label' => t('Added variable'),
      ),
    ),
    'group' => t('Data'),
    'base' => 'rules_action_variable_add',
    'callbacks' => array(
      'form_alter' => 'rules_action_type_form_alter',
      'validate' => 'rules_action_create_type_validate',
    ),
  );

  if (rules_data_action_data_create_options()) {
    $return['data_create'] = array(
      'label' => t('Create a data structure'),
      'named parameter' => TRUE,
      'parameter' => array(
        'type' => array(
          'type' => 'text',
          'label' => t('Type'),
          'options list' => 'rules_data_action_data_create_options',
          'description' => t('Specifies the type of the data structure that should be created.'),
          'restriction' => 'input',
        ),
        // Further needed parameters depend on the type.
      ),
      'provides' => array(
        'data_created' => array(
          'type' => 'unknown',
          'label' => t('Created data'),
        ),
      ),
      'group' => t('Data'),
      'base' => 'rules_action_data_create',
      'callbacks' => array(
        'form_alter' => 'rules_action_type_form_alter',
        'validate' => 'rules_action_create_type_validate',
      ),
    );
  }
  return $return;
}

/**
 * Customize access check for data set action.
 */
function rules_action_data_set_access(RulesAbstractPlugin $element) {
  if (isset($element->settings['data:select']) && $wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    return $wrapper instanceof EntityMetadataWrapper && $wrapper->access('edit');
  }
}

/**
 * Custom validation callback for the data set action.
 */
function rules_action_data_set_validate(RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  $info = $element->applyDataSelector($element->settings['data:select'])->info();
  if (strpos($element->settings['data:select'], ':') !== FALSE && empty($info['setter callback'])) {
    throw new RulesIntegrityException(t("The selected data property doesn't support writing."), array($element, 'parameter', 'data'));
  }
}

/**
 * Form alter callback for the data_set action.
 */
function rules_action_data_set_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    $form['negate']['#access'] = FALSE;
    unset($form['parameter']['value']);
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#limit_validation_errors' => array(array('parameter', 'data')),
      '#submit' => array('rules_form_submit_rebuild'),
    );
    $form_state['rules_element_step'] = 'data_value';
    // Clear the parameter mode for the value parameter, so its gets the proper
    // default value based upon the type of the the selected data on rebuild.
    unset($form_state['parameter_mode']['value']);
  }
  else {
    // Change the data parameter to be not editable.
    $form['parameter']['data']['settings']['#access'] = FALSE;
    // TODO: improve display
    $form['parameter']['data']['info'] = array(
      '#prefix' => '<p>',
      '#markup' => t('<strong>Selected data:</strong> %selector', array('%selector' => $element->settings['data:select'])),
      '#suffix' => '</p>',
    );
  }
}

/**
 * Form alter callback for the data calculation action.
 */
function rules_action_data_calc_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {

  $form['reload'] = array(
    '#weight' => 5,
    '#type' => 'submit',
    '#name' => 'reload',
    '#value' => t('Reload form'),
    '#limit_validation_errors' => array(array('parameter', 'input_1')),
    '#submit' => array('rules_form_submit_rebuild'),
    '#ajax' => rules_ui_form_default_ajax(),
  );
}

/**
 * Custom validate callback for entity create, add variable and data create
 * action.
 */
function rules_action_create_type_validate($element) {
  if (!isset($element->settings['type'])) {
    throw new RulesIntegrityException(t('Invalid type specified.'), array($element, 'parameter', 'type'));
  }
}

/**
 * Form alter callback for the list add and remove actions.
 *
 * Use multiple steps to configure the action to update the item configuration
 * form once we know the data type.
 *
 * @see rules_data_list_info_alter()
 */
function rules_data_list_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    unset($form['parameter']['item'], $form['parameter']['pos']);
    $form_state['rules_element_step'] = 1;
    $form['negate']['#access'] = FALSE;
    $form['parameter']['unique']['#access'] = FALSE;
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#limit_validation_errors' => array(array('parameter', 'list')),
      '#submit' => array('rules_form_submit_rebuild'),
    );
  }
  else {
    // Change the list parameter to be not editable any more.
    $form['parameter']['list']['settings']['#access'] = FALSE;
    $form['parameter']['list']['info'] = array(
      '#prefix' => '<p>',
      '#markup' => t('<strong>Selected list:</strong> %selector', array('%selector' => $element->settings['list:select'])),
      '#suffix' => '</p>',
    );
  }
}


/**
 * Form alter callback for actions relying on the entity type or the data type.
 */
function rules_action_type_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  $first_step = empty($element->settings['type']);
  $form['reload'] = array(
    '#weight' => 5,
    '#type' => 'submit',
    '#name' => 'reload',
    '#value' => $first_step ? t('Continue') : t('Reload form'),
    '#limit_validation_errors' => array(array('parameter', 'type')),
    '#submit' => array('rules_action_type_form_submit_rebuild'),
    '#ajax' => rules_ui_form_default_ajax(),
  );
  // Use ajax and trigger as the reload button.
  $form['parameter']['type']['settings']['type']['#ajax'] = $form['reload']['#ajax'] + array(
    'event' => 'change',
    'trigger_as' => array('name' => 'reload'),
  );

  if ($first_step) {
    // In the first step show only the type select.
    foreach (element_children($form['parameter']) as $key) {
      if ($key != 'type') {
        unset($form['parameter'][$key]);
      }
    }
    unset($form['submit']);
    unset($form['provides']);
    // Disable #ajax for the first step as it has troubles with lazy-loaded JS.
    // @todo: Re-enable once JS lazy-loading is fixed in core.
    unset($form['parameter']['type']['settings']['type']['#ajax']);
    unset($form['reload']['#ajax']);
  }
  else {
    // Hide the reload button in case js is enabled and it's not the first step.
    $form['reload']['#attributes'] = array('class' => array('rules-hide-js'));
  }
}

/**
 * FAPI submit callback for reloading the type form for entities or data types.
 */
function rules_action_type_form_submit_rebuild($form, &$form_state) {
  rules_form_submit_rebuild($form, $form_state);
  // Clear the parameter modes for the parameters, so they get the proper
  // default values based upon the data types on rebuild.
  $form_state['parameter_mode'] = array();
}

/**
 * Options list callback for possible insertion positions.
 */
function rules_action_data_list_add_positions() {
  return array(
    'end' => t('Append the item to the end.'),
    'start' => t('Prepend the item to the front.'),
  );
}

/**
 * Options list callback for variable add action.
 */
function rules_data_action_variable_add_options() {
  return RulesPluginUI::getOptions('data');
}

/**
 * Options list callback for the data calculation action.
 */
function rules_action_data_calc_operator_options(RulesPlugin $element, $param_name) {
  $options = array(
    '+' => '( + )',
    '-' => '( - )',
    '*' => '( * )',
    '/' => '( / )',
  );
  // Only show +/- in case a date has been selected.
  if (($info = $element->getArgumentInfo('input_1')) && $info['type'] == 'date') {
    unset($options['*']);
    unset($options['/']);
  }
  return $options;
}

/**
 * Options list callback for data create action.
 */
function rules_data_action_data_create_options() {
  $cache = rules_get_cache();
  $data_info = $cache['data_info'];
  $entity_info = entity_get_info();
  // Remove entities.
  $data_info = array_diff_key($data_info, $entity_info);
  $options = array();
  foreach ($data_info as $type => $properties) {
    if (isset($properties['creation callback'])) {
      // Add data types with creation callback only.
      $options[$type] = $properties['label'];
    }
  }
  natcasesort($options);
  return $options;
}

/**
 * Implements hook_rules_condition_info() on behalf of the pseudo data module.
 * @see rules_core_modules()
 */
function rules_data_condition_info() {
  return array(
    'data_is' => array(
      'label' => t('Data comparison'),
      'parameter' => array(
        'data' => array(
          'type' => '*',
          'label' => t('Data to compare'),
          'description' => t('The data to be compared, specified by using a data selector, e.g. "node:author:name".'),
          'allow null' => TRUE,
        ),
        'op' => array(
          'type' => 'text',
          'label' => t('Operator'),
          'description' => t('The comparison operator.'),
          'optional' => TRUE,
          'default value' => '==',
          'options list' => 'rules_condition_data_is_operator_options',
          'restriction' => 'input',
        ),
        'value' => array(
          'type' => '*',
          'label' => t('Data value'),
          'description' => t('The value to compare the data with.'),
          'allow null' => TRUE,
        ),
      ),
      'group' => t('Data'),
      'base' => 'rules_condition_data_is',
    ),
    'data_is_empty' => array(
      'label' => t('Data value is empty'),
      'parameter' => array(
        'data' => array(
          'type' => '*',
          'label' => t('Data to check'),
          'description' => t('The data to be checked to be empty, specified by using a data selector, e.g. "node:author:name".'),
          'allow null' => TRUE,
        ),
      ),
      'group' => t('Data'),
      'base' => 'rules_condition_data_is_empty',
    ),
    'list_contains'  => array(
      'label' => t('List contains item'),
      'parameter' => array(
        'list' => array(
          'type' => 'list',
          'label' => t('List', array(), array('context' => 'data_types')),
          'restriction' => 'selector',
        ),
        'item' => array(
          'type' => 'unknown',
          'label' => t('Item'),
          'description' => t('The item to check for.'),
        ),
      ),
      'group' => t('Data'),
      'base' => 'rules_condition_data_list_contains',
      'callbacks' => array(
        'info_alter' => 'rules_data_list_info_alter',
        'form_alter' => 'rules_data_list_form_alter',
      ),
    ),
    'text_matches'  => array(
      'label' => t('Text comparison'),
      'parameter' => array(
        'text' => array(
          'type' => 'text',
          'label' => t('Text'),
          'restriction' => 'selector',
        ),
        'match' => array(
          'type' => 'text',
          'label' => t('Matching text'),
        ),
        'operation' => array(
          'type' => 'text',
          'label' => t('Comparison operation'),
          'options list' => 'rules_data_text_comparison_operation_list',
          'restriction' => 'input',
          'default value' => 'contains',
          'optional' => TRUE,
          'description' => t('In case the comparison operation @regex is selected, the matching pattern will be interpreted as a <a href="@regex-wikipedia">regular expression</a>.  Tip: <a href="@RegExr">RegExr: Online Regular Expression Testing Tool</a> is helpful for learning, writing, and testing Regular Expressions.', array('@regex-wikipedia' => 'http://en.wikipedia.org/wiki/Regular_expression', '@RegExr' => 'http://gskinner.com/RegExr/', '@regex' => t('regular expression'))),
        ),
      ),
      'group' => t('Data'),
      'base' => 'rules_data_text_comparison',
    ),
  );
}

/**
 * If the bundle is compared, add the metadata assertion so other elements
 * can make use of properties specific to the bundle.
 */
function rules_condition_data_is_assertions($element) {
  // Assert the bundle of entities, if its compared.
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    $info = $wrapper->info();
    if (isset($info['parent']) && $info['parent'] instanceof EntityDrupalWrapper) {
      $entity_info = $info['parent']->entityInfo();
      if (isset($entity_info['entity keys']['bundle']) && $entity_info['entity keys']['bundle'] == $info['name']) {
        // Assert that the entity is of bundle $value.
        $value = is_array($element->settings['value']) ? $element->settings['value'] : array($element->settings['value']);
        // Chop of the last part of the selector.
        $parts = explode(':', $element->settings['data:select'], -1);
        return array(implode(':', $parts) => array('bundle' => $value));
      }
    }
  }
}

/**
 * Form alter callback for the condition data_is.
 * Use multiple steps to configure the condition as the needed type of the value
 * depends on the selected data.
 */
function rules_condition_data_is_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
  if (!empty($options['init']) && !isset($form_state['rules_element_step'])) {
    unset($form['parameter']['op'], $form['parameter']['value']);
    $form['negate']['#access'] = FALSE;
    $form_state['rules_element_step'] = 'data_value';
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#limit_validation_errors' => array(array('parameter', 'data')),
      '#submit' => array('rules_form_submit_rebuild'),
    );
    // Clear the parameter mode for the value parameter, so its gets the proper
    // default value based upon the type of the the selected data on rebuild.
    unset($form_state['parameter_mode']['value']);
  }
  else {
    // Change the data parameter to be not editable.
    $form['parameter']['data']['settings']['#access'] = FALSE;
    // TODO: improve display
    $form['parameter']['data']['info'] = array(
      '#prefix' => '<p>',
      '#markup' => t('<strong>Selected data:</strong> %selector', array('%selector' => $element->settings['data:select'])),
      '#suffix' => '</p>',
    );

    // Limit the operations to what makes sense for the data type.
    $info = $element->pluginParameterInfo();
    if (!RulesData::typesMatch($info['value'], array('type' => array('decimal', 'date')))) {
      $options =& $form['parameter']['op']['settings']['op']['#options'];
      unset($options['<'], $options['>']);
    }
    if ($info['value']['type'] != 'text' || !empty($info['value']['options list'])) {
      unset($form['parameter']['op']['settings']['op']['#options']['contains']);
    }
  }
}

/**
 * Provides configuration help for the data_is condition.
 */
function rules_condition_data_is_help() {
  return array('#markup' => t('Compare two data values of the same type with each other.'));}

/**
 * Options list callback for condition data_is.
 */
function rules_condition_data_is_operator_options() {
  return array(
    '==' => t('equals'),
    '<' => t('is lower than'),
    '>' => t('is greater than'),
    'contains' => t('contains'),
  );
}

/**
 * Options list callback for condition text_matches.
 */
function rules_data_text_comparison_operation_list() {
  return array(
    'contains' => t('contains'),
    'starts' => t('starts with'),
    'ends' => t('ends with'),
    'regex' => t('regular expression'),
  );
}

/**
 * Returns the options list as specified by the selected property of the first parameter.
 *
 * @see rules_data_list_info_alter()
 * @see rules_action_data_set_info_alter()
 * @see rules_condition_data_is_info_alter()
 */
function rules_data_selector_options_list(RulesAbstractPlugin $element) {
  $name = rules_array_key($element->pluginParameterInfo());
  // If the selected data property has an option list, make use of it.
  if (isset($element->settings[$name . ':select']) && $wrapper = $element->applyDataSelector($element->settings[$name . ':select'])) {
    return $wrapper->optionsList($element instanceof RulesActionInterface ? 'edit' : 'view');
  }
}

/**
 * @}
 */
