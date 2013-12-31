<?php

/**
 * @file
 * Contains rules integration for the node module needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Condition: Check for selected content types
 */
function rules_condition_node_is_of_type($node, $types) {
  return in_array($node->type, $types);
}

/**
 * Condition: Check if the node is published
 */
function rules_condition_node_is_published($node) {
  return $node->status == 1;
}

/**
 * Condition: Check if the node is sticky
 */
function rules_condition_node_is_sticky($node) {
  return $node->sticky == 1;
}

/**
 * Condition: Check if the node is promoted to the frontpage
 */
function rules_condition_node_is_promoted($node) {
  return $node->promote == 1;
}

/**
 * @}
 */
