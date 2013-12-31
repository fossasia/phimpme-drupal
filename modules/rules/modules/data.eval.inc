<?php

/**
 * @file
 * Contains rules integration for the data module needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action: Modify data.
 */
function rules_action_data_set($wrapper, $value, $settings, $state, $element) {
  if ($wrapper instanceof EntityMetadataWrapper) {
    try {
      // Update the value first then save changes, if possible.
      $wrapper->set($value);
    }
    catch (EntityMetadataWrapperException $e) {
      throw new RulesEvaluationException('Unable to modify data "@selector": ' . $e->getMessage(), array('@selector' => $settings['data:select']));
    }
    // Save changes if a property of a variable has been changed.
    if (strpos($element->settings['data:select'], ':') !== FALSE) {
      $info = $wrapper->info();
      // We always have to save the changes in the parent entity. E.g. when the
      // node author is changed, we don't want to save the author but the node.
      $state->saveChanges(implode(':', explode(':', $settings['data:select'], -1)), $info['parent']);
    }
  }
  else {
    // A not wrapped variable (e.g. a number) is being updated. Just overwrite
    // the variable with the new value.
    return array('data' => $value);
  }
}

/**
 * Info alter callback for the data_set action.
 */
function rules_action_data_set_info_alter(&$element_info, $element) {
  $element->settings += array('data:select' => NULL);
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    $info = $wrapper->info();
    $element_info['parameter']['value']['type'] = $wrapper->type();
    $element_info['parameter']['value']['options list']  = !empty($info['options list']) ? 'rules_data_selector_options_list' : FALSE;
  }
}

/**
 * Action: Calculate a value.
 */
function rules_action_data_calc($input1, $op, $input2, $settings, $state, $element) {
  switch ($op) {
    case '+':
      $result = $input1 + $input2;
      break;
    case '-':
      $result = $input1 - $input2;
      break;
    case '*':
      $result = $input1 * $input2;
      break;
    case '/':
      $result = $input1 / $input2;
      break;
  }
  if (isset($result)) {
    // Ensure results are valid integer values if necessary.
    $var_info = rules_array_key($element->providesVariables());
    if (in_array($var_info['type'], array('integer', 'date', 'duration'))) {
      $result = (int) $result;
    }
    return array('result' => $result);
  }
}

/**
 * Info alter callback for the data_calc action.
 */
function rules_action_data_calc_info_alter(&$element_info, RulesPlugin $element) {
  if ($info = $element->getArgumentInfo('input_1')) {
    // Only allow durations as offset for date values.
    if ($info['type'] == 'date') {
      $element_info['parameter']['input_2']['type'] = 'duration';
    }
    // Specify the data type of the result.
    $element_info['provides']['result']['type'] = $info['type'];

    if ($info['type'] == 'integer' && ($info2 = $element->getArgumentInfo('input_2')) && $info2['type'] == 'decimal') {
      $element_info['provides']['result']['type'] = 'decimal';
    }
    // A division with two integers results in a decimal.
    elseif (isset($element->settings['op']) && $element->settings['op'] == '/') {
      $element_info['provides']['result']['type'] = 'decimal';
    }
  }
}

/**
 * Action: Add a list item.
 */
function rules_action_data_list_add($list, $item, $unique = FALSE, $pos = 'end', $settings, $state) {
  // Optionally, only add the list item if it is not yet contained.
  if ($unique && rules_condition_data_list_contains($list, $item, $settings, $state)) {
    return;
  }

  switch ($pos) {
    case 'start':
      array_unshift($list, $item);
      break;

    default:
      $list[] = $item;
      break;
  }
  return array('list' => $list);
}

/**
 * Info alteration callback for the "Add and Remove a list item" actions.
 */
function rules_data_list_info_alter(&$element_info, RulesAbstractPlugin $element) {
  // Update the required type for the list item if it is known.
  $element->settings += array('list:select' => NULL);
  if ($wrapper = $element->applyDataSelector($element->settings['list:select'])) {
    if ($type = entity_property_list_extract_type($wrapper->type())) {
      $info = $wrapper->info();
      $element_info['parameter']['item']['type'] = $type;
      $element_info['parameter']['item']['options list']  = !empty($info['options list']) ? 'rules_data_selector_options_list' : FALSE;
    }
  }
}

/**
 * Action: Remove a list item.
 */
function rules_action_data_list_remove($list, $item) {
  foreach (array_keys($list, $item) as $key) {
    unset($list[$key]);
  }
  return array('list' => $list);
}

/**
 * Action: Add variable.
 */
function rules_action_variable_add($args, $element) {
  return array('variable_added' => $args['value']);
}

/**
 * Info alteration callback for variable add action.
 */
