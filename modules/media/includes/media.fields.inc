<?php

/**
 * @file: Provides a "Multimedia asset" field to the fields API
 */

/**
 * Implement hook_field_info().
 */
function media_field_info() {
  return array(
    'media' => array(
      'label' => t('Multimedia asset (deprecated)'),
      'description' => t('This field stores a reference to a multimedia asset.'),
      'settings' => array(),
      'instance_settings' => array(
        'file_extensions' => media_variable_get('file_extensions'),
      ),
      'default_widget' => 'media_generic',
      'default_formatter' => 'media_large',
      'property_type' => 'field_item_file',
      'property_callbacks' => array('entity_metadata_field_file_callback'),
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function media_field_is_empty($item, $field) {
  return empty($item['fid']);
}

/**
 * Implements hook_field_instance_settings_form().
 */
function media_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];

  // Make the extension list a little more human-friendly by comma-separation.
  $extensions = str_replace(' ', ', ', $settings['file_extensions']);
  $form['file_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed file extensions for uploaded files'),
    '#default_value' => $extensions,
    '#description' => t('Separate extensions with a space or comma and do not include the leading dot.'),
    '#element_validate' => array('_file_generic_settings_extensions'),
    // By making this field required, we prevent a potential security issue
    // that would allow files of any type to be uploaded.
    '#required' => TRUE,
    '#maxlength' => 255,
  );

  return $form;
}

/**
 * Implement hook_field_widget_info().
 */
