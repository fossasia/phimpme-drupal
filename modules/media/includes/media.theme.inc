<?php

/**
 * @file
 * Media Theming
 *
 * Theming functions for the Media module.
 */

/**
 * Display the media file browser.
 * @TODO this is depreciated I think
 * @param array $element
 *   The form element.
 * @return string
 */
function theme_media_file_browser($element) {
  // Add the CSS for our display.
  $output = '<div class="media browser">' . $element . '</div>';

  return $output;
}

/**
 * Display a media file list.
 * @TODO this is depreciated I think
 * @param array $element
 *   The form element.
 * @return string
 */
function theme_media_file_list($element) {
  // Add the CSS for our display.
  return '<div class="media-file-list">' . theme('form_element', $element, $element['#children']) . '</div>';
}

/**
 * Display a browser pane.
 * @TODO this is depreciated I think
 *
 * @param array $form
 *   The form element.
 * @return string
 */
function theme_media_browser_pane($form) {
  return;
  $output   = array();

  // render the drawers
  $output[] = '<div' . drupal_attributes($form['#attributes']) . '>';

  // render the drawer list
  $output[] = '  <div class="browser drawers">';
  $output[] = drupal_render_form(null, $form['drawers']);
  $output[] = ' </div>';

  // render the drawer displays
  $output[] = drupal_render_form(null, $form);
  $output[] = '</div>';

  return implode("\n", $output);
}


/**
 * Default theming function for creating the browser frame.
 * Assumes an array of file objects as $files and an
 * array of $parameters
 * @param $variables
 *   array of variables
 * @return unknown_type
 */
function theme_media_browser_content_frame($variables) {
  // Pull out all the variables into a usable form
  extract($variables);
  // Did we get any files back?
  if (! count($files)) {
    // @TODO display no files found
  }

  $html = array();

  // On the first invocation, load javascript and build the browser frame
  if ($invoke) {


  }
   // Render the results limiter
  $html[] = theme('media_browser_control_result_limit', array('parameters' => $parameters));
  // Render the actual content
  $form = drupal_get_form('media_file_listing_form', $files, $parameters);
  $html[] = drupal_render($form);

  // Make sure to close the wrapping div
  if ($invoke) {
    $html[] = '</div>';
  }
  return implode("\n", $html);
}



/**
 * Display a item list of files as thumbnails. Implements
 * the admin thumbnail theme for now- serves as a wrapper
 *
 *  @param $files
 *    An array of file objects to display.
 *  @return
 */
function theme_media_browser_thumbnails($variables) {
  $files = $variables['files'];
  $style_name = $variables['style_name'];
  $thumbnails = array();
  foreach ($files as $file) {
    $thumbnails[] = theme('media_admin_thumbnail', array('file' => $file, 'style_name' => $style_name));
  }
  return theme('item_list', array('items' => $thumbnails, 'attributes' => array('class' => 'media_content_navigator results')));
}


/**
 * Theme a thumbnail.
 * @param $variables
 *   array items being passed in
 */
function theme_media_admin_thumbnail($variables) {
  $path = drupal_get_path('module', 'media');
  $file = $variables['file'];
  $style_name = $variables['style_name'];
  if (isset($file)) {
    $file_url = file_create_url($file->uri);
  }
  else {
    return '';
  }
  $output = '';
  if (module_exists('styles')) {
    $thumbnail = theme('styles',
      array(
        'field_type' => 'file',
        'style_name' => $style_name,
        'uri' => $file->uri,
        'description' => t('Thumbnail for !filename.', array('!filename' => $file->filename)),
        'object' => $variables['file'],
    ));
  }
  else {
    // Display a thumbnail for images.
    if (strstr($file->filemime, 'image')) {
      $thumbnail = theme('image_style',
        array(
          'style_name' => 'thumbnail',
          'path' => $file->uri,
          'alt' => t('Thumbnail for !filename.', array('!filename' => $file->filename)),
        )
      );
    }
    // Display the 'unknown' icon for other file types.
    else {
      $thumbnail = theme('image',
        array(
          'path' => $path . '/images/file-unknown.png',
          'alt' => t('Thumbnail for !filename.', array('!filename' => $file->filename)),
          'attributes' => array('class' => 'file-unknown'),
        ));
    }
  }
  $output .= l($thumbnail,
    $file_url,
    array(
      'html' => TRUE,
      'attributes' => array('class' => 'media-thumbnail'),
    ));
  return $output;
}


/**
 * Theme operations for a thumbnail.
 */
