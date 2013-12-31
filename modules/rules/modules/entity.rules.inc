<?php

/**
 * @file General entity related rules integration
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the entity module.
 * @see rules_core_modules()
 */
function rules_entity_file_info() {
  return array('modules/entity.eval');
}

/**
 * Implements hook_rules_action_info() on behalf of the entity module.
 * @see rules_core_modules()
 */
function rules_entity_action_info() {
  if (rules_entity_action_type_options('entity_fetch')) {
    $return['entity_fetch'] = array(
      'label' => t('Fetch entity by id'),
      'parameter' => array(
        'type' => array(
          'type' => 'text',
          'label' => t('Entity type'),
          'options list' => 'rules_entity_action_type_options',
          'description' => t('Specifies the type of entity that should be fetched.'),
          'restriction' => 'input',
        ),
        'id' => array('type' => 'unknown', 'label' => t('Identifier')),
      ),
      'provides' => array(
        'entity_fetched' => array('type' => 'unknown', 'label' => t('Fetched entity')),
      ),
      'group' => t('Entities'),
      'access callback' => 'rules_entity_action_access',
      'base' => 'rules_action_entity_fetch',
      'callbacks' => array(
        'access' => 'rules_action_entity_createfetch_access',
        'form_alter' => 'rules_action_type_form_alter',
      ),
    );
    $return['entity_query'] = array(
      'label' => t('Fetch entity by property'),
      'parameter' => array(
        'type' => array(
          'type' => 'text',
          'label' => t('Entity type'),
          'options list' => 'rules_entity_action_type_options',
          'description' => t('Specifies the type of the entity that should be fetched.'),
          'restriction' => 'input',
        ),
        'property' => array(
          'type' => 'text',
          'label' => t('Property'),
          'description' => t('The property by which the entity is to be selected.'),
          'restriction' => 'input',
        ),
        'value' => array(
          'type' => 'unknown',
          'label' => t('Value'),
          'description' => t('The property value of the entity to be fetched.'),
        ),
        'limit' => array(
          'type' => 'integer',
          'label' => t('Limit result count'),
          'description' => t('Limit the maximum number of fetched entities.'),
          'optional' => TRUE,
          'default value' => '10',
        ),
      ),
      'provides' => array(
        'entity_fetched' => array('type' => 'list', 'label' => t('Fetched entity')),
      ),
      'group' => t('Entities'),
      'access callback' => 'rules_entity_action_access',
      'base' => 'rules_action_entity_query',
      'callbacks' => array(
        'form_alter' => 'rules_action_type_form_alter',
      ),
    );
  }

  if (rules_entity_action_type_options('entity_create')) {
    $return['entity_create'] = array(
      'label' => t('Create a new entity'),
      'named parameter' => TRUE,
      'parameter' => array(
        'type' => array(
          'type' => 'text',
          'label' => t('Entity type'),
          'options list' => 'rules_entity_action_type_options',
          'description' => t('Specifies the type of the entity that should be created.'),
          'restriction' => 'input',
        ),
        // Further needed parameter depends on the type.
      ),
      'provides' => array(
        'entity_created' => array(
          'type' => 'unknown',
          'label' => t('Created entity'),
          'save' => TRUE,
        ),
      ),
      'group' => t('Entities'),
      'access callback' => 'rules_entity_action_access',
      'base' => 'rules_action_entity_create',
      'callbacks' => array(
        'access' => 'rules_action_entity_createfetch_access',
        'form_alter' => 'rules_action_type_form_alter',
        'validate' => 'rules_action_create_type_validate',
      ),
    );
  }

  $return['entity_save'] = array(
    'label' => t('Save entity'),
    'parameter' => array(
      'data' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'description' => t('Specifies the entity, which should be saved permanently.'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
      ),
      'immediate' => array(
        'type' => 'boolean',
        'label' => t('Force saving immediately'),
        'description' => t('Usually saving is postponed till the end of the evaluation, so that multiple saves can be fold into one. If this set, saving is forced to happen immediately.'),
        'default value' => FALSE,
        'optional' => TRUE,
        'restriction' => 'input',
      ),
    ),
    'group' => t('Entities'),
    'access callback' => 'rules_entity_action_access',
    'base' => 'rules_action_entity_save',
    'callbacks' => array(
      'access' => 'rules_action_entity_savedelete_access',
    ),
  );

  $return['entity_delete'] = array(
    'label' => t('Delete entity'),
    'parameter' => array(
      'data' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'description' => t('Specifies the entity, which should be deleted permanently.'),
        'restriction' => 'selector',
        'wrapped' => TRUE,
      ),
    ),
    'group' => t('Entities'),
    'access callback' => 'rules_entity_action_access',
    'base' => 'rules_action_entity_delete',
    'callbacks' => array(
      'access' => 'rules_action_entity_savedelete_access',
    ),
  );
  return $return;
}

