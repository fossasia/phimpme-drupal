<?php

/**
 * @file Invokes events on behalf core modules. Usually this should be
 *   directly in the module providing rules integration instead.
 *
 * @addtogroup rules
 * @{
 */


/**
 * Gets an unchanged entity that doesn't contain any recent changes. This
 * handler assumes the name of the variable for the changed entity is the same
 * as for the unchanged entity but without the trailing "_unchanged"; e.g., for
 * the "node_unchanged" variable the handler assumes there is a "node" variable.
 */
function rules_events_entity_unchanged($arguments, $name, $info) {
  // Cut of the trailing _unchanged.
  $var_name = substr($name, 0, -10);
  $entity = $arguments[$var_name];
  if (isset($entity->original)) {
    return $entity->original;
  }
}

/**
 * Generic entity events, used for core-entities for which we provide Rules
 * integration only.
 * We are implementing the generic-entity hooks instead of the entity-type
 * specific hooks to ensure we come last. See http://drupal.org/node/1211946
 * for details.
 */

/**
 * Implements hook_entity_view().
 */
function rules_entity_view($entity, $type, $view_mode, $langcode) {
  $entity_types = array(
    'comment' => TRUE,
    'node' => TRUE,
    'user' => TRUE,
  );
  if (isset($entity_types[$type])) {
    rules_invoke_event($type . '_view', $entity, $view_mode);
  }
}

/**
 * Implements hook_entity_presave().
 */
function rules_entity_presave($entity, $type) {
  $entity_types = array(
    'comment' => TRUE,
    'node' => TRUE,
    'taxonomy_term' => TRUE,
    'taxonomy_vocabulary' => TRUE,
    'user' => TRUE,
  );
  if (isset($entity_types[$type])) {
    rules_invoke_event($type . '_presave', $entity);
  }
}

/**
 * Implements hook_entity_update().
 */
function rules_entity_update($entity, $type) {
  $entity_types = array(
    'comment' => TRUE,
    'node' => TRUE,
    'taxonomy_term' => TRUE,
    'taxonomy_vocabulary' => TRUE,
    'user' => TRUE,
  );
  if (isset($entity_types[$type])) {
    rules_invoke_event($type . '_update', $entity);
  }
}

/**
 * Implements hook_entity_insert().
 */
function rules_entity_insert($entity, $type) {
  $entity_types = array(
    'comment' => TRUE,
    'node' => TRUE,
    'taxonomy_term' => TRUE,
    'taxonomy_vocabulary' => TRUE,
    'user' => TRUE,
  );
  if (isset($entity_types[$type])) {
    rules_invoke_event($type . '_insert', $entity);
  }
}

/**
 * Implements hook_entity_delete().
 */
function rules_entity_delete($entity, $type) {
  $entity_types = array(
    'comment' => TRUE,
    'node' => TRUE,
    'taxonomy_term' => TRUE,
    'taxonomy_vocabulary' => TRUE,
    'user' => TRUE,
  );
  if (isset($entity_types[$type])) {
    rules_invoke_event($type . '_delete', $entity);
  }
}

/**
 * Implements hook_user_login().
 */
function rules_user_login(&$edit, $account) {
  rules_invoke_event('user_login', $account);
}

/**
 * Implements hook_user_logout().
 */
function rules_user_logout($account) {
  rules_invoke_event('user_logout', $account);
}

/**
 * System events. Note that rules_init() is the main module file is used to
 * invoke the init event.
 */

/**
 * Implements hook_cron().
 */
function rules_cron() {
  rules_invoke_event('cron');
}

/**
 * Implements hook_watchdog().
 */
function rules_watchdog($log_entry) {
  rules_invoke_event('watchdog', $log_entry);
}

/**
 * Getter callback for the log entry message property.
 */
function rules_system_log_get_message($log_entry) {
  return t($log_entry['message'], (array)$log_entry['variables']);
}

/**
 * Gets all view modes of an entity for an entity_view event.
 */
function rules_get_entity_view_modes($name, $var_info) {
  // Read the entity type from a special key out of the variable info.
  $entity_type = $var_info['options list entity type'];
  $info = entity_get_info($entity_type);
  foreach ($info['view modes'] as $mode => $mode_info) {
    $modes[$mode] = $mode_info['label'];
  }
  return $modes;
}

/**
 * @}
 */