function theme_media_admin_thumbnail_operations($variables) {
  $destination = drupal_get_destination();
  $file = $variables['file'];
  $output = l(t('edit'), 'media/' . $file->fid . '/edit', array('query' => $destination));
  return $output;
}

/**
 * Add messages to the page.
 */
function template_preprocess_media_dialog_page(&$variables) {
  $variables['messages'] = theme('status_messages');
}

/* ******************************************** */
/* Content navigation controls                  */
/* ******************************************** */


/**
 * Theme function to display the results limiting- 10, 30, 50 results
 * per page.
 *
 * @param $variables
 *   array parameters
 * @return unknown
 */
function theme_media_browser_control_result_limit($variables) {
  // Pull out all the variables into a usable form
  extract($variables);

  if (!isset($limits)) {
    $limits = array(10, 30, 50);
  }

  // @NOTE these do not need to be aware of the current
  //       page because clicking them will reset the
  //       display to 1 -> $limit
  $parameters['page'] = 0;
  // save the active limit
  $current_limit = $parameters['limit'];
  $per_display = array();
  foreach ($limits as $limit) {
    if ($limit == $current_limit) {
      $class = 'active';
    }
    else {
      $class = '';
    }
    // set the value of this limit parameter to this limit value
    $parameters['limit'] = $limit;
    $per_display[] = l($limit, $limit, array('query' => $parameters, 'attributes' => array('class' => $class)));
  }

  return theme('item_list', array('items' => $per_display, 'attributes' => array('class' => 'result_limit')));
}


/**
 * Stolen from file.module theme_file_link
 *
 * @param $variables
 *   An associative array containing:
 *   - file: A file object to which the link will be created.
 */
function theme_media_link($variables) {
  $file = $variables['file'];

  $url = 'media/' . $file->fid;
  $icon = theme('file_icon', array('file' => $file));

  // Set options as per anchor format described at
  // http://microformats.org/wiki/file-format-examples
  $options = array(
    'attributes' => array(
      'type' => $file->filemime . '; length=' . $file->filesize,
    ),
  );

  // Use the description as the link text if available.
  if (empty($file->description)) {
    $link_text = check_plain($file->filename);
  }
  else {
    $link_text = check_plain($file->description);
    $options['attributes']['title'] = check_plain($file->filename);
  }

  return '<span class="file">' . $icon . ' ' . l($link_text, $url, $options) . '</span>';
}

/**
 * Adds a wrapper around a preview of a media file.
 * @param unknown_type $element
 * @return unknown_type
 */
function theme_media_thumbnail($variables) {
  $label = '';
  $element = $variables['element'];
  $destination = drupal_get_destination();

  // Wrappers to go around the thumbnail
  $prefix = '<div class="media-item"><div class="media-thumbnail">';
  $suffix = '</div></div>';

  // Arguments for the thumbnail link
  $thumb = $element['#children'];
  $target = 'media/' . $element['#file']->fid . '/edit';
  $options = array('query' => $destination, 'html' => TRUE, 'attributes' => array('title' => t('Click to edit details')));

  // Element should be a field renderable array... This is a little wonky - admitted.
  if (!empty($element['#show_names']) && $element['#name']) {
    $label = '<div class="label-wrapper"><label class="media-filename">' . $element['#name'] . '</label></div>';
  }

  // How can I attach CSS here?
  //$element['#attached']['css'][] = drupal_get_path('module', 'media') . '/css/media.css';
  drupal_add_css(drupal_get_path('module', 'media') . '/css/media.css');

  $output = $prefix;
  if (!empty($element['#add_link'])) {
    $output .= l($thumb, $target, $options);
  }
  else {
    $output .= $thumb;
  }
  $output .= $label . $suffix;
  return $output;
}

function template_preprocess_media_thumbnail(&$variables) {
  // Set the name for the thumbnail to be the filename.  This is done here so
  // that other modules can hijack the name displayed if it should not be the
  // filename.
  $variables['element']['#name'] = isset($variables['element']['#file']->filename) ? check_plain($variables['element']['#file']->filename) : NULL;
}

/**
 * Field formatter for displaying a file as a large icon.
 */
function theme_media_formatter_large_icon($variables) {
  $file = $variables['file'];
  $icon_dir = media_variable_get('icon_base_directory') . '/' . media_variable_get('icon_set');
  $icon = file_icon_path($file, $icon_dir);
  $variables['path'] = $icon;
  // theme_image() requires the 'alt' attribute passed as its own variable.
  // @see http://drupal.org/node/999338
  if (!isset($variables['alt']) && isset($variables['attributes']['alt'])) {
    $variables['alt'] = $variables['attributes']['alt'];
  }
  return theme('image', $variables);
}