/**
 * Custom access callback for data create and fetch action.
 */
function rules_action_entity_createfetch_access(RulesAbstractPlugin $element) {
  $op = $element->getElementName() == 'entity_create' ? 'create' : 'view';
  return entity_access($op, $element->settings['type']);
}

/**
 * Custom validate callback for data fetch action.
 */
function rules_action_entity_fetch_validate($element) {
  if (!isset($element->settings['type'])) {
    throw new RulesEvaluationException('Invalid type specified.', array(), array($element, 'parameter', 'type'));
  }
}

/**
 * Custom validate callback for data query action.
 */
function rules_action_entity_query_validate($element) {
  if (!isset($element->settings['type'])) {
    throw new RulesEvaluationException('Invalid type specified.', array(), array($element, 'parameter', 'type'));
  }
  if (!isset($element->settings['property'])) {
    throw new RulesEvaluationException('Invalid property specified.', array(), array($element, 'parameter', 'property'));
  }
}

/**
 * Custom access callback for the data query action.
 */
function rules_action_entity_query_access(RulesAbstractPlugin $element) {
  if (!rules_action_entity_createfetch_access($element)) {
    return FALSE;
  }
  $properties = entity_get_all_property_info($element->settings['type']);
  if (isset($element->settings['property']) && isset($properties[$element->settings['property']]['access callback'])) {
    return call_user_func($properties[$element->settings['property']]['access callback'], 'view', $element->settings['property'], $element->settings['type'], NULL, NULL);
  }
  return TRUE;
}

/**
 * Options list callback for a parameter of entity_create.
 */
function rules_action_entity_parameter_options_list(RulesPlugin $element, $param_name) {
  // Remove the parameter name prefix 'param_'.
  $property_name = substr($param_name, 6);
  $wrapper = entity_metadata_wrapper($element->settings['type']);
  // The possible values of the "value" parameter are those of the data param.
  return $wrapper->$property_name->optionsList();
}

/**
 * Custom access callback for data save and delete action.
 */
function rules_action_entity_savedelete_access(RulesAbstractPlugin $element) {
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    $op = $element->getElementName() == 'entity_save' ? 'save' : 'delete';
    return $wrapper instanceof EntityDrupalWrapper && $wrapper->entityAccess($op);
  }
  return FALSE;
}

/**
 * Returns the options list for choosing a property of an entity type.
 */
function rules_action_entity_query_property_options_list(RulesAbstractPlugin $element) {
  $element->settings += array('type' => NULL);
  if ($element->settings['type']) {
    $properties = entity_get_all_property_info($element->settings['type']);
    return rules_extract_property($properties, 'label');
  }
}

/**
 * Returns the options list specified for the chosen property.
 */
function rules_action_entity_query_value_options_list(RulesAbstractPlugin $element) {
  // Get the possible values for the selected property.
  $element->settings += array('type' => NULL, 'property' => NULL);
  if ($element->settings['type'] && $element->settings['property']) {
    $wrapper = entity_metadata_wrapper($element->settings['type']);

    if (isset($wrapper->{$element->settings['property']}) && $property = $wrapper->{$element->settings['property']}) {
      return $property->optionsList('view');
    }
  }
}

/**
 * Options list callback for data actions.
 *
 * @param $element
 *   The element to return options for.
 * @param $param
 *   The name of the parameter to return options for.
 */
function rules_entity_action_type_options($element, $name = NULL) {
  // We allow calling this function with just the element name too. That way
  // we ease manual re-use.
  $name = is_object($element) ? $element->getElementName() : $element;
  return ($name == 'entity_create') ? rules_entity_type_options('create') : rules_entity_type_options();
}

/**
 * Returns options containing entity types having the given key set in the info.
 *
 * Additionally, we exclude all entity types that are marked as configuration.
 */
