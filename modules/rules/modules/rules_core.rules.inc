<?php

/**
 * @file Rules core integration providing data types and conditions and
 * actions to invoke configured components.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the pseudo rules_core module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_file_info() {
  return array('modules/rules_core.eval');
}

/**
 * Implements hook_rules_data_info() on behalf of the pseudo rules_core module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_data_info() {
  $return = array(
    'text' => array(
      'label' => t('text'),
      'ui class' => 'RulesDataUIText',
    ),
   'token' => array(
      'label' => t('text token'),
      'parent' => 'text',
      'ui class' => 'RulesDataUITextToken',
    ),
    // A formatted text as used by entity metadata.
    'text_formatted' => array(
      'label' => t('formatted text'),
      'ui class' => 'RulesDataUITextFormatted',
      'wrap' => TRUE,
      'property info' => entity_property_text_formatted_info(),
    ),
   'decimal' => array(
      'label' => t('decimal number'),
      'parent' => 'text',
      'ui class' => 'RulesDataUIDecimal',
    ),
   'integer' => array(
      'label' => t('integer'),
      'class' => 'RulesIntegerWrapper',
      'parent' => 'decimal',
      'ui class' => 'RulesDataUIInteger',
    ),
   'date' => array(
      'label' => t('date'),
      'ui class' => 'RulesDataUIDate',
    ),
   'duration' => array(
      'label' => t('duration'),
      'parent' => 'integer',
      'ui class' => 'RulesDataUIDuration',
    ),
   'boolean' => array(
      'label' => t('truth value'),
      'ui class' => 'RulesDataUIBoolean',
    ),
   'uri' => array(
      'label' => t('URI'),
      'parent' => 'text',
      // Clean inserted tokens with "rawurlencode".
      'cleaning callback' => 'rawurlencode',
      'ui class' => 'RulesDataUIURI',
    ),
   'list' => array(
      'label' => t('list', array(), array('context' => 'data_types')),
      'wrap' => TRUE,
      'group' => t('List', array(), array('context' => 'data_types')),
    ),
   'list<text>' => array(
      'label' => t('list of text'),
      'ui class' => 'RulesDataUIListText',
      'wrap' => TRUE,
      'group' => t('List', array(), array('context' => 'data_types')),
    ),
   'list<integer>' => array(
      'label' => t('list of integer'),
      'ui class' => 'RulesDataUIListInteger',
      'wrap' => TRUE,
      'group' => t('List', array(), array('context' => 'data_types')),
    ),
    'list<token>' => array(
      'label' => t('list of text tokens'),
      'ui class' => 'RulesDataUIListToken',
      'wrap' => TRUE,
      'group' => t('List', array(), array('context' => 'data_types')),
    ),
   'entity' => array(
      'label' => t('any entity'),
      'group' => t('Entity'),
      'is wrapped' => TRUE,
    ),
  );
  foreach (entity_get_info() as $type => $info) {
    if (!empty($info['label'])) {
      $return[$type] = array(
        'label' => strtolower($info['label'][0]) . substr($info['label'], 1),
        'parent' => 'entity',
        'wrap' => TRUE,
        'group' => t('Entity'),
        'ui class' => empty($info['exportable']) ? 'RulesDataUIEntity' : 'RulesDataUIEntityExportable',
      );
    }
  }

  if (module_exists('taxonomy')) {
    // For exportability identify vocabularies by name.
    $return['taxonomy_vocabulary']['wrapper class'] = 'RulesTaxonomyVocabularyWrapper';
    $return['taxonomy_vocabulary']['ui class'] = 'RulesDataUITaxonomyVocabulary';
  }

  return $return;
}

/**
 * Implements hook_rules_data_info_alter() on behalf of the pseudo rules_core module.
 *
 * Makes sure there is a list<type> data type for each type registered.
 *
 * @see rules_rules_data_info_alter()
 */
function rules_rules_core_data_info_alter(&$data_info) {
  foreach ($data_info as $type => $info) {
    if (!entity_property_list_extract_type($type)) {
      $list_type = "list<$type>";
      if (!isset($data_info[$list_type])) {
        $data_info[$list_type] = array(
          'label' => t('list of @type_label items', array('@type_label' => $info['label'])),
          'wrap' => TRUE,
          'group' => t('List', array(), array('context' => 'data_types')),
        );
        if (isset($info['parent']) && $info['parent'] == 'entity') {
          $data_info[$list_type]['ui class'] = 'RulesDataUIListEntity';
        }
      }
    }
  }
}

