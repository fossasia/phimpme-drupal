<?php

/**
 * @file
 * Media Gallery Theming
 */

/**
 * Displays a collection of galleries as a grid of teasers.
 */
function theme_media_gallery_collection($variables) {
  $element = $variables['element'];
  $collection = $element['#term'];

  $columns = $collection->media_gallery_columns[LANGUAGE_NONE][0]['value'];
  $grid = '<div class="media-gallery-collection mg-collection-' . $collection->vocabulary_machine_name . ' mg-col mg-col-' . $columns . '">';
  foreach (element_children($element['nodes']) as $nid) {
    // This invokes node.tpl.php, and where that calls render($content),
    // theme_media_gallery_teaser() is called.

    // Add the term to the node so that we can use it to configure the meta data
    $element['nodes'][$nid]['#node']->term = $collection;
    $teaser = drupal_render($element['nodes'][$nid]);

    // @todo Implement real display needs.
    $cell = $teaser;
    $grid .= $cell;
  }
  $grid .= '</div>';

  // Replace the 'nodes' element with the rendered grid while preserving its
  // weight, so that other fields that are part of the collection term get
  // rendered normally, and in the correct order.
  $weight = isset($element['nodes']['#weight']) ? $element['nodes']['#weight'] : 0;
  $element['nodes'] = array('#markup' => $grid, '#weight' => $weight);
  $output = drupal_render_children($element);

  return $output;
}

/**
 * Displays a gallery node as a teaser.
 *
 * The Galleries page displays a grid of gallery teasers. Each gallery teaser is
 * themed as a node using node.tpl.php or one if its overrides. Where that
 * template file calls @code render($content) @endcode, the output of this
 * function is returned.
 */
function theme_media_gallery_teaser($variables) {
  $element = $variables['element'];
  $node = $element['#node'];

  if (isset($element['media_gallery_media'][0])) {
    $element['media_gallery_media'][0]['#theme'] = 'media_gallery_file_field_inline';
    $image = drupal_render($element['media_gallery_media'][0]);
  }
  else {
    $image = theme('image', array('path' => drupal_get_path('module', 'media_gallery') . '/images/empty_gallery.png'));
  }

  $link_vars = array();
  $link_vars['image'] = $image;
  $uri = entity_uri('node', $node);
  $link_vars['link_path'] = $uri['path'];
  $link_vars['classes'] = array('media-gallery-thumb');

  $output = '<div class="media-collection-item-wrapper"><img class="stack-image" src="' . base_path() . drupal_get_path('module', 'media_gallery') . '/images/stack_bg.png" />' . theme('media_gallery_item', $link_vars) . '</div>';

  // Set the variables to theme the meta data if there is a term on the node
  if (isset($node->term)) {
    $term = $node->term;
    $meta_vars = array();
    $meta_vars['location'] = $term->media_gallery_image_info_where[LANGUAGE_NONE][0]['value'];
    $meta_vars['title'] = $node->title;
    $meta_vars['link_path'] = $link_vars['link_path'];
    // Organize the file count by type. We only expect images and videos for
    // now, so we put those first and group the others together into a general
    // category at the end.
    $type_count = media_gallery_get_media_type_count($node, 'media_gallery_media_original');
    $description = array();
    if (isset($type_count['image'])) {
      $count = $type_count['image'];
      $description[] = format_plural($count, '<span>@num image</span>', '<span>@num images</span>', array('@num' => $count));
      unset($type_count['image']);
    }
    if (isset($type_count['video'])) {
      $count = $type_count['video'];
      $description[] = format_plural($count, '<span>@num video</span>', '<span>@num videos</span>', array('@num' => $count));
      unset($type_count['video']);
    }
    if (!empty($type_count)) {
      $count = array_sum($type_count);
      $description[] = format_plural($count, '<span>@num other item</span>', '<span>@num other items</span>', array('@num' => $count));
    }
    $meta_vars['description'] = implode(', ', $description);

    // Add the meta information
    $output .= theme('media_gallery_meta', $meta_vars);
  }

  return $output;
}

