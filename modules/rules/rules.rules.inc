<?php

/**
 * @file Includes any rules integration provided by the module.
 */

/**
 * Load all module includes as soon as this file gets included, which is done
 * automatically by module_implements().
 */
foreach (rules_core_modules() as $module) {
  module_load_include('inc', 'rules', "modules/$module.rules");
  module_load_include('inc', 'rules', 'modules/events');
}

/**
 * Defines a list of core module on whose behalf we provide module integration.
 *
 * We also add a pseudo 'data' module, which will be used for providing generic
 * rules data integration, 'entity' for entity-related integration and 'rules'
 * for providing some general stuff.
 */
function rules_core_modules() {
  $return = array('data', 'entity', 'node', 'system', 'user', 'rules_core');
  foreach (array('comment', 'taxonomy', 'php', 'path') as $module) {
    if (module_exists($module)) {
      $return[] = $module;
    }
  }
  return $return;
}

/**
 * Returns all items for a hook applying the right module defaults.
 */
function _rules_rules_collect_items($hook) {
  $items = array();
  foreach (rules_core_modules() as $module) {
    if (function_exists($function = "rules_{$module}_{$hook}")) {
      $items += (array) $function();
    }
  }
  return $items;
}

/**
 * Implements hook_rules_file_info().
 */
function rules_rules_file_info() {
  $items = array();
  foreach (rules_core_modules() as $module) {
    if (function_exists($function = "rules_{$module}_file_info")) {
      $items = array_merge($items, (array)$function());
      // Automatically add "$module.rules.inc" for each module.
      $items[] = 'modules/' . $module . '.rules';
    }
  }
  return $items;
}

/**
 * Implements hook_rules_action_info().
 */
function rules_rules_action_info() {
  return _rules_rules_collect_items('action_info');
}

/**
 * Implements hook_rules_condition_info().
 */
function rules_rules_condition_info() {
  return _rules_rules_collect_items('condition_info');
}

/**
 * Implements hook_rules_event_info().
 */
function rules_rules_event_info() {
  return _rules_rules_collect_items('event_info');
}

/**
 * Implements hook_rules_data_info().
 */
function rules_rules_data_info() {
  return _rules_rules_collect_items('data_info');
}

/**
 * Implements hook_rules_data_info_alter().
 */
function rules_rules_data_info_alter(&$items) {
  // For now just invoke the rules core implementation manually.
  rules_rules_core_data_info_alter($items);
}

/**
 * Implements hook_rules_evaluator_info().
 */
function rules_rules_evaluator_info() {
  return _rules_rules_collect_items('evaluator_info');
}

/**
 * Implements hook_rules_data_processor_info().
 */
function rules_rules_data_processor_info() {
  return _rules_rules_collect_items('data_processor_info');
}
