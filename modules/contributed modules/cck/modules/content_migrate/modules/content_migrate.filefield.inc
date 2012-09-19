<?php
/**
 * @file content_migrate.filefield.inc
 * Code to implement hook_content_migrate_field_alter, content_migrate_instance_alter() and content_migrate_data_record_alter()
 * on behalf of the former filefield and imagefield modules, moved into a separate file for efficiency.
 */

/**
 * Implements hook_content_migrate_field_alter().
 * 
 * Use this to tweak the conversion of field settings
 * from the D6 style to the D7 style for specific
 * situations not handled by basic conversion,
 * as when field types or settings are changed.
 */
function content_migrate_filefield_field_alter(&$field_value, $instance_value) {

  // There are a bunch of custom imagefield widgets. If they at least start the widget name with 'imagefield' this will work.
  if (substr($instance_value['widget']['type'], 0, 10) == 'imagefield') {
    // Module names and types changed.
    $field_value['messages'][] = t("Changed field type: The '@field' field type will be changed from '@type' to 'image'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
    $field_value['module'] = 'image';
    $field_value['type'] = 'image';
    // default_image is now a field setting.
    $field_value['settings']['default_image'] = $instance_value['widget']['settings']['default_image'];
  }
  // There are a bunch of custom filefield widgets. If they at least start the widget name with 'filefield' this will work.
  if (substr($instance_value['widget']['type'], 0, 9) == 'filefield') {
    // Module names and types changed.
    $field_value['messages'][] = t("Changed field type: The '@field' field type will be changed from '@type' to 'file'.", array('@type' => $field_value['type'], '@field' => $field_value['field_name'], '@widget' => $instance_value['widget']['type']));
    $field_value['module'] = 'file';
    $field_value['type'] = 'file';
    // Some settings have changed names.
    $field_value['settings']['display_field'] = $field_value['settings']['list_field'];
    unset($field_value['settings']['list_field']);
    $field_value['settings']['display_default'] = $field_value['settings']['list_default'];
    unset($field_value['settings']['list_default']);
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
function content_migrate_filefield_instance_alter(&$instance_value, $field_value) {
  // Translate formatters.
  // There are a bunch of custom imagefield / filefield widgets. If they at
  // least start the widget name with 'imagefield' / 'filefield', we can detect
  // them and translate formatters accordingly
  if (substr($instance_value['widget']['type'], 0, 10) == 'imagefield') {
    foreach ($instance_value['display'] as $context => $settings) {
      switch ($settings['type']) {
        case 'image_plain':
          $instance_value['display'][$context]['type'] = 'image';
          $instance_value['display'][$context]['module'] = 'image';
          $instance_value['display'][$context]['settings'] = array(
            'image_style' => '',
            'image_link' => '',
          );
          break;

        case 'image_nodelink':
          $instance_value['display'][$context]['type'] = 'image';
          $instance_value['display'][$context]['module'] = 'image';
          $instance_value['display'][$context]['settings'] = array(
            'image_style' => '',
            'image_link' => 'content',
          );
          break;

        case 'image_imagelink':
          $instance_value['display'][$context]['type'] = 'image';
          $instance_value['display'][$context]['module'] = 'image';
          $instance_value['display'][$context]['settings'] = array(
            'image_style' => '',
            'image_link' => 'file',
          );
          break;
      }
    }
  }
  if (substr($instance_value['widget']['type'], 0, 9) == 'filefield') {
    foreach ($instance_value['display'] as $context => $settings) {
      // Note; the 'path_plain' D6 formatter has disappeared.
      if (in_array($settings['type'], array('default', 'url_plain'))) {
        $instance_value['display'][$context]['type'] = 'file_'. $settings['type'];
        $instance_value['display'][$context]['moduke'] = 'file';
      }
    }
  }

  // Translate the original imagefield and fielfield widgets.
  switch ($instance_value['widget']['module']) {
  
    case 'imagefield':
      // Module names and types changed.
      $instance_value['widget']['module'] = 'image';
      $instance_value['widget']['type'] = 'image';

      // Most settings became instance settings instead of widget settings, with a couple name changes.
      $instance_value['settings']['file_directory'] = isset($instance_value['widget']['settings']['file_path']) ? $instance_value['widget']['settings']['file_path'] : '';
      unset($instance_value['widget']['settings']['file_path']);
      $instance_value['settings']['max_filesize'] = isset($instance_value['widget']['settings']['max_filesize_per_file']) ? $instance_value['widget']['settings']['max_filesize_per_file'] : 0;
      unset($instance_value['widget']['settings']['max_filesize_per_file']);
      $instance_value['settings']['file_extensions'] = isset($instance_value['widget']['settings']['file_extensions']) ? $instance_value['widget']['settings']['file_extensions'] : '';
      unset($instance_value['widget']['settings']['file_extensions']);
      $settings = array(
        'max_resolution' => 0,
        'min_resolution' => 0,
      );
      foreach ($settings as $setting => $default_value) {
        $instance_value['settings'][$setting] = isset($instance_value['widget']['settings'][$setting]) ? $instance_value['widget']['settings'][$setting] : $default_value;
        unset($instance_value['widget']['settings'][$setting]);
      }

      // What is the difference between alt and custom_alt on the old field?
      $instance_value['settings']['alt_field'] = $instance_value['widget']['settings']['custom_alt'];
      unset($instance_value['widget']['settings']['custom_alt']);
      $instance_value['settings']['title_field'] = $instance_value['widget']['settings']['custom_title'];
      unset($instance_value['widget']['settings']['custom_title']);

      // Many settings have no place in the new field array.
      unset($instance_value['widget']['settings']['title']);
      unset($instance_value['widget']['settings']['alt']);
      unset($instance_value['widget']['settings']['max_filesize_per_node']);
      unset($instance_value['widget']['settings']['title_type']);
               
      // default_image is now a field setting.
      unset($instance_value['widget']['settings']['default_image']);
      break;

    case 'filefield':
      // Module names and types changed.
      $instance_value['widget']['module'] = 'file';
      $instance_value['widget']['type'] = 'file_generic';

      // Some settings had name changes, or moved from the widget to the
      // instance.
      $instance_value['settings']['file_directory'] = $instance_value['widget']['settings']['file_path'];
      unset($instance_value['widget']['settings']['file_path']);
      $instance_value['settings']['file_extensions'] = $instance_value['widget']['settings']['file_extensions'];
      unset($instance_value['widget']['settings']['file_extensions']);
      $instance_value['settings']['max_filesize'] = $instance_value['widget']['settings']['max_filesize_per_file'];
      unset($instance_value['widget']['settings']['max_filesize_per_file']);

      // This setting is gone.
      unset($instance_value['widget']['settings']['max_filesize_per_node']);

      break;
  }
}


/**
 * Implements hook_content_migrate_data_record_alter().
 *
 * Tweaks individual records in a field.
 */
function content_migrate_filefield_data_record_alter(&$record, $field, $instance) {

  switch($field['type']) {
    case 'image':
      // Map D6 imagefield field columns to D7 image field columns.
      if (!empty($record[$field['field_name'] . '_data']) && ($data = unserialize($record[$field['field_name'] . '_data']))) {
        $record[$field['field_name'] . '_alt'] = $data['alt'];
        $record[$field['field_name'] . '_title'] = $data['title'];
      }
      else {
        unset($record[$field['field_name'] . '_alt']);
        unset($record[$field['field_name'] . '_title']);
      }

    // Fall through.
    case 'file':
      // Map D6 filefield field columns to D7 file field columns.
      if (!empty($record[$field['field_name'] . '_data']) && ($data = unserialize($record[$field['field_name'] . '_data'])) && isset($data['description'])) {
        $record[$field['field_name'] . '_description'] = $data['description'];
      }
      else {
        unset($record[$field['field_name'] . '_description']);
      }

      if (isset($record[$field['field_name'] . '_list'])) {
        $record[$field['field_name'] . '_display'] = $record[$field['field_name'] . '_list'];
        unset($record[$field['field_name'] . '_list']);
      }

      // Copies imagefield data from the old 'files' table into 'files_managed' and sets file_usage
      // Mostly copied from system_update_7061, which does the same for the D6 core 'upload' module

      $nid = $record['entity_id'];
      $fid = $record[$field['field_name'] . '_fid'];

      if (empty($fid)) {
        $record = NULL;
        return;
      };

      $file = db_select('files', 'f')->fields('f', array('fid', 'uid', 'filename', 'filepath', 'filemime', 'filesize', 'timestamp', 'status'))->condition('fid', $fid)->execute()->fetchObject();

      $basename = variable_get('file_directory_path', conf_path() . '/files');
      $scheme = file_default_scheme() . '://';

      // We will convert filepaths to uri using the default scheme
      // and stripping off the existing file directory path.
      $file->uri = $scheme . str_replace($basename, '', $file->filepath);
      $file->uri = file_stream_wrapper_uri_normalize($file->uri);
      unset($file->filepath);

      // Insert into the file_managed table.
      // Each fid should only be stored once in file_managed.
      db_merge('file_managed')
        ->key(array(
          'fid' => $file->fid,
        ))
        ->fields(array(
          'uid' => $file->uid,
          'filename' => $file->filename,
          'uri' => $file->uri,
          'filemime' => $file->filemime,
          'filesize' => $file->filesize,
          'status' => $file->status,
          'timestamp' => $file->timestamp,
        ))
        ->execute();

      // Add the usage entry for the file.
      file_usage_add($file, 'file', 'node', $nid);
      break;
  }
}