/**
 * Template preprocess function for displaying a media item (entity) as a
 * thumbnail on the gallery page.
 */
function template_preprocess_media_gallery_media_item_thumbnail(&$variables) {
  $element = $variables['element'];

  // Attach the colorbox javascript if the format calls for it.
  $format = $element['#media_gallery_entity']->media_gallery_format[LANGUAGE_NONE][0]['value'];
  $lightbox = is_numeric(strpos($format, 'lightbox')) ? TRUE : FALSE;
  if ($lightbox) {
    $element['file']['#attached']['js'][] = drupal_get_path('module', 'media_gallery') . '/colorbox-display.js';
    $element['file']['#attached']['library'][] = array('media_gallery', 'colorbox');
  }

  // Get the rendered file without annoying DIV wrappers.
  $element['file'] = array('#theme' => 'media_gallery_file_field_inline', '0' => $element['file']);
  $image = drupal_render($element['file']);

  $gallery_id = $element['#media_gallery_entity']->nid;
  $media_id = $element['#file']->fid;

  // Add a class that is a more targeted version of what template_preprocess()
  // automatically adds for this theme hook, to enable per-type (e.g., video vs.
  // image) styling.
  $variables['classes_array'][] = drupal_html_class('media_gallery_media_item_thumbnail_' . $element['#file']->type);

  // Add a class for the wrapper.
  $variables['classes_array'][] = 'media-gallery-item-wrapper';

  // Create an array of variables to be added to the main thumbnail link.
  $link_vars = array();
  $link_vars['image'] = $image;
  $link_vars['link_path'] = "media-gallery/detail/$gallery_id/$media_id";
  $link_vars['classes'] = $lightbox ? array('media-gallery-thumb', 'cbEnabled') : array('media-gallery-thumb');
  $link_vars['title'] = $element['#bundle'] == 'image' ? t('View larger image') : t('Watch video');
  // Add the image as a link to the detailed view
  $variables['media_gallery_item'] = theme('media_gallery_item', $link_vars);

  // Set the variables to theme the metadata.
  $meta_vars = array();
  $meta_vars['location'] = $element['#media_gallery_entity']->media_gallery_image_info_where[LANGUAGE_NONE][0]['value'];
  $meta_vars['title'] = isset($element['media_title']) ? $element['media_title'][0]['#markup'] : '';
  $meta_vars['link_path'] = $link_vars['link_path'];

  // Theme the metadata.
  $variables['media_gallery_meta'] = theme('media_gallery_meta', $meta_vars);
}

/**
 * Displays a media item (entity) within a lightbox.
 *
 * Clicking a thumbnail within the gallery page opens a lightbox if all these
 * conditions are met:
 * - The gallery node's media_gallery_format field indicates to open a lightbox.
 * - The colorbox jQuery plugin is available.
 * - The user has JavaScript enabled.
 *
 * The lightbox contains some navigation functionality (including a "slideshow"
 * link) and a <div> containing the actual content. This function themes the
 * contents of that <div>.
 *
 * When any of the conditions for opening a lightbox aren't met, the user is
 * taken to a separate detail page instead, the contents of which are themed by
 * theme_media_gallery_media_item_detail().
 */
