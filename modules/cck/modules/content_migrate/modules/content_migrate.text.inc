<?php
/**
 * @file content_migrate.text.inc
 * Code to implement hook_content_migrate_field_alter, content_migrate_instance_alter() and content_migrate_data_record_alter()
 * on behalf of the former text module, moved into a separate file for efficiency.
 */

/**
 * Implements hook_content_migrate_field_alter().
 * 
 * Use this to tweak the conversion of field settings
 * from the D6 style to the D7 style for specific
 * situations not handled by basic conversion,
 * as when field types or settings are changed.
 */
function content_migrate_text_field_alter(&$field_value, $instance_value) {
  switch ($field_value['type']) {
    case 'text':

      // Text fields are translatable.      
      // See the debate going on at http://drupal.org/node/1164852.
      // Reverting back to setting all nodes as untranslated.
      //$field_value['translatable'] = TRUE;

      switch ($instance_value['widget']['type']) {

        // Text fields using optionwidgets are
        // now List fields.
        case 'optionwidgets_buttons':
        case 'optionwidgets_select':
          $field_value['messages'][] = t("Changed field type: The '@field' field uses a '@widget' widget. The field type will be changed from '@type' to 'list_text'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
          $field_value['type'] = 'list_text';
          $field_value['module'] = 'list';
          // Lists don't have a max_length setting.
          if (isset($field_value['settings']['max_length'])) {
            unset($field_value['settings']['max_length']);
          }
          break;

        case 'optionwidgets_onoff':
          $field_value['messages'][] = t("Changed field type: The '@field' field uses a '@widget' widget. The field type will be changed from '@type' to 'list_boolean'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
          $field_value['type'] = 'list_boolean';
          $field_value['module'] = 'list';
          // Lists don't have a max_length setting.
          if (isset($field_value['settings']['max_length'])) {
            unset($field_value['settings']['max_length']);
          }
          break;

        // The max_length field can no longer be empty 
        // or it will create a SQL error. There also have been
        // changes to the way text fields and widgets are configured.
        // In D6, if you left max_length empty CCK would create a long_text field,
        // even if you were using a textfield widget. In D7 it is not
        // possible to have a long_text field with a textfield widget.
        // We are now forced to figure out what to do with this data.
    
        // It is so easy to leave the length unset that it is likely that any
        // any field with a missing length that uses a textfield widget 
        // was intended to be a varchar field, so we will make that change.    
        case 'text_textarea':
          if (empty($field_value['settings']['max_length'])) {
            $field_value['type'] = 'text_long';
          }
          break;
        case 'text_textfield':
          if (empty($field_value['settings']['max_length'])) {
            $field_value['type'] = 'text';
            $field_value['settings']['max_length'] = 255;
            $field_value['messages'][] = t("Invalid field/widget combination: The field '@field' in the bundle '@bundle' is an unlimited length field using a textfield widget, not allowed in D7. The field length will be set to 255.", array('@field' => $field_value['field_name'], '@bundle' => $instance_value['bundle']));
          }
          break;

      }

      // The allowed values list should now be stored as an array.
      switch ($field_value['type']) {
        case 'list_text':
        case 'list_boolean':
          $allowed_values = array();
          if (!empty($field_value['settings']['allowed_values'])) {
            $allowed_values = content_migrate_extract_allowed_values($field_value['settings']['allowed_values'], $field_value['type']);
          }
          $field_value['settings']['allowed_values'] = $allowed_values;
          break;

      }

      break;
  }

}

/**
 * Implements hook_content_migrate_instance_alter().
 *
 * Use this to tweak the conversion of instance or widget settings
 * from the D6 style to the D7 style for specific
 * situations not handled by basic conversion, as when
 * formatter or widget names or settings are changed.
 */
function content_migrate_text_instance_alter(&$instance_value, $field_value) {
  switch ($field_value['module']) {
    case 'text':

      // The text processing setting got switched from a field setting to an instance setting.
      $instance_value['settings']['text_processing'] = $field_value['settings']['text_processing'];

      // The formatter names changed, all are prefixed
      // with 'text_'.
      foreach ($instance_value['display'] as $context => $settings) {
        $instance_value['display'][$context]['type'] = 'text_'. $settings['type'];
      }
      break;
  }

}


/**
 * Implements hook_content_migrate_data_record_alter().
 *
 * Tweaks individual records in a field.
 */
function content_migrate_text_data_record_alter(&$record, $field, $instance) {
  $existing_formats = &drupal_static(__FUNCTION__);
  if (empty($existing_formats)) {
    $existing_formats = db_query("SELECT format FROM {filter_format}")->fetchCol();
  }

  switch($field_value['module']) {
    case 'text':
      if (!empty($instance['settings']['text_processing'])) {
        // Adapted from node_update_7006(), to adjust the filter format values 
        // from the old numeric to the new text formats. Ensure we have a valid value.
        if (!in_array($record[$field['field_name'] . '_format'], $existing_formats)) {
          $record[$field['field_name'] . '_format'] = variable_get('filter_default_format', 1);
        }
      }
  }
}