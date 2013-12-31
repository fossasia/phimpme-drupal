<?php

/**
 * @file
 * Provide Views data and handlers for media.module
 */

/**
 * Implements hook_field_views_data().
 */
function media_field_views_data($field) {
  $data = field_views_field_default_views_data($field);
  foreach ($data as $table_name => $table_data) {
    // Add the relationship only on the fid field.
    $data[$table_name][$field['field_name'] . '_fid']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'file_managed',
      'entity type' => 'file',
      'base field' => 'fid',
      'label' => t('file from !field_name', array('!field_name' => $field['field_name'])),
    );
  }

  return $data;
}
