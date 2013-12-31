<?php

/**
 * @file
 * Media module integration for the Media module.
 */

/**
 * Implements hook_media_browser_plugin_info().
 */
function media_media_browser_plugin_info() {
  $plugins = array();

  // @TODO: Should we add a permission around this?
  $plugins['library'] = array(
    '#weight' => 10,
  );

  if (user_access('administer media') || user_access('edit media')) {
    $plugins['upload'] = array(
      '#weight' => -10,
    );
  }
  return $plugins;
}

/**
 * Implements hook_media_browser_plugin_view().
 */
function media_media_browser_plugin_view($plugin_name, $params) {
  $path = drupal_get_path('module', 'media');

  $params += array(
    'types' => array(),
    'multiselect' => FALSE,
  );

  // The multiselect parameter is a string.  So we check to see if it is set and
  // adjust the local variable accordingly.
  if ($params['multiselect'] != 'false' && $params['multiselect'] !== FALSE) {
    $params['multiselect'] = TRUE;
  }

  $redirect = array('media/browser', array('query' => array('render' => 'media-popup')));
  switch ($plugin_name) {
    case 'upload':
      $attached = array();
      if ($params['multiselect'] && module_exists('plupload')) {
        $upload_form_id = 'media_add_upload_multiple';
        $attached['js'] = array($path . '/js/plugins/media.upload_multiple.js');
      }
      else {
        $upload_form_id = 'media_add_upload';
      }

      module_load_include('inc', 'media', 'includes/media.pages');
      $upload_form = drupal_get_form($upload_form_id, $params);
      return array(
        '#title' => t('Upload'),
        'form' => array($upload_form),
        '#attached' => $attached,
      );
      break;
    case 'library':
      return array(
        '#title' => t('Library'),
        '#attached' => array(
          'js' => array(
            $path . '/js/plugins/media.library.js',
          ),
          'css' => array(
            //@todo: should move this.
            $path . '/js/plugins/media.library.css',
          ),
        ),
        '#settings' => array(
          'viewMode' => 'thumbnails',
          'getMediaUrl' => url('media/browser/list'),
        // We should probably change this to load dynamically when requested
        // via the JS file.
        ) + $params,
        '#markup' => '<div id="container"><div id="scrollbox"><ul id="media-browser-library-list" class="media-list-thumbnails"></ul><div id="status"></div></div></div>',
      );
    break;
  }

  return array();
}

/**
 * Implements hook_media_display_types().
 *
 * This is used to display media in different ways on the admin section.
 * Perhaps should be merged in with the browser display.
 */
function media_media_display_types() {
  $path = drupal_get_path('module', 'media');
  $display_types = array();

  $display_types['list'] = array(
    'title' => t('List'),
    'description' => t('Display as a list.'),
    'icon' => $path . '/images/display-list.png',
    'icon_active' => $path . '/images/display-list-active.png',
    'callback' => 'media_admin_list',
    'file' => drupal_get_path('module', 'media') . '/includes/media.admin.inc',
  );

  $display_types['thumbnails'] = array(
    'title' => t('Thumbnails'),
    'description' => t('Display as thumbnails.'),
    'icon' => $path . '/images/display-thumbnails.png',
    'icon_active' => $path . '/images/display-thumbnails-active.png',
    'callback' => 'media_admin_thumbnails',
    'file' => drupal_get_path('module', 'media') . '/includes/media.admin.inc',
  );

  return $display_types;
}

/**
 * Implements hook_media_operations().
 */
function media_media_operations() {
  $operations = array(
    'delete' => array(
      'label' => t('Delete'),
      'callback' => NULL,
    ),
    'edit' => array(
      'label' => t('Edit'),
      'callback' => NULL,
      'redirect' => 'media/%fids/multiedit'
    ),
  );

  if (!module_exists('multiform')) {
    // If the multiform module is not installed, do not show this option.
    unset($operations['edit']);
  }
  return $operations;
}
