<?php

/**
 * @file
 * Provides info for fields.
 */

/**
 * Implements hook_entity_property_info() on top of field module.
 *
 * @see entity_field_info_alter()
 * @see entity_entity_property_info()
 */
function entity_metadata_field_entity_property_info() {
  $info = array();
  // Loop over all field instances and add them as property.
  foreach (field_info_fields() as $field_name => $field) {
    $field += array('bundles' => array());
    if ($field_type = field_info_field_types($field['type'])) {
      // Add in our default callback as the first one.
      $field_type += array('property_callbacks' => array());
      array_unshift($field_type['property_callbacks'], 'entity_metadata_field_default_property_callback');

      foreach ($field['bundles'] as $entity_type => $bundles) {
        foreach ($bundles as $bundle) {
          $instance = field_info_instance($entity_type, $field_name, $bundle);

          if ($instance && empty($instance['deleted'])) {
            foreach ($field_type['property_callbacks'] as $callback) {
              $callback($info, $entity_type, $field, $instance, $field_type);
            }
          }
        }
      }
    }
  }
  return $info;
}

/**
 * Callback to add in property info defaults per field instance.
 * @see entity_metadata_field_entity_property_info().
 */
function entity_metadata_field_default_property_callback(&$info, $entity_type, $field, $instance, $field_type) {
  if (!empty($field_type['property_type'])) {
    if ($field['cardinality'] != 1) {
      $field_type['property_type'] = 'list<' . $field_type['property_type'] . '>';
    }
    // Add in instance specific property info, if given and apply defaults.
    $name = $field['field_name'];
    $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];
    $instance += array('property info' => array());
    $property = $instance['property info'] + array(
      'label' => $instance['label'],
      'type' => $field_type['property_type'],
      'description' => t('Field "@name"', array('@name' => $name)),
      'getter callback' => 'entity_metadata_field_property_get',
      'setter callback' => 'entity_metadata_field_property_set',
      'access callback' => 'entity_metadata_field_access_callback',
      'query callback' => 'entity_metadata_field_query',
      'translatable' => !empty($field['translatable']),
      // Specify that this property stems from a field.
      'field' => TRUE,
      'required' => !empty($instance['required']),
    );
    // For field types of the list module add in the options list callback.
    if (strpos($field['type'], 'list') === 0) {
      $property['options list'] = 'entity_metadata_field_options_list';
    }
  }
}

/**
 * Additional callback to adapt the property info for text fields. If a text
 * field is processed we make use of a separate data structure so that format
 * filters are available too. For the text value the sanitized, thus processed
 * value is returned by default.
 *
 * @see entity_metadata_field_entity_property_info()
 * @see entity_field_info_alter()
 * @see entity_property_text_formatted_info()
 */
function entity_metadata_field_text_property_callback(&$info, $entity_type, $field, $instance, $field_type) {
  if (!empty($instance['settings']['text_processing']) || $field['type'] == 'text_with_summary') {
    // Define a data structure for dealing with text that is formatted or has
    // a summary.
    $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];

    $property['getter callback'] = 'entity_metadata_field_verbatim_get';
    $property['setter callback'] = 'entity_metadata_field_verbatim_set';
    unset($property['query callback']);

    if (empty($instance['settings']['text_processing'])) {
      $property['property info'] =  entity_property_field_item_textsummary_info();
      // Enable auto-creation of the item, so that it is possible to just set
      // the textual or summary value.
      $property['auto creation'] = 'entity_property_create_array';
    }
    else {
      // For formatted text we use the type name 'text_formatted'.
      $property['type'] = ($field['cardinality'] != 1) ? 'list<text_formatted>' : 'text_formatted';
      $property['property info'] = entity_property_text_formatted_info();
    }
    if ($field['type'] != 'text_with_summary') {
      unset($property['property info']['summary']);
    }
  }
}

/**
 * Additional callback to adapt the property info for term reference fields.
 * @see entity_metadata_field_entity_property_info().
 */
function entity_metadata_field_term_reference_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  if (count($field['settings']['allowed_values']) == 1) {
    $settings = reset($field['settings']['allowed_values']);
    $property['bundle'] = $settings['vocabulary'];
  }
  // Only add the options list callback for controlled vocabularies, thus
  // vocabularies not using the autocomplete widget.
  if ($instance['widget']['type'] != 'taxonomy_autocomplete') {
    $property['options list'] = 'entity_metadata_field_options_list';
  }
  unset($property['query callback']);
}

/**
 * Additional callback to adapt the property info for file fields.
 * @see entity_metadata_field_entity_property_info().
 */
function entity_metadata_field_file_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  // Define a data structure so it's possible to deal with files and their
  // descriptions.
  $property['getter callback'] = 'entity_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_metadata_field_verbatim_set';

  // Auto-create the field $items as soon as a property is set.
  $property['auto creation'] = 'entity_metadata_field_file_create_item';
  $property['validation callback'] = 'entity_metadata_field_file_validate_item';

  $property['property info'] = entity_property_field_item_file_info();

  if (empty($instance['settings']['description_field'])) {
    unset($property['property info']['description']);
  }
  if (empty($field['settings']['display_field'])) {
    unset($property['property info']['display']);
  }
  unset($property['query callback']);
}

/**
 * Additional callback to adapt the property info for image fields.
 * This callback gets invoked after entity_metadata_field_file_callback().
 * @see entity_metadata_field_entity_property_info().
 */
function entity_metadata_field_image_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  // Update the property info with the info for image fields.
  $property['property info'] = entity_property_field_item_image_info();

  if (empty($instance['settings']['alt_field'])) {
    unset($property['property info']['alt']);
  }
  if (empty($field['settings']['title_field'])) {
    unset($property['property info']['title']);
  }
}
