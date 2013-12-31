<?php

/**
 * Page callback to list all galleries.
 *
 * TODO: When I grow up I want to be a View.
 */
function media_gallery_list_galleries($term) {
  $collections_vid = variable_get('media_gallery_collection_vid', 0);
  if ($term->vid !== $collections_vid) {
    module_load_include('inc', 'taxonomy', 'taxonomy.pages');
    return taxonomy_term_page($term);
  }
  
  // Add front-end resources for drag-and-drop sorting.
  if (user_access('administer media galleries')) {
    drupal_add_library('system', 'ui.sortable');
    drupal_add_js(drupal_get_path('module', 'media_gallery') . '/media_gallery.dragdrop.js');
    drupal_add_css(drupal_get_path('module', 'media_gallery') . '/media_gallery.dragdrop.css');
    $sort_collection_url = url('media-gallery/sort/collection/' . $term->tid . '/' . drupal_get_token('media_gallery'));
    drupal_add_js(array('mediaGallerySortCollectionUrl' => $sort_collection_url), array('type' => 'setting'));
  }
  // Build breadcrumb based on the hierarchy of the term.
  $current = (object) array(
    'tid' => $term->tid,
  );
  $breadcrumb = array();
  while ($parents = taxonomy_get_parents($current->tid)) {
    $current = array_shift($parents);
    $breadcrumb[] = l($current->name, 'taxonomy/term/' . $current->tid);
  }
  $breadcrumb[] = l(t('Home'), NULL);
  $breadcrumb = array_reverse($breadcrumb);
  drupal_set_breadcrumb($breadcrumb);
  drupal_add_feed('taxonomy/term/' . $term->tid . '/feed', 'RSS - ' . $term->name);

  $build['term_heading'] = array(
    '#prefix' => '<div class="term-listing-heading">',
    '#suffix' => '</div>',
    'term' => taxonomy_term_view($term, 'full'),
  );

  // There is a small bug here with the "entity"
  $term_entity = new FieldsRSIPreventor($term);
  $limit = $term_entity->getValue('media_gallery_columns') * $term_entity->getValue('media_gallery_rows');
  if (!$limit) {
    $limit = 10;
  }

  $nids = media_gallery_select_galleries($term->tid, TRUE, $limit);

  if (!empty($nids)) {
    $nodes = node_load_multiple($nids);
    // Add the nodes to the build array, with a weight of 5.
    $build += node_view_multiple($nodes, 'teaser', 5);
    $build['pager'] = array(
      '#theme' => 'pager',
      '#weight' => 10,
     );
    $build['#term'] = $term;
    $build['#theme'] = 'media_gallery_collection';
  }
  else {
    $build['no_content'] = array(
      '#prefix' => '<p>',
      '#markup' => t('No galleries have been set up yet. <a href="@link">Add a gallery.</a>', array('@link' => url('node/add/media-gallery'))),
      '#suffix' => '</p>',
    );
  }
  return $build;
}

/**
 * Menu callback; view a single gallery media entity as its own page.
 */
function media_gallery_detail_page($gallery_node, $file) {
  // Set the breadcrumb.
  $node_url_arguments = entity_uri('node', $gallery_node);
  drupal_set_breadcrumb(array(
    l(t('Home'), NULL),
    l($gallery_node->title, $node_url_arguments['path'], $node_url_arguments['options']),
  ));

  // Set the title for the page
  $title = _media_gallery_get_media_title($file);
  drupal_set_title($title);

  drupal_add_js(drupal_get_path('module', 'media_gallery') . '/media_gallery.js');

  $return = media_gallery_item_view($gallery_node, $file, 'media_gallery_detail');
  return $return;
}

/**
 * Menu callback; view a single gallery media item as the content of a lightbox.
 */
function media_gallery_lightbox_page($gallery_node, $file) {
  return media_gallery_item_view($gallery_node, $file, 'media_gallery_lightbox');
}

/**
 * Menu page delivery callback; print content for a lightbox.
 *
 * This overrides drupal_deliver_html_page() for pages displayed within the
 * lightbox, in order to skip printing any HTML except the main page content.
 */
function media_gallery_lightbox_page_deliver($page_content) {
  // Display an error message if something went wrong.
  if (!isset($page_content) || is_int($page_content)) {
    $content = array('#markup' => t('An unexpected error occurred. Please try again later.'));
  }
  // This shouldn't happen, but just in case.
  elseif (is_string($page_content)) {
    $content = array('#markup' => $page_content);
  }
  // This is the part of the code we expect to reach.
  else {
    $content = $page_content;
  }

  // Render the main page content, and nothing else. We don't want to call
  // drupal_render_page() because the results of this function are inserted
  // into a <div> on the parent page, so we can't print a full HTML document.
  // If we inserted them into an iframe within the lightbox, things would be
  // different, but we don't want to do that because displaying the page in an
  // iframe results in reduced functionality (you don't get the colorbox
  // plugin's autoresizing behavior).
  print drupal_render($content);
  drupal_exit();
}