/**
 * Implements hook_rules_evaluator_info() on behalf of the pseudo rules_core
 * module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_evaluator_info() {
  return array(
    // Process strtotime() inputs to timestamps.
    'date' => array(
      'class' => 'RulesDateInputEvaluator',
      'type' => 'date',
      'weight' => -10,
     ),
    // Post-process any input value to absolute URIs.
    'uri' => array(
      'class' => 'RulesURIInputEvaluator',
      'type' => 'uri',
      'weight' => 50,
     ),
  );
}

/**
 * Implements hook_rules_data_processor_info() on behalf of the pseudo
 * rules_core module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_data_processor_info() {
  return array(
    'date_offset' => array(
      'class' => 'RulesDateOffsetProcessor',
      'type' => 'date',
      'weight' => -2,
     ),
    'num_offset' => array(
      'class' => 'RulesNumericOffsetProcessor',
      'type' => array('integer', 'decimal'),
      'weight' => -2,
     ),
  );
}

/**
 * Implements hook_rules_condition_info() on behalf of the pseudo rules_core
 * module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_condition_info() {
  $defaults = array(
    'group' => t('Components'),
    'base' => 'rules_element_invoke_component',
    'named parameter' => TRUE,
    'access callback' => 'rules_element_invoke_component_access_callback',
  );
  $items = array();
  foreach (rules_get_components(FALSE, 'condition') as $name => $config) {
    $items['component_' . $name] = $defaults + array(
      'label' => $config->plugin() . ': ' . drupal_ucfirst($config->label()),
      'parameter' => $config->parameterInfo(),
    );
    $items['component_' . $name]['#config_name'] = $name;
  }
  return $items;
}

/**
 * Implements hook_rules_action_info() on behalf of the pseudo rules_core
 * module.
 *
 * @see rules_core_modules()
 */
function rules_rules_core_action_info() {
  $defaults = array(
    'group' => t('Components'),
    'base' => 'rules_element_invoke_component',
    'named parameter' => TRUE,
    'access callback' => 'rules_element_invoke_component_access_callback',
  );
  $items = array();
  foreach (rules_get_components(FALSE, 'action') as $name => $config) {
    $items['component_' . $name] = $defaults + array(
      'label' => $config->plugin() . ': ' . drupal_ucfirst($config->label()),
      'parameter' => $config->parameterInfo(),
      'provides' => $config->providesVariables(),
    );
    $items['component_' . $name]['#config_name'] = $name;
  }
  return $items;
}

/**
 * Implements RulesPluginUIInterface::operations() for the action.
 */
function rules_element_invoke_component_operations(RulesPlugin $element) {
  $defaults = $element->extender('RulesPluginUI')->operations();
  $info = $element->info();

  // Add an operation for editing the component.
  $defaults['#links']['component'] = array(
    'title' => t('edit component'),
    'href' => RulesPluginUI::path($info['#config_name']),
  );
  return $defaults;
}

/**
 * Validate callback to make sure the invoked component exists and is not dirty.
 *
 * @see rules_scheduler_action_schedule_validate()
 */
function rules_element_invoke_component_validate(RulesPlugin $element) {
  $recursion = &drupal_static(__FUNCTION__, array());

  // Bail out in case of recursive checks to avoid infinite loops. This is
  // necessary when handling components that directly or indirectly call itself.
  if (!empty($recursion[$element->root()->name])) {
    return;
  }
  $recursion[$element->root()->name] = TRUE;

  $info = $element->info();
  $component = rules_config_load($info['#config_name']);
  if (!$component) {
    unset($recursion[$element->root()->name]);
    throw new RulesIntegrityException(t('The component %config does not exist.', array('%config' => $info['#config_name'])), $element);
  }
  try {
    $component->integrityCheck();
  }
  catch (RulesIntegrityException $e) {
    unset($recursion[$element->root()->name]);
    throw new RulesIntegrityException(t('The component %config fails the integrity check. Error: !message', array('%config' => $info['#config_name'], '!message' => $e->getMessage())), $element);
  }

  unset($recursion[$element->root()->name]);
}

/**
 * Implements the features export callback of the RulesPluginFeaturesIntegrationInterace.
 */
function rules_element_invoke_component_features_export(&$export, &$pipe, $module_name = '', $element) {
  // Add the used component to the pipe.
  $info = $element->info();
  $pipe['rules_config'][] = $info['#config_name'];
}

/**
 * Access callback for the invoke component condition/action.
 */
function rules_element_invoke_component_access_callback($type, $name) {
  // Only allow access to the action/condition if the user has access to the
  // component.
  // Cut of the leading 'component_' from the action name.
  $component = rules_config_load(substr($name, 10));
  return $component && $component->access();
}

/**
 * @}
 */