function rules_entity_type_options($key = NULL) {
  $info = entity_get_info();
  $types = array();
  foreach ($info as $type => $entity_info) {
    if (empty($entity_info['configuration']) && empty($entity_info['exportable'])) {
      if (!isset($key) || entity_type_supports($type, $key)) {
        $types[$type] = $entity_info['label'];
      }
    }
  }
  return $types;
}

/**
 * Entity actions access callback.
 *
 * Returns TRUE if at least one type is available for configuring the action.
 */
function rules_entity_action_access($type, $name) {
  if ($name == 'entity_fetch' || $name == 'entity_create' || $name == 'entity_query') {
    $types = array_keys(rules_entity_action_type_options($name));
    $op = $name == 'entity_create' ? 'create' : 'view';
  }
  elseif ($name == 'entity_save' || $name == 'entity_delete') {
    $types = array_keys(entity_get_info());
    $op = $name == 'entity_save' ? 'save' : 'delete';
  }
  foreach ($types as $key => $type) {
    if (!entity_access($op, $type)) {
      unset($types[$key]);
    }
  }
  return !empty($types);
}

/**
 * Implements hook_rules_condition_info() on behalf of the entity module.
 * @see rules_core_modules()
 */
function rules_entity_condition_info() {
  return array(
    'entity_is_new' => array(
      'label' => t('Entity is new'),
      'parameter' => array(
        'entity' => array(
          'type' => 'entity',
          'label' => t('Entity'),
          'description' => t('Specifies the entity for which to evaluate the condition.'),
          'restriction' => 'selector',
        ),
      ),
      'group' => t('Entities'),
      'base' => 'rules_condition_entity_is_new',
    ),
    'entity_has_field' => array(
      'label' => t('Entity has field'),
      'parameter' => array(
        'entity' => array(
          'type' => 'entity',
          'label' => t('Entity'),
          'description' => t('Specifies the entity for which to evaluate the condition.'),
          'restriction' => 'selector',
        ),
        'field' => array(
          'type' => 'text',
          'label' => t('Field'),
          'description' => t('The name of the field to check for.'),
          'options list' => 'rules_condition_entity_has_field_options',
          'restriction' => 'input',
        ),
      ),
      'group' => t('Entities'),
      'base' => 'rules_condition_entity_has_field',
    ),
    'entity_is_of_type' => array(
      'label' => t('Entity is of type'),
      'parameter' => array(
        'entity' => array(
          'type' => 'entity',
          'label' => t('Entity'),
          'description' => t('Specifies the entity for which to evaluate the condition.'),
        ),
        'type' => array(
          'type' => 'token',
          'label' => t('Entity type'),
          'description' => t('The entity type to check for.'),
          'options list' => 'rules_entity_action_type_options',
          'restriction' => 'input',
        ),
      ),
      'group' => t('Entities'),
      'base' => 'rules_condition_entity_is_of_type',
    ),
  );
}

/**
 * Help callback for condition entity_is_new.
 */
function rules_condition_entity_is_new_help() {
  return t('This condition determines whether the specified entity has just been created and has not yet been saved to the database.');
}

/**
 * Returns options for choosing a field for the selected entity.
 */
function rules_condition_entity_has_field_options(RulesAbstractPlugin $element) {
  $options = array();
  foreach (field_info_fields() as $field_name => $field) {
    $options[$field_name] = $field_name;
  }
  return $options;
}

/**
 * Assert that the entity has the field, if there is metadata for the field.
 */
function rules_condition_entity_has_field_assertions($element) {
  // Assert the field is there if the condition matches.
  if ($wrapper = $element->applyDataSelector($element->settings['entity:select'])) {
    $type = $wrapper->type();
    $field_property = $element->settings['field'];
    // Get all possible properties and check whether we have one for the field.
    $properties = entity_get_all_property_info($type == 'entity' ? NULL : $type);

    if (isset($properties[$field_property])) {
      $assertion = array('property info' => array($field_property => $properties[$field_property]));
      return array($element->settings['entity:select'] => $assertion);
    }
  }
}

/**
 * Assert the selected entity type.
 */
function rules_condition_entity_is_of_type_assertions($element) {
  if ($type = $element->settings['type']) {
    return array('entity' => array('type' => $type));
  }
}

/**
 * @}
 */