function theme_media_gallery_media_item_lightbox($variables) {
  $element = $variables['element'];
  $gallery_node = new FieldsRSIPreventor($element['#media_gallery_entity']);
  $file = $element['#file'];

  // The lightbox JavaScript requires width and height attributes to be set on
  // the displayed image, but if we're displaying an image derivative, we need
  // to create it in order to know its width and height.
  // @todo Improve the JavaScript to not require this.
  if ($element['file']['#theme'] == 'image_style') {
    $style_name = $element['file']['#style_name'];
    $style_path = image_style_path($style_name, $file->uri);
    if (!file_exists($style_path)) {
      $style = image_style_load($style_name);
      image_style_create_derivative($style, $file->uri, $style_path);
    }
    $info = image_get_info($style_path);
    $element['file'] += array('#attributes' => array());
    $element['file']['#attributes'] += array('width' => $info['width'], 'height' => $info['height']);
  }

  $image = drupal_render($element['file']);

  $matches = NULL;
  if (preg_match('@<img .*?/>@', $image, $matches)) {
    $image = $matches[0];
  }
  else {

  }

  $gallery_id = $element['#media_gallery_entity']->nid;
  $media_id = $element['#file']->fid;

  // Create an array of variables to be added to the main image link.
  $link_vars = array();
  $link_vars['image'] = $image;
  $link_vars['link_path'] = "media-gallery/detail/$gallery_id/$media_id";
  $link_vars['no_link'] = $element['#bundle'] != 'image' ? TRUE : FALSE;

  if ($gallery_node->getValue('media_gallery_allow_download') == TRUE) {
    $download_link = $element['#bundle'] == 'image' ? theme('media_gallery_download_link', array('file' => $file)) : l(t('View detail page'), $link_vars['link_path']);
  }

  else {
    // Very ugly fix: This prevents the license info from being either hidden
    // or causing scrollbars (depending on the browser) in cases where a
    // download link is not being shown. There may be a CSS-only fix for this,
    // but we haven't found one yet.
    $download_link = '&nbsp;';
  }

  $media_gallery_detail =
      '<div class="lightbox-stack">' .
      theme('media_gallery_item', $link_vars) .
      '<div class="media-gallery-detail-info">' .
      $download_link .
      theme('media_gallery_license', array('element' => isset($element['field_license']) ? $element['field_license'] : array(), 'color' => 'medium', 'file' => $file)) .
      '</div></div>';
  // The license info has been themed already, keep it from being rendered as a child
  $element['field_license']['#access'] = FALSE;

  $output = 'Error';
  // If the format is to have the description as well, we add it here
  if (!empty($gallery_node->media_gallery_lightbox_extras[LANGUAGE_NONE][0]['value'])) {
    $output =
    '<div class="mg-lightbox-wrapper clearfix">' .
      '<div class="lightbox-title">' . drupal_render($element['media_title']) . '</div>' .
      '<div class="mg-lightbox-detail">' .
      $media_gallery_detail .
      '</div><div class="mg-lightbox-description">' .
        drupal_render_children($element) .
      '</div>' .
    '</div>';
  } else {
    $output = $media_gallery_detail;
  }

  return $output;
}

/**
 * Displays a media item (entity) as its own page, within gallery context.
 *
 * @see theme_media_gallery_media_item_lightbox()
 */
