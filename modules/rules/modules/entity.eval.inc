<?php

/**
 * @file
 * Contains rules integration for entities needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action: Fetch data.
 */
function rules_action_entity_fetch($type, $id, $revision) {
  $info = entity_get_info($type);

  // Support the revision parameter, if applicable.
  if (!empty($info['entity keys']['revision']) && isset($revision)) {
    $conditions = array($info['entity keys']['revision'] => $revision);
  }

  $return = entity_load($type, array($id), isset($conditions) ? $conditions : array());
  $entity = reset($return);
  if (!$entity) {
    throw new RulesEvaluationException('Unable to load @entity with id "@id"', array('@id' => $id, '@entity' => $type));
  }
  return array('entity_fetched' => $entity);
}

/**
 * Info alteration callback for the entity fetch action.
 */
function rules_action_entity_fetch_info_alter(&$element_info, RulesAbstractPlugin $element) {
  $element->settings += array('type' => NULL);
  $info = entity_get_info($element->settings['type']);

  // Fix the type of the identifier.
  $element_info['parameter']['id']['type'] = isset($info['entity keys']['name']) ? 'text' : 'integer';

  // Add an optional revision parameter, if supported.
  if (!empty($info['entity keys']['revision'])) {
    $element_info['parameter']['revision_id'] = array(
      'type' => 'integer',
      'label' => t('Revision identifier'),
      'optional' => TRUE,
    );
  }
  $element_info['provides']['entity_fetched']['type'] = $element->settings['type'];
}

/**
 * Action: Query entities.
 */
function rules_action_entity_query($type, $property, $value, $limit) {
  $return = entity_property_query($type, $property, $value, $limit);
  return array('entity_fetched' => array_values($return));
}

/**
 * Info alteration callback for the entity query action.
 */
function rules_action_entity_query_info_alter(&$element_info, RulesAbstractPlugin $element) {
  $element->settings += array('type' => NULL, 'property' => NULL);
  if ($element->settings['type']) {
    $element_info['parameter']['property']['options list'] = 'rules_action_entity_query_property_options_list';

    if ($element->settings['property']) {
      $wrapper = entity_metadata_wrapper($element->settings['type']);
      if (isset($wrapper->{$element->settings['property']}) && $property = $wrapper->{$element->settings['property']}) {
        $element_info['parameter']['value']['type'] = $property->type();
        $element_info['parameter']['value']['options list']  = $property->optionsList() ? 'rules_action_entity_query_value_options_list' : FALSE;
      }
    }
  }
  $element_info['provides']['entity_fetched']['type'] = 'list<' . $element->settings['type'] . '>';
}

/**
 * Action: Create entities.
 */
function rules_action_entity_create($args, $element) {
  $values = array();
  foreach ($element->pluginParameterInfo() as $name => $info) {
    if ($name != 'type') {
      // Remove the parameter name prefix 'param_'.
      $values[substr($name, 6)] = $args[$name];
    }
  }
  try {
    $data = entity_property_values_create_entity($args['type'], $values);
    return array('entity_created' => $data);
  }
  catch (EntityMetadataWrapperException $e) {
    throw new RulesEvaluationException('Unable to create entity @type": ' . $e->getMessage(), array('@type' => $args['type']), $element);
  }
}

/**
 * Info alteration callback for the entity create action.
 */
function rules_action_entity_create_info_alter(&$element_info, RulesAbstractPlugin $element) {
  if (!empty($element->settings['type'])) {
    $wrapper = entity_metadata_wrapper($element->settings['type']);
    // Add the data type's needed parameter for loading to the parameter info.
    foreach ($wrapper as $name => $child) {
      $info = $child->info();
      if (!empty($info['required'])) {
        $info += array('type' => 'text');
        // Prefix parameter names to avoid name clashes with existing parameters.
        $element_info['parameter']['param_' . $name] = array_intersect_key($info, array_flip(array('type', 'label', 'description')));
        $element_info['parameter']['param_' . $name]['options list']  = $child->optionsList() ? 'rules_action_entity_parameter_options_list' : FALSE;
      }
    }
    $element_info['provides']['entity_created']['type'] = $element->settings['type'];
    if (($bundleKey = $wrapper->entityKey('bundle')) && isset($element->settings['param_' . $bundleKey])) {
      $element_info['provides']['entity_created']['bundle'] = $element->settings['param_' . $bundleKey];
    }
  }
}

/**
 * Action: Save entities.
 */
function rules_action_entity_save($wrapper, $immediate = FALSE, $settings, $state, $element) {
  $state->saveChanges($settings['data:select'], $wrapper, $immediate);
}

/**
 * Action: Delete entities.
 */
function rules_action_entity_delete($wrapper, $settings, $state, $element) {
  try {
    $wrapper->delete();
  }
  catch (EntityMetadataWrapperException $e) {
    throw new RulesEvaluationException($e->getMessage(), array(), $element);
  }
}

/**
 * Condition: Entity is new.
 */
function rules_condition_entity_is_new($wrapper, $settings, $state, $element) {
  return !$wrapper->getIdentifier() || !empty($entity->is_new);
}

/**
 * Condition: Entity has field.
 */
function rules_condition_entity_has_field($wrapper, $field_name, $settings, $state) {
  return isset($wrapper->$field_name) || isset($entity->$field_name);
}

/**
 * Condition: Entity is of type.
 */
function rules_condition_entity_is_of_type($wrapper, $type) {
  return $wrapper->type() == $type;
}
