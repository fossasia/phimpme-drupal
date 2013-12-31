<?php
/**
 * @file content_migrate.number.inc
 * Code to implement hook_content_migrate_field_alter, content_migrate_instance_alter() and content_migrate_data_record_alter()
 * on behalf of the former number module, moved into a separate file for efficiency.
 */

/**
 * Implements hook_content_migrate_field_alter().
 * 
 * Use this to tweak the conversion of field settings
 * from the D6 style to the D7 style for specific
 * situations not handled by basic conversion,
 * as when field types or settings are changed.
 */
function content_migrate_number_field_alter(&$field_value, $instance_value) {
  switch ($field_value['type']) {
   
    case 'number_integer':
    case 'number_decimal':
    case 'number_float':  
      
      // Changed name of setting from 'decimal' to 
      // 'decimal_separator'.
      if (isset($field_value['settings']['decimal'])) {
        $field_value['settings']['decimal_separator'] = $field_value['settings']['decimal'];
        unset($field_value['settings']['decimal']);
      }
      // Add a decimal_separator setting to floats.
      if ($field_value['type'] == 'number_float') {
        $field_value['settings']['decimal_separator'] = '.';
      }
      
      // Number fields using optionwidgets are
      // now List fields.
      switch ($instance_value['widget']['type']) {
        case 'optionwidgets_buttons':
        case 'optionwidgets_select':
          if ($field_value['type'] == 'number_integer') {
            $field_value['messages'][] = t("Changed field type: The '@field' field uses a '@widget' widget. The field type will be changed from '@type' to 'list_integer'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
            $field_value['type'] = 'list_integer';
          }
          else {
            $field_value['messages'][] = t("Changed field type: The '@field' field uses a '@widget' widget. The field type will be changed from '@type' to 'list_float'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
            $field_value['type'] = 'list_float';
          }
          $field_value['module'] = 'list';
          break;
        case 'optionwidgets_onoff':
          $field_value['messages'][] = t("Changed field type: The '@field' field uses a '@widget' widget. The field type will be changed from '@type' to 'list_boolean'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
          $field_value['type'] = 'list_boolean';
          $field_value['module'] = 'list';
          break;
      }

      // The allowed values list should now be stored as an array.
      $allowed_values = array();
      if (!empty($field_value['settings']['allowed_values'])) {
        $allowed_values = content_migrate_extract_allowed_values($field_value['settings']['allowed_values'], $field_value['type']);
      }
      $field_value['settings']['allowed_values'] = $allowed_values;
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
function content_migrate_number_instance_alter(&$instance_value, $field_value) {
  switch ($field_value['module']) {
    case 'number':
      // The number formatters and formatter settings
      // have changed.
      $new_type = array(
        'unformatted' => 'number_unformatted',
        'default' => $field_value['type'] == 'number_integer' ? 'number_integer' : 'number_decimal',
        'us_0' => 'number_integer',
        'us_1' => 'number_decimal',
        'us_2' => 'number_decimal',
        'be_0' => 'number_integer',
        'be_1' => 'number_decimal',
        'be_2' => 'number_decimal',        
        'fr_0' => 'number_integer',
        'fr_1' => 'number_decimal',
        'fr_2' => 'number_decimal',      
      );
      $new_settings = array(
        'default' => array(
          'thousand_separator' => '',
          'decimal_separator' => '.',
          'scale' => 0,
          'prefix_suffix' => TRUE,
        ),
        'us_0' => array(
          'thousand_separator' => ',',
          'decimal_separator' => '.',
          'scale' => 0,
          'prefix_suffix' => TRUE,
        ),        
        'us_1' => array(
          'thousand_separator' => ',',
          'decimal_separator' => '.',
          'scale' => 1,
          'prefix_suffix' => TRUE,
        ),
        'us_2' => array(
          'thousand_separator' => ',',
          'decimal_separator' => '.',
          'scale' => 2,
          'prefix_suffix' => TRUE,
        ),  
        'be_0' => array(
          'thousand_separator' => '',
          'decimal_separator' => ',',
          'scale' => 0,
          'prefix_suffix' => TRUE,
        ),        
        'be_1' => array(
          'thousand_separator' => '.',
          'decimal_separator' => ',',
          'scale' => 1,
          'prefix_suffix' => TRUE,
        ),
        'be_2' => array(
          'thousand_separator' => '.',
          'decimal_separator' => ',',
          'scale' => 2,
          'prefix_suffix' => TRUE,
        ),         
        'fr_0' => array(
          'thousand_separator' => '',
          'decimal_separator' => ', ',
          'scale' => 0,
          'prefix_suffix' => TRUE,
        ),        
        'fr_1' => array(
          'thousand_separator' => ' ',
          'decimal_separator' => ', ',
          'scale' => 1,
          'prefix_suffix' => TRUE,
        ),
        'fr_2' => array(
          'thousand_separator' => ' ',
          'decimal_separator' => ', ',
          'scale' => 2,
          'prefix_suffix' => TRUE,
        ),
      );
      foreach ($instance_value['display'] as $context => $settings) {
        if (array_key_exists($settings['type'], $new_type)) {
          $instance_value['display'][$context]['type'] = $new_type[$settings['type']];
          $instance_value['display'][$context]['settings'] = $new_settings[$settings['type']];
        }
      }

      // Min, max, prefix, and suffix moved to instance settings.
      $instance_value['settings']['min'] = $field_value['settings']['min'];
      $instance_value['settings']['max'] = $field_value['settings']['max'];
      $instance_value['settings']['prefix'] = $field_value['settings']['prefix'];
      $instance_value['settings']['suffix'] = $field_value['settings']['suffix'];
      break;
  }
}
