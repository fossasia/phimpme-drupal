<?php

/**
 * @file Include file for testing file inclusion.
 */

/**
 * Extender for the node data type.
 */
function rules_test_custom_node_save($object) {
  throw new RulesEvaluationException('Custom save method invoked.');
}


/**
 * Custom help callback for the rules_node_publish_action
 */
function rules_test_custom_help() {
  return 'custom';
}

/**
 * Action callback
 */
function rules_action_test_reference($data) {
  $data['changed'] = TRUE;
  return array('arg' => $data);
}

/**
 * Condition: Check for selected content types
 */
function rules_condition_content_is_type($node, $type) {
  return in_array($node->type, $type);
}

/**
 * Condition: Check if the node is published
 */
function rules_condition_content_is_published($node, $settings) {
  return $node->status == 1;
}

/**
 * Loads a node
 */
function rules_action_load_node($nid, $vid = NULL) {
  return array('node_loaded' => node_load($nid, $vid ? $vid : NULL));
}

/**
 * Action "Delete a node".
 */
function rules_action_delete_node($node) {
  node_delete($node->nid);
}

/**
 * An action making use of named parameters.
 */
function rules_action_node_set_title($arguments) {
  // Make sure the data is unwrapped.
  if ($arguments['node'] instanceof EntityMetadataWrapper) {
    throw new Exception('Argument has not been correctly unwrapped.');
  }
  $arguments['node']->title = $arguments['title'];
  return $arguments;
}

/**
 * Action: Test saving - nothing to do here.
 */
function rules_test_type_save($node) {

}
