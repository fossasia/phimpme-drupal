<?php

/**
 * @file
 * Field API integration for media_gallery.module.
 */


/**
 * Implements hook_field_formatter_info().
 */
function media_gallery_field_formatter_info() {
  return array(
    'media_gallery' => array(
      'label' => t('Gallery media'),
      'field types' => array('media'),
      'settings' => array('file_view_mode' => 'media_gallery_thumbnail'),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function media_gallery_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();

  if ($display['type'] == 'media_gallery') {
    $element['file_view_mode'] = array(
      '#title' => t('File view mode'),
      '#type' => 'select',
      '#default_value' => $settings['file_view_mode'],
      '#options' => media_gallery_file_view_modes(),
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function media_gallery_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';

  if ($display['type'] == 'media_gallery') {
    $entity_info = entity_get_info('file');
    $file_view_mode_label = isset($entity_info['view modes'][$settings['file_view_mode']]) ? $entity_info['view modes'][$settings['file_view_mode']]['label'] : t('Default');
    $summary = t('File view mode: @view_mode', array('@view_mode' => $file_view_mode_label));
  }

  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function media_gallery_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  if (!$items) {
    return $element;
  }
  $file_view_mode = $display['settings']['file_view_mode'];

  // The Formatter Reference module allows per-entity, rather than
  // per-bundle, formatter assignment, but it only works when the field being
  // formatted and the reference field are part of the same entity. For media
  // galleries, we want to enable formatter reference fields on the gallery node
  // to be used as formatters for fields within the file entities, so we pass
  // along those field values to them. Here we just collect all formatter
  // reference field names for the entity.
  $formatter_reference_fields = array();
  if (module_exists('formatter_reference')) {
    $fields = field_info_fields();
    list(, , $bundle) = entity_extract_ids($entity_type, $entity);
    foreach (field_info_instances($entity_type, $bundle) as $field_name => $instance_info) {
      if ($fields[$field_name]['type'] == 'formatter_reference') {
        $formatter_reference_fields[] = $field_name;
      }
    }
  }

  // Prepare the referenced file entities for viewing.
  $files = array();
  foreach ($items as $delta => $item) {
    $file = $item['file'];

    // Pass along formatter reference field values from the gallery to the file.
    foreach ($formatter_reference_fields as $field_name) {
      if (!isset($file->{$field_name}) && isset($entity->{$field_name})) {
        $file->{$field_name} = $entity->{$field_name};
      }
    }

    // Set a default value for the file's media_title field.
    // @todo Eventually, fix this to take into account the possibility of a
    //   multilingual media title field.
    if (isset($file->media_title) && !isset($file->media_title[LANGUAGE_NONE][0]['value'])) {
      $file->media_title[LANGUAGE_NONE][0]['value'] = _media_gallery_get_media_title($file);
    }

    $files[$file->fid] = $file;
  }
  field_attach_prepare_view('file', $files, $file_view_mode);
  entity_prepare_view('file', $files);

  // View each file. We don't use file_view_multiple(), because we need the
  // render array indexed by $delta rather than by file id.
  foreach ($items as $delta => $item) {
    $element[$delta] = file_view($item['file'], $file_view_mode, $langcode);
    $element[$delta]['#media_gallery_entity_type'] = $entity_type;
    $element[$delta]['#media_gallery_entity'] = $entity;
    switch ($file_view_mode) {
      case 'media_gallery_thumbnail':
        $element[$delta]['#theme'] = 'media_gallery_media_item_thumbnail';
        break;
      case 'media_gallery_lightbox':
        $element[$delta]['#theme'] = 'media_gallery_media_item_lightbox';
        break;
      case 'media_gallery_detail':
        $element[$delta]['#theme'] = 'media_gallery_media_item_detail';
        break;
      case 'media_gallery_block_thumbnail':
        $element[$delta]['#theme'] = 'media_gallery_block_thumbnail';
        break;
      case 'media_gallery_collection_thumbnail':
        $element[$delta]['#theme'] = 'media_gallery_collection_thumbnail';
        break;
    }
  }

  return $element;
}

/**
 * Constructs a drupal_render() array for a media entity displayed within a gallery.
 *
 * During normal gallery node display, the Field Attach API ensures that all of
 * its fields get rendered. Sometimes, however, we need to display just a single
 * media entity (one item from the multi-valued "media_gallery_media" field)
 * without displaying any other media entities within that field, and without
 * displaying any other gallery node fields, but via a field formatter.
 *
 * @see media_gallery_detail_page()
 * @see media_gallery_lightbox_page()
 */
function media_gallery_item_view($gallery_node, $file, $file_view_mode) {
  $display = array('type' => 'media_gallery', 'settings' => array('file_view_mode' => $file_view_mode));
  $id = $gallery_node->nid;
  $field_name = 'media_gallery_media';
  $field = field_info_field($field_name);
  $instance = field_info_instance('node', $field_name, 'media_gallery');
  $langcode = key($gallery_node->{$field_name});
  $items = array(0 => array('fid' => $file->fid, 'file' => $file));
  $items_multi = array($id => &$items); // Taken by reference, so can't inline.
  $formatter = field_info_formatter_types($display['type']);
  // "prepare_view" function is optional.
  $function = $formatter['module'] . '_field_formatter_prepare_view';
  if (function_exists($function)) {
    $function('node', array($id => $gallery_node), $field, array($id => $instance), $langcode, $items_multi, array($id => $display));
  }
  // "view" function is mandatory, so deliberately break if it doesn't exist.
  $function = $formatter['module'] . '_field_formatter_view';
  $content = $function('node', $gallery_node, $field, $instance, $langcode, $items, $display);
  _media_gallery_attach_css_resources($content);
  return $content;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @see formatter_reference_form_field_ui_display_overview_form_alter()
 * @see media_gallery_field_formatter_prepare_view()
 */
function media_gallery_form_field_ui_display_overview_form_alter(&$form) {
  // See formatter_reference_form_field_ui_display_overview_form_alter() for
  // more details about why this form needs to be altered. All we're doing here
  // is allowing a gallery node's formatter reference field to format file
  // entity fields.
  if (module_exists('formatter_reference') && $form['#entity_type'] == 'file') {
    $gallery_field_instances = field_info_instances('node', 'media_gallery');
    $fields = field_info_fields();
    foreach ($form['#fields'] as $field_name) {
      // Iterate all the formatter options, not just what's in
      // $form['settings'][$field_name]['type']['#options'], in case
      // formatter_reference_form_field_ui_display_overview_form_alter() ran
      // first, and already removed the option.
      foreach (field_ui_formatter_options($fields[$field_name]['type']) as $formatter_name => $formatter_label) {
        $parts = explode('__', $formatter_name);
        if (count($parts) == 2 && $parts[0] == 'formatter_reference_router') {
          $formatter_reference_field_name = $parts[1];
          // If the formatter reference field exists in the 'media_gallery' node
          // bundle, then ensure it's an option within the media entity. But in
          // the edge case that the same reference field also exists in the
          // media entity, then that takes priority, so don't change its label.
          if (isset($gallery_field_instances[$formatter_reference_field_name]) && !field_info_instance($form['#entity_type'], $formatter_reference_field_name, $form['#bundle'])) {
            $form['settings'][$field_name]['type']['#options'][$formatter_name] = t('Value of field "@field" in the containing gallery', array('@field' => $gallery_field_instances[$formatter_reference_field_name]['label']));
          }
        }
      }
    }
  }
}

/**
 * Gets the title of a media entity either from the media_title field or based on the filename.
 */
function _media_gallery_get_media_title($file) {
  // If the entity has a value for the title field, use it.
  if (isset($file->media_title[LANGUAGE_NONE][0]['value'])) {
    $title = $file->media_title[LANGUAGE_NONE][0]['value'];
  }
  // Otherwise, base it on the file's filename, but with some adjustments for
  // human-friendly display.
  else {
    $replacements = array(
      '/\..*/' => '',                     // Remove first "." and everything after.
      '/[^a-zA-Z0-9]+/' => ' ',           // Replace non letters or numbers with a single space.
      '/([a-z])([A-Z])/' => '\1 \2',      // Insert a space between a lowercase letter and an uppercase letter.
      '/([a-zA-Z])([0-9])/' => '\1 \2',   // Insert a space between a letter and a number.
      '/([0-9])([a-zA-Z])/' => '\1 \2',   // Insert a space between a number and a letter.
    );
    // In addition to above replacements, also capitalize the first letter of
    // each word, and remove leading and trailing spaces.
    $title = trim(ucwords(preg_replace(array_keys($replacements), array_values($replacements), $file->filename)));
  }
  return $title;
}

/**
 * Allowed values callback for media_gallery_format list field.
 */
function _media_gallery_get_format_values() {
  return array(
    'node' => t('Show media on a full page'),
    'lightbox' => t('Show media in a lightbox'),
  );
}

/**
 * Allowed values callback for media_gallery_lightbox_extras list field.
 */
function _media_gallery_get_lightbox_extras_values() {
  return array(
    0 => t('Do not show title and description'),
    1 => t('Show title and description'),
  );
}

/**
 * Allowed values callback for media_gallery_image_info list field.
 */
function _media_gallery_get_image_info_values() {
  return array(
    'nothing' => t('Nothing'),
    'title' => t('Title'),
    'title_license' => t('Title and license'),
  );
}

/**
 * Allowed values callback for media_gallery_image_info_where list field.
 */
function _media_gallery_get_image_info_placement_values() {
  return array(
    'hover' => t('Show title on hover'),
    'below' => t('Show title below'),
    'nothing' => t('Show nothing'),
  );
}

/**
 * Allowed values callback for media_gallery_allow_download list field.
 */
function _media_gallery_get_allow_download_values() {
  return array(
    0 => t('Do not allow downloading of the original image'),
    1 => t('Allow downloading of the original image'),
  );
}

/**
 * Allowed values callback for media_gallery_expose_block list field.
 */
function _media_gallery_get_expose_block_values() {
  return array(
    0 => t('Do not create a block of most recently added media'),
    1 => t('Create a block of most recently added media'),
  );
}

/**
 * Allowed values callback for field_license list field.
 * 
 * @todo: should be moved to media_cc module or something.
 */
function _media_gallery_get_field_license_values() {
  return array(
    'none' => t('None (all rights reserved)'),
    '' => t('-- Creative Commons --'),
    'cc_sa_nc' => t('Attribution, Non-Commercial, Share Alike'),
    'cc_nc' => t('Attribution, Non-Commercial'),
    'cc_nd_nc' => t('Attribution, Non-Commercial, No Derivative Works'),
    'cc' => t('Attribution'),
    'cc_sa' => t('Attribution, Share Alike'),
    'cc_nd' => t('Attribution, No Derivative Works'),
  );
}

/**
 * Allowed values callback for media_gallery_columns list field.
 */
function _media_gallery_get_columns_values() {
  return drupal_map_assoc(array(2, 3, 4, 5, 6, 7, 8, 9, 10));
}

/**
 * Allowed values callback for media_gallery_block_columns list field.
 */
function _media_gallery_get_block_columns_values() {
  return drupal_map_assoc(array(1, 2, 3, 4));
}