/**
 * AJAX callback for drag-and-drop gallery sorting.
 *
 * @param string $type
 *   The type of item being sorted: 'gallery' for an individual gallery or
 *   'collection' for a gallery collection.
 * @param mixed $item
 *   For a media gallery, the $node object for that gallery; for gallery
 *   collections, the taxonomy term corresponding to the collection.
 */
function media_gallery_ajax_sort($type, $item) {
  $order = $_POST['order'];
  $result = FALSE;
  $id_prefix = '';
  switch ($type) {
    case 'collection':
      $id_prefix = 'node-';
      $order = _media_gallery_sanitize_ids($id_prefix, $order);
      $result = media_gallery_reorder_collection($item, $order);
      break;
    case 'gallery':
      $id_prefix = 'media-gallery-media-';
      $order = _media_gallery_sanitize_ids($id_prefix, $order);
      $result = media_gallery_reorder_gallery($item, $order);
      break;
  }
  drupal_json_output(array('result' => $result, 'order' => $_POST['order'], 'idPrefix' => $id_prefix));
}

/**
 * Helper function for media_gallery_ajax_sort() that sanitizes a list of IDs.
 *
 * Removes a string such as 'node-' from each item in an array, leaving only a
 * numeric ID.
 *
 * @param string $prefix
 *   The prefix to remove from each ID in the list.
 * @param array $list
 *   The list of IDs.
 *
 * @return array
 *   The list of sanitized IDs.
 */
function _media_gallery_sanitize_ids($prefix, $list) {
  foreach ($list as &$value) {
    $value = intval(str_replace($prefix, '', $value));
  }
  return $list;
}

/**
 * Reorder a gallery collection.
 */
function media_gallery_reorder_collection($collection, $order) {
  // Get a complete ordered list of the galleries in this collection.
  $galleries = db_query('SELECT ti.nid FROM {taxonomy_index} ti LEFT JOIN {media_gallery_weight} mgw on ti.nid = mgw.nid WHERE ti.tid = :tid ORDER BY mgw.weight ASC, ti.nid ASC', array(':tid' => $collection->tid))->fetchCol();
  // Sort the list.
  $galleries = _media_gallery_reorder($galleries, $order);
  // Resave gallery weights for the entire collection.
  db_delete('media_gallery_weight')
    ->condition('tid', $collection->tid)
    ->execute();
  $query = db_insert('media_gallery_weight')
    ->fields(array('tid', 'nid', 'weight'));
  foreach ($galleries as $weight => $nid) {
    $query->values(array(
      'tid' => $collection->tid,
      'nid' => $nid,
      'weight' => $weight,
    ));
  }
  $query->execute();
  return TRUE;
}

/**
 * Reorder the media items in a gallery.
 */
function media_gallery_reorder_gallery($gallery, $order) {
  $media_field = $gallery->media_gallery_media[LANGUAGE_NONE];

  if (!empty($_POST['page'])) {
    $page = array_pop(explode(',', $_POST['page']));
    $num_per_page = $gallery->media_gallery_columns[LANGUAGE_NONE][0]['value'] * $gallery->media_gallery_rows[LANGUAGE_NONE][0]['value'];
    $offset = $num_per_page * $page;
  }
  
  foreach ($order as $new_delta => $old_delta) {
    $gallery->media_gallery_media[LANGUAGE_NONE][$new_delta + $offset] = $media_field[$old_delta + $offset];
  }
  node_save($gallery);
  return TRUE;
}

/**
 * Helper function to reorder a list when given a reordered subset of the list.
 *
 * @param array $list
 *   The list in its original order, as a numeric array.
 * @param array $ordered_subset
 *   The part of the list that is to be reordered, as a numeric array.
 * @return array
 *   The list in its new order.
 */
function _media_gallery_reorder($list, $ordered_subset) {
  $list_by_id = array_flip($list);
  // Determine the current position of each item in the subset that we're
  // reordering.
  $subset = array();
  foreach ($ordered_subset as $position => $id) {
    $subset[$list_by_id[$id]] = $id;
  }
  ksort($subset);
  // Place each item of the reordered subset into the list in its new position.
  foreach ($subset as $original_position => $id) {
    $list[$original_position] = current($ordered_subset);
    next($ordered_subset);
  }
  return $list;
}

/**
 * Page callback to add images to a media gallery.
 *
 * @param $node
 *   The node to which the images should be added.
 */
