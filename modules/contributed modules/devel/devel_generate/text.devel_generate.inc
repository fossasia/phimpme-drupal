<?php

function text_devel_generate($object, $field, $instance, $bundle) {
  if (field_behaviors_widget('multiple values', $instance) == FIELD_BEHAVIOR_CUSTOM) {
    return devel_generate_multiple('_text_devel_generate', $object, $field, $instance, $bundle);
  }
  else {
    return _text_devel_generate($object, $field, $instance, $bundle);
  }
}

function _text_devel_generate($object, $field, $instance, $bundle) {
  $object_field = array();
  if (!empty($instance['settings']['text_processing'])) {
    $formats = filter_formats();
    $format = array_rand($formats);
  }
  else {
    $format = filter_fallback_format();
  }

  if ($instance['widget']['type'] != 'text_textfield') {
    // Textarea handling
    $object_field['value'] = devel_create_content($format);
    if ($instance['widget']['type'] == 'text_textarea_with_summary' && !empty($instance['display_summary'])) {
      $object_field['summary'] = devel_create_content($format);
    }
  }
  else {
    // Textfield handling.
    // Generate a value that respects max_length.
    if (empty($field['settings']['max_length'])) {
      $field['settings']['max_length'] = 12;
    }
    $object_field['value'] = user_password($field['settings']['max_length']);
  }
  $object_field['format'] = $format;
  return $object_field;
}
