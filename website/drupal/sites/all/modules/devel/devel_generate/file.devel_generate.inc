<?php

function file_devel_generate($object, $field, $instance, $bundle) {
  if (field_behaviors_widget('multiple values', $instance) == FIELD_BEHAVIOR_CUSTOM) {
    return devel_generate_multiple('_file_devel_generate', $object, $field, $instance, $bundle);
  }
  else {
    return _file_devel_generate($object, $field, $instance, $bundle);
  }
}

function _file_devel_generate($object, $field, $instance, $bundle) {
  static $file;

  if (empty($file)) {
    if ($path = devel_generate_textfile()) {
      $source->uri = $path;
      $source->uid = 1; // TODO: randomize? use case specific.
      $source->filemime = 'text/plain';
      $destination = $field['settings']['uri_scheme'] . '://' . $instance['settings']['file_directory'] . '/' . basename($path);
      $file = file_move($source, $destination);
    }
    else {
      return FALSE;
    }
  }
  $object_field['fid'] = $file->fid;
  $object_field['display'] = $field['settings']['display_default'];
  $object_field['description'] = devel_create_greeking(10);

  return $object_field;
}

/**
 * Private function for generating a random text file.
 */
function devel_generate_textfile($filesize = 1024) {
  if ($tmp_file = drupal_tempnam('temporary://', 'filefield_')) {
    $destination = $tmp_file . '.txt';
    file_unmanaged_move($tmp_file, $destination);

    $fp = fopen($destination, 'w');
    fwrite($fp, str_repeat('01', $filesize/2));
    fclose($fp);

    return $destination;
  }
}