function theme_media_gallery_media_item_detail($variables) {
  $element = $variables['element'];
  $gallery_node = new FieldsRSIPreventor($element['#media_gallery_entity']);
  $file = $element['#file'];
  // Page number for next and previous pages and current page.
  $i_next = NULL;
  $i_previous = NULL;
  $i_current = NULL;

  // Not considering the possibility of this field being translatable, at the
  // moment. Is there a use-case for a media field (which is just a reference to
  // a media entity) to be translatable?
  $media_ids = array();
  foreach ($gallery_node->media_gallery_media[LANGUAGE_NONE] as $delta => $item) {
    $media_ids[] = _media_gallery_get_media_fid($item);
  }
  $media_ids = array_values(array_unique($media_ids));

  // Get the variables needed for previous and next buttons and "Image X of Y"
  // text.
  $num_items = count($media_ids);
  foreach ($media_ids as $i => $id) {
    if ($id == $file->fid) {
      $i_current = $i;
      break;
    }
  }

  $i_previous = $i_current - 1;
  $i_next = $i_current + 1;

  if ($i_previous < 0) {
    $i_previous = NULL;
  }

  $i_next = $i_current + 1;
  if ($i_next > $num_items - 1) {
    $i_next = NULL;
  }

  if ($gallery_node->getValue('media_gallery_allow_download') == TRUE) {
    $download_link = $element['#bundle'] == 'image' ? theme('media_gallery_download_link', array('file' => $file)) : '&nbsp;';
  }
  else {
    // Very ugly fix: This prevents the license info from being either hidden
    // or causing scrollbars (depending on the browser) in cases where a
    // download link is not being shown. There may be a CSS-only fix for this,
    // but we haven't found one yet.
    $download_link = '&nbsp;';
  }

  $previous_link = !is_null($i_previous) ? l(t('« Previous'), "media-gallery/detail/{$gallery_node->nid}/{$media_ids[$i_previous]}", array('html' => TRUE, 'attributes' => array('class' => 'prev'))) : '';
  $next_link = !is_null($i_next) ? l(t('Next »'), "media-gallery/detail/{$gallery_node->nid}/{$media_ids[$i_next]}", array('html' => TRUE, 'attributes' => array('class' => 'next'))) : '';

  // Render the file out in a wrapper
  $output =
    '<div class="media-gallery-detail-wrapper">' .
    '<div class="media-gallery-detail">' .
      drupal_render($element['file']) .
      '<div class="media-gallery-detail-info">' . $download_link .
        theme('media_gallery_license', array('element' => isset($element['field_license']) ? $element['field_license'] : array('#view_mode' => 'media_gallery_detail'), 'color' => 'dark', 'file' => $file)) .
      '</div>' .
      '<div class="media-gallery-detail-info">' .
        '<span class="media-gallery-back-link">' .
          l(t('« Back to gallery'), 'node/' . $gallery_node->nid) .
        '</span>' .
        '<span class="media-gallery-detail-image-info-wrapper">' .
          '<span class="media-gallery-image-count">' .
            t("Item @current of @total", array('@current' => $i_current + 1, '@total' => $num_items)) .
          '</span>' .
          '<span class="media-gallery-controls">' .
            $previous_link .
            (!empty($previous_link) && !empty($next_link) ? ' | ' : '') .
            $next_link .
          '</span>' .
        '</span>' .
      '</div>' .
    '</div>';

  // The license was already output above via a direct theme() call rather
  // than drupal_render().
  $element['field_license']['#printed'] = TRUE;

  // The file was rendered above, but due to a drupal_render() bug, might not be
  // marked as printed.
  // @todo Remove when http://drupal.org/node/1305220 is fixed.
  $element['file']['#printed'] = TRUE;

  // Render the remaining fields, but not the title, since that's output as
  // part of the page title.
  $element['media_title']['#access'] = FALSE;
  $output .=
    '<div class="no-overflow">' .
      drupal_render_children($element) .
    '</div></div>';

  return $output;
}

/**
 * Displays a media item (entity) as a thumbnail in a block
 */
function theme_media_gallery_block_thumbnail($variables) {
  $element = $variables['element'];

  // Attach the colorbox javascript if the format calls for it.
  $format = $element['#media_gallery_entity']->media_gallery_format[LANGUAGE_NONE][0]['value'];
  $lightbox = is_numeric(strpos($format, 'lightbox')) ? TRUE : FALSE;
  if ($lightbox) {
    $element['file']['#attached']['js'][] = drupal_get_path('module', 'media_gallery') . '/colorbox-display.js';
    $element['file']['#attached']['library'][] = array('media_gallery', 'colorbox');
  }

  // Get the rendered file without annoying DIV wrappers.
  $element['file'] = array('#theme' => 'media_gallery_file_field_inline', '0' => $element['file']);
  $image = drupal_render($element['file']);

  // Create some variables to be added to the main image link
  $classes = $lightbox ? array('media-gallery-thumb', 'cbEnabled') : array('media-gallery-thumb');
  $gallery_id = $element['#media_gallery_entity']->nid;
  $media_id = $element['#file']->fid;
  $link_path = "media-gallery/detail/$gallery_id/$media_id";

  // Create a wrapper
  $output = '<div class="media-gallery-item-wrapper">';
  // Style the thumbnail as a link
  $output .= theme('media_gallery_item', array('image' => $image, 'link_path' => $link_path, 'classes' => $classes));
  // Close the wrapper
  $output .= '</div>';

  return $output;
}