function media_field_widget_info() {
  return array(
    'media_generic' => array(
      'label' => t('Media file selector'),
      'field types' => array('media', 'file', 'image'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'allowed_types' => array('image'),
        'allowed_schemes' => array('public', 'private'),
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_info().
 */
function media_field_formatter_info() {
  $formatters = array(
    'media_large_icon' => array(
      'label' => t('Large filetype icon'),
      'field types' => array('file'),
    ),
    // This was originally used when media entities contained file fields. The
    // current file entity architecture no longer needs this, but people may
    // have used this formatter for other file fields on their website.
    // @todo Some day, remove this.
    'media' => array(
      'label' => t('Media'),
      'field types' => array('media'),
      'settings' => array('file_view_mode' => 'default'),
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function media_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();

  if ($display['type'] == 'media') {
    $entity_info = entity_get_info('file');
    $options = array('default' => t('Default'));
    foreach ($entity_info['view modes'] as $file_view_mode => $file_view_mode_info) {
      $options[$file_view_mode] = $file_view_mode_info['label'];
    }
    $element['file_view_mode'] = array(
      '#title' => t('File view mode'),
      '#type' => 'select',
      '#default_value' => $settings['file_view_mode'],
      '#options' => $options,
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function media_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';

  if ($display['type'] == 'media') {
    $entity_info = entity_get_info('file');
    $file_view_mode_label = isset($entity_info['view modes'][$settings['file_view_mode']]) ? $entity_info['view modes'][$settings['file_view_mode']]['label'] : t('Default');
    $summary = t('File view mode: @view_mode', array('@view_mode' => $file_view_mode_label));
  }

  return $summary;
}

/**
 * Implements hook_field_prepare_view().
 */
function media_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // Collect all file IDs that need loading.
  $fids = array();
  foreach ($entities as $id => $entity) {
    // Load the files from the files table.
    foreach ($items[$id] as $delta => $item) {
      if (!empty($item['fid'])) {
        $fids[] = $item['fid'];
      }
    }
  }

  // Load the file entities.
  $files = file_load_multiple($fids);

  // Add the loaded file entities to the field item array.
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      // If the file does not exist, mark the entire item as empty.
      if (empty($files[$item['fid']])) {
        unset($items[$id][$delta]);
      }
      else {
        $items[$id][$delta]['file'] = $files[$item['fid']];
      }
    }
  }
}

/**
 * Implement hook_field_formatter_view().
 */
function media_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  if ($display['type'] == 'media_large_icon') {
    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#theme' => 'media_formatter_large_icon',
        '#file' => (object) $item,
      );
    }
  }

  // Legacy support for the extra formatter added to file fields. See
  // media_field_formatter_info().
  if ($display['type'] == 'media') {
    $files = array();
    foreach ($items as $delta => $item) {
      if (!empty($item['file'])) {
        $files[$item['fid']] = $item['file'];
      }
    }

    if (!empty($files)) {
      $element = file_view_multiple($files, $display['settings']['file_view_mode'], 0, $langcode);
    }
  }

  return $element;
}

/**
 * Implement hook_field_widget_settings_form().
 */
function media_field_widget_settings_form($field, $instance) {

  $widget = $instance['widget'];
  $settings = $widget['settings'];
  $form = array();

  // Setup type selection form
  $types = media_type_get_types();
  $options = array();
  foreach ($types as $key => $definition) {
    $options[$key] = $definition->label;
  }

  $streams = file_get_stream_wrappers();

  $form['allowed_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed remote media types'),
    '#options' => $options,
    '#default_value' => $settings['allowed_types'],
    '#description' => t('Media types which are allowed for this field when using remote streams.'),
    '#weight' => 1,
    '#access' => count(file_get_stream_wrappers(STREAM_WRAPPERS_LOCAL)) != count($streams),
  );

  $options = array();
  unset($streams['temporary']);
  foreach ($streams as $scheme => $data) {
    $options[$scheme] = t('@scheme (@name)', array('@scheme' => $scheme . '://', '@name' => $data['name']));
  }
  $form['allowed_schemes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed URI schemes'),
    '#options' => $options,
    '#default_value' => $settings['allowed_schemes'],
    '#description' => t('URI schemes include public:// and private:// which are the Drupal files directories, and may also refer to remote sites.'),
    '#weight' => 2,
  );
  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function media_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $field_settings = $instance['settings'];
  $widget_settings = $instance['widget']['settings'];

  // @todo The Field API supports automatic serialization / unserialization, so
  //   this should no longer be needed. After verifying with a module that uses
  //   the 'data' column, remove this.
  // @see media_field_widget_value()
  $current_value = array();
  if (isset($items[$delta])) {
    $current_value = $items[$delta];
    // @todo $items[$delta] is sometimes a loaded media entity (an object)
    //   rather than an array. This conflicts with Field API expectations (for
    //   example, it results in fatal errors when previewing a node with a
    //   multi-valued media field), so should be fixed. In the meantime, don't
    //   assume that $current_value is an array.
    if (is_array($current_value) && isset($current_value['data']) && is_string($current_value['data'])) {
      $current_value['data'] = unserialize($current_value['data']);
    }
  }

  $element += array(
    '#type' => 'media', // Would like to make this a fieldset, but throws some weird warning about element_children... not sure what it is about yet.
    '#collapsed' => TRUE,
    '#default_value' => $current_value,
    '#required' => $instance['required'],
    '#media_options' => array(
      'global' => array(
        'types' => array_filter($widget_settings['allowed_types']),
        'schemes' => $widget_settings['allowed_schemes'],
        'file_directory' => isset($field_settings['file_directory']) ? $field_settings['file_directory'] : '',
        'file_extensions' => isset($field_settings['file_extensions']) ? $field_settings['file_extensions'] : media_variable_get('file_extensions'),
        'max_filesize' => isset($field_settings['max_filesize']) ? $field_settings['max_filesize'] : 0,
        'uri_scheme' => !empty($field['settings']['uri_scheme']) ? $field['settings']['uri_scheme'] : file_default_scheme(),
      ),
    ),
  );

  if ($field['type'] == 'file') {
    $element['display'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
  }

  // Add image field specific validators.
  if ($field['type'] == 'image') {
    if ($field_settings['min_resolution'] || $field_settings['max_resolution']) {
      $element['#media_options']['global']['min_resolution'] = $field_settings['min_resolution'];
      $element['#media_options']['global']['max_resolution'] = $field_settings['max_resolution'];
    }
  }

  return $element;
}

/**
 * Implements hook_field_validate().
 *
 * Possible error codes:
 * - 'media_remote_file_type_not_allowed': The remote file is not an allowed
 *   file type.
 */
function media_field_validate($obj_type, $object, $field, $instance, $langcode, $items, &$errors) {
  $allowed_types = array_keys(array_filter($instance['widget']['settings']['allowed_types']));

    // @TODO: merge in stuff from media_uri_value
  foreach ($items as $delta => $item) {
    if (empty($item['fid'])) {
      return TRUE;
      //@TODO: make support for submiting with just a URI here?
    }

    $file = file_load($item['fid']);

    // Only validate allowed types if the file is remote and not local.
    if (!file_entity_file_is_local($file)) {
      if (!in_array($file->type, $allowed_types)) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'media_remote_file_type_not_allowed',
          'message' => t('%name: Only remote files with the following types are allowed: %types-allowed.', array('%name' => t($instance['label']), '%types-allowed' => !empty($allowed_types) ? implode(', ', $allowed_types) : t('no file types selected'))),
        );
      }
    }
  }
}

