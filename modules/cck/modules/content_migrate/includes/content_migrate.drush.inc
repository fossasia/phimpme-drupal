<?php

/**
 * @file content_migrate.drush.inc
 * Drush commands for content_migrate,
 */

/**
 * Implementation of hook_drush_help().
 */
function content_migrate_drush_help($section) {
  switch ($section) {
    case 'content migrate:content-migrate-status':
      return dt("Show field migration status");
    case 'content migrate:content-migrate-fields':
      return dt("Migrate some or all fields");
    case 'content migrate:content-migrate-rollback':
      return dt("Roll back some or all fields");
    case 'content migrate:content-migrate-structure':
      return dt("Migrate a single field structure");
    case 'content migrate:content-migrate-data':
      return dt("Migrate a single field data");
  }
}


/**
 * Implementation of hook_drush_command().
 */
function content_migrate_drush_command() {
  $items['content-migrate-status'] = array(
    'description' => 'Show field migration status.',
  );
  $items['content-migrate-fields'] = array(
    'description' => 'Migrate some or all fields.',
    'arguments' => array(
      'name' => 'An optional list of field system names. If not specified, all available fields will be migrated.',
    ),
  );
  $items['content-migrate-rollback'] = array(
    'description' => 'Rollback migration of some or all fields.',
    'arguments' => array(
      'name' => 'An optional list of field system names. If not specified, all available fields will be migrated.',
    ),
  );
  // TODO: decide if these have purpose on their own, or if we should make them hidden.
  $items['content-migrate-field-structure'] = array(
    'description' => 'Migrate a single field structure.',
    'arguments' => array(
      'name' => 'A field system name.',
    ),
  );
  $items['content-migrate-field-data'] = array(
    'description' => 'Migrate a single field data.',
    'arguments' => array(
      'name' => 'A field system name.',
    ),
  );

  return $items;
}

/**
 * Command callback.
 */
function drush_content_migrate_status() {
  module_load_include('inc', 'content_migrate', 'includes/content_migrate.admin');
  $fields_info = content_migrate_get_options();

  $rows = array();
  $rows[] = array(t('Status'), t('Field'), t('Field type'), t('Content type(s)'), t('Other information'));
  foreach ($fields_info as $status => $fields) {
    foreach ($fields as $field_name => $field) {
      array_unshift($field, t(ucfirst($status)));
      $rows[] = $field;
    }
  }
  drush_print_table($rows, TRUE);
}

/**
 * Command callback.
 */
function drush_content_migrate_fields() {
  $field_names = drush_content_migrate_get_fields(_convert_csv_to_array(func_get_args()), 'available');
  if (empty($field_names)) {
    return drush_set_error('CONTENT_MIGRATE_NOTHING_TO_DO', dt('No fields to migrate.'));
  }
  drush_log(dt('The following fields will be migrated: !fields', array('!fields' => implode(', ', $field_names))), 'status');
  if(!drush_confirm(dt('Do you really want to continue?'))) {
    return drush_user_abort();
  }

  foreach ($field_names as $field_name) {
    drush_invoke_process_args('content-migrate-field-structure', array($field_name));
  }
  foreach ($field_names as $field_name) {
    drush_invoke_process_args('content-migrate-field-data', array($field_name));
  }
  // TODO: make an API function for this in Drush.
  _drush_log_drupal_messages();
  drush_log(dt('Migration complete'), 'status');
}

function drush_content_migrate_field_structure($field_name) {
  drush_log(dt('Migrating structure for !field', array('!field' => $field_name)), 'status');
  module_load_include('inc', 'content_migrate', 'includes/content_migrate.admin');
  $context = array();
  _content_migrate_batch_process_create_fields($field_name, $context);
}

function drush_content_migrate_field_data($field_name) {
  drush_log(dt('Migrating data for !field', array('!field' => $field_name)), 'status');
  module_load_include('inc', 'content_migrate', 'includes/content_migrate.admin');
  $batch = array(
    'title' => t('Migrating data'),
    'file'  => drupal_get_path('module', 'content_migrate') . '/includes/content_migrate.admin.inc',
    'operations' => array(array('_content_migrate_batch_process_migrate_data', array($field_name))),
    'finished' => "Field migration is finished",
    'init_message' => t("Fields migration is starting."),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Field migration has encountered an error.'),
  );
  batch_set($batch);
  $batch =& batch_get();
  $batch['progressive'] = FALSE;
  drush_backend_batch_process();
}

/**
 * Command callback.
 */
function drush_content_migrate_rollback() {
  $field_names = drush_content_migrate_get_fields(_convert_csv_to_array(func_get_args()), 'converted');
  if (empty($field_names)) {
    return drush_set_error('CONTENT_MIGRATE_NOTHING_TO_DO', dt('No fields to roll back.'));
  }
  drush_log(dt('The following fields will be rolled back: !fields', array('!fields' => implode(', ', $field_names))), 'status');
  drush_log(dt('Rolling fields back will completely destroy the new field tables. This operation cannot be undone!'), 'warning');
  if(!drush_confirm(dt('Do you really want to continue?'))) {
    return drush_user_abort();
  }
  
  content_migrate_rollback($field_names);
  // Print messages here, so we can give our status at the end.
  // TODO: make an API function for this in Drush.
  _drush_log_drupal_messages();
  drush_log(dt('Roll back complete'), 'status');
}

function drush_content_migrate_get_fields($requests = array(), $status = 'available') {
  module_load_include('inc', 'content_migrate', 'includes/content_migrate.admin');
  $fields_info = content_migrate_get_options();
  $field_names = array();
  if (!empty($fields_info[$status])) {
    $field_names = array_keys($fields_info[$status]);
    if (!empty($requests) && !empty($field_names)) {
      $not_found = array_diff($requests, $field_names);
      $field_names = array_intersect($field_names, $requests);
      if (!empty($not_found)) {
        return drush_set_error('CONTENT_MIGRATE_FIELD_NOT_FOUND', dt('The following fields were not found: !fields', array('!fields' => implode(', ', $not_found))));
      }
    }
  }
  return $field_names;  
}