/**
 * Displays a media item (entity) as a thumbnail on a Gallery collection page
 */
function theme_media_gallery_collection_thumbnail($variables) {
  // In the default configuration of the module, the collection thumbnail
  // display format is used for node teasers, and theme_media_gallery_teaser()
  // bypasses calling this theme function. However, if someone configures their
  // site differently (to use this display format in a different view mode), we
  // need some sensible behavior here. For now, we just theme the media item
  // the same way we theme it when it appears in a block thumbnail.
  // @todo: Ideally, some of the code in theme_media_gallery_teaser() would
  //   move here instead, so that people could configure their site to
  //   reproduce the default "media gallery node teaser" style in other view
  //   modes as well.
  return theme('media_gallery_block_thumbnail', $variables);
}

/**
 * Theme the thumbnail link for a media gallery item
 *
 * @param string $image
 *   Which meta data fields to display
 * @param string $link_path
 *   The location to place the meta data on the media item
 * @param string $classes
 *   An array of classes to attach to the link.
 *
 */
function theme_media_gallery_item($variables) {
  $image = $variables['image'];
  $link_path = $variables['link_path'];
  $attributes = array();
  if (!empty($variables['classes'])) {
    $attributes['class'] = $variables['classes'];
  }
  if (!empty($variables['title'])) {
    // I'm fairly sure I don't like this solution.  But as Alex mentions in
    // theme_media_gallery_file_field_inline() the File Styles module isn't allowing
    // us access to the render array pre-rendering, so I'm doing a str_replace()
    // here specifically to address the title and alt for thumbnails.  This had
    // to be further modified to remove and then add the title and alt attributes
    // video thumbnails had no title and alt attributes so the string replace was
    // not triggering for them.
    $new_image = str_replace(array('title=""', 'alt=""'), array('', ''), $image);
    $image = str_replace('/>', ' title="'.$variables['title'].'" alt="'.$variables['title'].'" />', $new_image);;
  }

  // Add sliding door top div and wrappers
  $item = '<div class="media-gallery-item"><div class="top"><div class="top-inset-1"><div class="top-inset-2"></div></div></div><div class="gallery-thumb-outer"><div class="gallery-thumb-inner">';
  // Create a link around the image
  $item .= empty($variables['no_link']) ? l($image, $link_path, array('html' => TRUE, 'attributes' => $attributes)) : $image;
  // Add sliding door bottom div and close wrappers
  $item .= '</div></div><div class="bottom"><div class="bottom-inset-1"><div class="bottom-inset-2"></div></div></div></div>';
  return $item;
}

/**
 * Theme the meta data for a media gallery item
 *
 * @param string $display
 *   Which meta data fields to display
 * @param string $location
 *   The location to place the meta data on the media item
 * @param string $title
 *   The title to display (if applicable)
 * @param string $license
 *   The license information for the media item
 * @param string $description
 *   A description to display with the title (if applicable).
 */
function theme_media_gallery_meta($variables) {
  $location = $variables['location'];
  $title = $variables['title'];
  $description = $variables['description'];
  $link_path = $variables['link_path'];
  // Add a top sliding door to the meta info
  $meta = '<span class="top slider"><span class="top-inner slider"></span></span>';

  // Open the content sliding door for the meta info
  $meta .= '<span class="meta-outer slider"><span class="meta-inner slider">';

  // If we display nothing, nothing else matters
  if ($location != 'nothing') {
    // Add a wrapper around the meta data
    #$meta .= '<div class="meta-wrapper ' . $location . '">';
    // Add title
    $attributes = array('class' => array('meta-wrapper', 'cbEnabled', $location));
    $meta .= $title ? '<span class="media-title">' . $title . '</span>' : '';
    if ($description) {
      $meta .= '<span class="media-description">' . $description . '</span>';
    }
    // Close the content sliding door
    $meta .= '</span></span>';

    // Add a bottom sliding door
    $meta .= '<span class="bottom slider"><span class="bottom-inner slider"></span></span>';

    // Close the wrapper around the meta data
    $meta_link = $location == 'hover' ? l($meta, $link_path, array('attributes' => $attributes, 'html' => TRUE)) : '<span class="meta-wrapper">' . $meta . '</span>';
  }

  return isset($meta_link) ? $meta_link : NULL;
}