/**
 * Implements_hook_field_widget_error().
 */
function media_field_widget_error($element, $error, $form, &$form_state) {
  form_error($element['fid'], $error['message']);
}

/**
 * @todo Is this function ever called? If not, remove it. The Field API now
 *   supports automatic serialization / unserialization, so this should no
 *   longer be needed. After verifying with a module that uses the 'data'
 *   column, remove this.
 *
 * @see media_field_widget_form()
 */
function media_field_widget_value($element, $input, $form_state) {
  $return = $input;

  if (!is_array($return)) {
    $return = array();
  }

  if (isset($return['data'])) {
    $return['data'] = serialize($return['data']);
  }

  $return += array(
    'fid' => 0,
    'title' => '',
    'data' => NULL,
  );

  return $return;
}

/**
 * @todo The following hook_field_(insert|update|delete|delete_revision)
 *   implementations are nearly identical to the File module implementations of
 *   the same field hooks. The only differences are:
 *   - We pass 'media' rather than 'file' as the module argument to the
 *     file_usage_(add|delete)() functions.
 *   - We do not delete the file / media entity when its usage count goes to 0.
 *   We should submit a core patch to File module to make it flexible with
 *   respect to the above, so that we can reuse its implementation rather than
 *   duplicating it.
 */

/**
 * Implements hook_field_insert().
 */
function media_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Add a new usage of each uploaded file.
  foreach ($items as $item) {
    $file = (object) $item;
    file_usage_add($file, 'media', $entity_type, $id);
  }
}

/**
 * Implements hook_field_update().
 *
 * Checks for files that have been removed from the object.
 */
function media_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // On new revisions, all files are considered to be a new usage and no
  // deletion of previous file usages are necessary.
  if (!empty($entity->revision)) {
    foreach ($items as $item) {
      $file = (object) $item;
      file_usage_add($file, 'media', $entity_type, $id);
    }
    return;
  }

  // Build a display of the current FIDs.
  $current_fids = array();
  foreach ($items as $item) {
    $current_fids[] = $item['fid'];
  }

  // Compare the original field values with the ones that are being saved.
  $original_fids = array();
  if (!empty($entity->original->{$field['field_name']}[$langcode])) {
    foreach ($entity->original->{$field['field_name']}[$langcode] as $original_item) {
      $original_fids[] = $original_item['fid'];
      if (isset($original_item['fid']) && !in_array($original_item['fid'], $current_fids)) {
        // Decrement the file usage count by 1.
        $file = (object) $original_item;
        file_usage_delete($file, 'media', $entity_type, $id, 1);
      }
    }
  }

  // Add new usage entries for newly added files.
  foreach ($items as $item) {
    if (!in_array($item['fid'], $original_fids)) {
      $file = (object) $item;
      file_usage_add($file, 'media', $entity_type, $id);
    }
  }
}

/**
 * Implements hook_field_delete().
 */
function media_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Delete all file usages within this entity.
  foreach ($items as $delta => $item) {
    $file = (object) $item;
    file_usage_delete($file, 'media', $entity_type, $id, 0);
  }
}

/**
 * Implements hook_field_delete_revision().
 */
function media_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, &$items) {
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  foreach ($items as $delta => $item) {
    // @TODO: Not sure if this is correct
    $file = (object)$item;
    if (file_usage_delete($file, 'media', $entity_type, $id, 1)) {
      $items[$delta] = NULL;
    }
  }
}

/**
 * Implements hook_field_instance_update().
 */
function media_field_update_instance($instance, $prior_instance) {
  // Clear the filter cache when updating instance settings for a media entity.
  if ($instance['entity_type'] == 'media') {
    media_filter_invalidate_caches();
  }
}