function media_gallery_add_images($node) {
  // The caller provides a numeric array of file ids that the user selected.
  $fids = $_POST['files'];

  // Determine which file ids are already included in this gallery.
  $items = $node->media_gallery_media[LANGUAGE_NONE];
  foreach ($items as $item) {
    $existing_fids[$item['fid']] = TRUE;
  }

  // Add the newly selected media items to the previous list of items.
  foreach ($fids as $fid) {
    // Only add a selected item if it isn't already in the gallery.
    if (empty($existing_fids[$fid])) {
      $file = Array();
      $file['fid'] = $fid;
      $file['title'] = null;
      $file['data'] = '';
      $items[] = $file;
    }
  }

  $node->media_gallery_media[LANGUAGE_NONE] = $items;
  node_save($node);

  drupal_json_output(array('result' => TRUE, 'items' => $items));
}

/**
 * Form callback: Display a form for removing a media item from a gallery.
 *
 * @param $node
 *   The gallery node object.
 * @param $file
 *   The file to remove from the gallery.
 */
function media_gallery_remove_item_form($form, &$form_state, $node, $file) {
  // Set the breadcrumb.
  $node_url_arguments = entity_uri('node', $node);
  drupal_set_breadcrumb(array(
    l(t('Home'), NULL),
    l($node->title, $node_url_arguments['path'], $node_url_arguments['options']),
  ));

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  $form['fid'] = array(
    '#type' => 'value',
    '#value' => $file->fid,
  );
  return confirm_form($form,
    t('Are you sure you want to remove %file from the gallery %gallery?', array('%file' => $file->filename, '%gallery' => $node->title)),
    "media-gallery/detail/{$node->nid}/{$file->fid}",
    t('The file will still be available in the media library to use elsewhere on this site.'),
    t('Remove file')
  );
}

/**
 * Submit handler for removing a media item from a gallery.
 */
function media_gallery_remove_item_form_submit($form, &$form_state) {
  $node = node_load($form_state['values']['nid']);
  $file = file_load($form_state['values']['fid']);
  media_gallery_remove_item_from_gallery($node, $file);
  drupal_set_message(t('The file %file was removed from the gallery.', array('%file' => $file->filename)));
  $form_state['redirect'] = "node/{$node->nid}";
}

/**
 * Menu callback; presents the Media editing form in the context of a gallery.
 */
function media_gallery_media_page_edit($gallery, $file) {
  // The Media Gallery module defines titles differently than just using the
  // filename.
  drupal_set_title(t('<em>Edit @type</em> @title', array('@type' => $file->type, '@title' => _media_gallery_get_media_title($file))), PASS_THROUGH);

  // Set the breadcrumb.
  $node_url_arguments = entity_uri('node', $gallery);
  drupal_set_breadcrumb(array(
    l(t('Home'), NULL),
    l($gallery->title, $node_url_arguments['path'], $node_url_arguments['options']),
  ));

  // We'll be reusing Media module's 'media_edit' form, so we need to load its
  // include file. In addition to loading it here, we also set the include file
  // info within $form_state in case there are AJAX enabled widgets that will
  // submit to system/ajax, bypassing this menu callback.
  module_load_include('inc', 'media', 'includes/media.pages');
  $form_state['build_info']['files']['menu'] = array('type' => 'inc', 'module' => 'media', 'name' => 'media.pages');

  // Provide context information for form building and processing functions.
  $form_state['media_gallery']['gallery'] = $gallery;

  // Since we have a $form_state, we can't call drupal_get_form(), so pass the
  // expected arguments to drupal_build_form().
  $form_state['build_info']['args'] = array($file);
  return drupal_build_form('media_edit', $form_state);
}

/**
 * Menu callback; presents the Media multiedit form in the context of a gallery.
 */
function media_gallery_media_page_multiedit($node) {
  // Determine the media entities to edit.
  $columns = $node->media_gallery_columns[LANGUAGE_NONE][0]['value'];
  $rows = $node->media_gallery_rows[LANGUAGE_NONE][0]['value'];
  $number_per_page = $columns * $rows;
  $current_page = isset($_GET['page']) ? $_GET['page'] : 0;
  $items = array_slice($node->media_gallery_media[LANGUAGE_NONE], $current_page*$number_per_page, $number_per_page);
  $fids = array();
  foreach ($items as $item) {
    $fids[] = $item['fid'];
  }
  $files = file_load_multiple($fids);

  // Load the Media module include file containing the form callback.
  // @todo To support AJAX enabled widgets within the form, we need to also add
  //   media.pages.inc to $form_state['build_info']['files']['menu']. Figure out
  //   how to integrate that properly with the Multiform module.
  module_load_include('inc', 'media', 'includes/media.pages');

  // Build and process the form.
  $form = media_page_multiedit($files);

  // Override the page title set by media_page_multiedit() and return the form.
  drupal_set_title(t('<em>Edit media for</em> @title', array('@type' => $node->type, '@title' => $node->title)), PASS_THROUGH);
  return $form;
}