/**
 * Returns HTML for a link to a file.
 *
 * @param $variables
 *   An associative array containing:
 *   - file: A file object to which the link will be created.
 *   - text: (optional) The link text. Defaults to t('Download original image').
 *   - options: (optional) Extra options to pass to l().
 *
 * @see theme_file_link()
 *
 * @ingroup themeable
 *
 * @todo Move to Media module?
 */
function theme_media_gallery_download_link($variables) {
  $file = $variables['file'];
  $text = $variables['text'];
  $options = $variables['options'];

  // Link to "media/FID/download", so that Drupal can set the
  // Content-Disposition header as needed to instruct the browser to download
  // rather than display the file. Append the file name at the end to work
  // around Internet Explorer's deficiencies in understanding HTTP headers.
  $path = 'media/' . $file->fid . '/download/' . $file->filename;

  if (!isset($text)) {
    $text = t('Download original image');
  }

  // Set options as per anchor format described at
  // http://microformats.org/wiki/file-format-examples
  $options['attributes']['type'] = $file->filemime . '; length=' . $file->filesize;
  $options['attributes']['class'] = 'gallery-download';

  return l($text, $path, $options);
}

/*
 * Turns the license information into the Creative Commons images.
 */
function theme_media_gallery_license($variables) {
  // Don't display license information for externally hosted media. See
  //   media_gallery_field_attach_form().
  if (isset($variables['file'])) {
    // @todo Implement a more generic determination for when a license applies
    //   and when it doesn't.
    if (file_uri_scheme($variables['file']->uri) == 'youtube') {
      return '';
    }
  }

  if (isset($variables['element']['#items'][0]['value'])) {
    $item = $variables['element']['#items'][0]['value'];
  }
  else {
    $item = 'none';
  }
  $color = $variables['color'];
  $options = explode('_', $item);

  // Open a wrapper around the icons.
  $output = '<span class="media-license ' . $color . '">';

  if (empty($item) || $item === 'none') {
    $output .= '<span class="copyright" title="All rights reserved"></span>';
  }

  $output .= in_array('cc', $options) ? '<span class="attribution" title="Attribution"></span>' : '';
  $output .= in_array('nc', $options) ? '<span class="non-commercial" title="Non-Commercial"></span>' : '';
  $output .= in_array('sa', $options) ? '<span class="share-alike" title="Share Alike"></span>' : '';
  $output .= in_array('nd', $options) ? '<span class="no-deriv" title="No Derivative Works"></span>' : '';

  $output .= '</span>';

  return $output;
}

/**
 * Renders a file field for use where block-level HTML tags are not wanted, such as in a link.
 *
 * This theme function is called instead of theme_field(), and therefore, we
 * bypass all the container DIV tags normally added for fields.
 */
function theme_media_gallery_file_field_inline($variables) {
  $element = $variables['element'];

  $output = drupal_render_children($element);

  // @todo Most likely, the image was rendered using a Styles module formatter
  //   (probably a File Styles wrapper to an image style). The Styles module
  //   needs to be improved to integrate properly into Drupal 7 rendering, so
  //   that we can adjust the render array before rendering it. But until the
  //   Styles module is sufficiently improved, we're stuck with getting back a
  //   rendered string containing a DIV wrapper around the image. So here, we
  //   remove all DIV tags, which is totally hacky, and exactly the kind of
  //   stuff that the D7 render system is meant to avoid.
  $output = trim(preg_replace('@</?div.*?/?\>@', '', $output));

  return $output;
}