function rules_action_variable_add_info_alter(&$element_info, RulesAbstractPlugin $element) {
  if (isset($element->settings['type']) && $type = $element->settings['type']) {
    $cache = rules_get_cache();
    $type_info = $cache['data_info'][$type];
    $element_info['parameter']['value']['type'] = $type;
    $element_info['provides']['variable_added']['type'] = $type;

    // For lists, we default to an empty list so subsequent actions can add
    // items.
    if (entity_property_list_extract_type($type)) {
      $element_info['parameter']['value']['default value'] = array();
    }
  }
}

/**
 * Action: Create data.
 */
function rules_action_data_create($args, $element) {
  $type = $args['type'];
  $values = array();
  foreach ($element->pluginParameterInfo() as $name => $info) {
    if ($name != 'type') {
      // Remove the parameter name prefix 'param_'.
      $values[substr($name, 6)] = $args[$name];
    }
  }
  $cache = rules_get_cache();
  $type_info = $cache['data_info'][$type];
  if (isset($type_info['creation callback'])) {
    try {
      $data = $type_info['creation callback']($values, $type);
      return array('data_created' => $data);
    }
    catch (EntityMetadataWrapperException $e) {
      throw new RulesEvaluationException('Unable to create @data": ' . $e->getMessage(), array('@data' => $type), $element);
    }
  }
  else {
    throw new RulesEvaluationException('Unable to create @data, no creation callback found.', array('@data' => $type), $element, RulesLog::ERROR);
  }
}

/**
 * Info alteration callback for data create action.
 */
function rules_action_data_create_info_alter(&$element_info, RulesAbstractPlugin $element) {
  if (!empty($element->settings['type'])) {
    $type = $element->settings['type'];
    $cache = rules_get_cache();
    $type_info = $cache['data_info'][$type];
    if (isset($type_info['property info'])) {
      // Add the data type's properties as parameters.
      foreach ($type_info['property info'] as $property => $property_info) {
        // Prefix parameter names to avoid name clashes with existing parameters.
        $element_info['parameter']['param_' . $property] = array_intersect_key($property_info, array_flip(array('type', 'label')));
        if (empty($property_info['required'])) {
          $element_info['parameter']['param_' . $property]['optional'] = TRUE;
        }
      }
    }
    $element_info['provides']['data_created']['type'] = $type;
  }
}

/**
 * Creation callback for array structured data.
 */
function rules_action_data_create_array($values = array(), $type) {
  // $values is an array already, so we can just pass it to the wrapper.
  return rules_wrap_data($values, array('type' => $type));
}

/**
 * Condition: Compare data.
 */
function rules_condition_data_is($data, $op, $value) {
  switch ($op) {
    default:
    case '==':
      // In case both values evaluate to FALSE, do a type-safe comparison.
      return (!$data && !$value) ? $data === $value : $data == $value;
    case '<':
      return $data < $value;
    case '>':
      return $data > $value;
    case 'contains':
      return is_string($data) && strpos($data, $value) !== FALSE || is_array($data) && in_array($value, $data);
  }
}

/**
 * Info alteration callback for the data_is condition.
 *
 * If we check the bundle property of a variable, add an assertion so that later
 * evaluated elements can make use of this information.
 */
function rules_condition_data_is_info_alter(&$element_info, RulesAbstractPlugin $element) {
  $element->settings += array('data:select' => NULL);
  if ($wrapper = $element->applyDataSelector($element->settings['data:select'])) {
    $info = $wrapper->info();
    $element_info['parameter']['value']['type'] = $wrapper->type();
    $element_info['parameter']['value']['options list']  = !empty($info['options list']) ? 'rules_data_selector_options_list' : FALSE;
  }
}

/**
 * Condition: List contains.
 */
function rules_condition_data_list_contains($list, $item, $settings, $state) {
  $wrapper = $state->currentArguments['item'];
  if ($wrapper instanceof EntityStructureWrapper && $id = $wrapper->getIdentifier()) {
    // Check for equal items using the identifier if there is one.
    foreach ($state->currentArguments['list'] as $i) {
      if ($i->getIdentifier() == $id) {
        return TRUE;
      }
    }
    return FALSE;
  }
  return in_array($item, $list);
}

/**
 * Condition: Data value is empty.
 */
function rules_condition_data_is_empty($data) {
  return empty($data);
}

/**
 * Condition: Textual comparison.
 */
function rules_data_text_comparison($text, $text2, $op = 'contains') {
  switch ($op) {
    case 'contains':
      return strpos($text, $text2) !== FALSE;
    case 'starts':
      return strpos($text, $text2) === 0;
    case 'ends':
     return strrpos($text, $text2) === (strlen($text) - strlen($text2));
    case 'regex':
     return (bool) preg_match('/'. str_replace('/', '\\/', $text2) .'/', $text);
  }
}
