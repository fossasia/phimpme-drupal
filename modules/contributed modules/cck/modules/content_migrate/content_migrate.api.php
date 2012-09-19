<?php

/**
 * @file
 * Documentation for content migrate API.
 */

/**
 * Implement this hook to alter the field definition of the migrated content.
 *
 * Use this to tweak the conversion of field settings from the D6 style to the
 * D7 style for specific situations not handled by basic conversion, as when
 * field types or settings are changed.
 *
 * @param $field_value
 * @param $instance_value
 */
function hook_content_migrate_field_alter(&$field_value, $instance_value) {
  switch ($instance_value['widget']['module']) {
    case 'filefield':
      // Module names and types changed.
      $field_value['module'] = 'file';
      $field_value['type'] = 'file';
      break;
  }
}

/**
 * Implements this hook to alter the instance definition of the migrated content.
 *
 * Use this to tweak the conversion of instance or widget settings from the D6
 * style to the D7 style for specific situations not handled by basic
 * conversion, as when formatter or widget names or settings are changed.
 *
 * @param $instance_value
 * @param $field_value
 */
function hook_content_migrate_instance_alter(&$instance_value, $field_value) {
  switch ($instance_value['widget']['module']) {
    case 'text':
      // The formatter names changed, all are prefixed with 'text_'.
      foreach ($instance_value['display'] as $context => $settings) {
        $instance_value['display'][$context]['type'] = 'text_'. $settings['type'];
      }
      break;
  }
}


/**
 * Implement this hook to alter individual data records as they are migrated.
 *
 * This hook is called after the old data record has been read from the
 * database and before it is inserted into the corresponding D7 field. The data
 * column names are renamed in a one-to-one mapping by the order they appear in
 * the database. This is often the desired behavior, but in some cases an
 * implementation of this hook may need to move data between columns.
 *
 * @param $record
 *  The data record, as read by _content_migrate_batch_process_migrate_data().
 *  If the ordering of the D6 and D7 field columns remain the same, no action
 *  is required. If the columns were re-ordered or the data format was changed,
 *  $record should be modified to fit the new field definition.
 * @param $field
 *
 */
function hook_content_migrate_data_record_alter(&$record, $field, $instance) {
  switch($field['type']) {
    case 'file':
      // Map D6 filefield field columns to D7 file field columns. Note the data
      // which was previously in the 'data' column is read into the
      // 'description' column since the data column no longer exists.
      if (!empty($record[$field['field_name'] . '_description']) && ($data = unserialize($record[$field['field_name'] . '_description']))) {
        $record[$field['field_name'] . '_description'] = $data['description'];
      }
      else {
        unset($record[$field['field_name'] . '_description']);
      }
      break;
  }
}
