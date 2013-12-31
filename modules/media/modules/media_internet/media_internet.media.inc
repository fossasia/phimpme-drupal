<?php

/**
 * @file
 * Media module integration for the Media internet module.
 */

/**
 * Implements hook_media_browser_plugin_info().
 */
function media_internet_media_browser_plugin_info() {
  $plugins = array();

  if (user_access('administer media') || user_access('add media from remote sources')) {
    $plugins['media_internet'] = array(
      '#weight' => -10,
    );
  }

  return $plugins;
}

/**
 * Implements hook_media_browser_plugin_view().
 */
function media_internet_media_browser_plugin_view($plugin_name, $params) {
  $path = drupal_get_path('module', 'media');

  $types = isset($params['types']) ? $params['types'] : array();
  $multiselect = isset($params['multiselect']) ? $params['multiselect'] : FALSE;

  $redirect = array('media/browser', array('query' => array('render' => 'media-popup')));
  switch ($plugin_name) {
    case 'media_internet':
      // @todo: implement the multiselect argument here.
      $from_web_form = drupal_get_form('media_internet_add',  $types, $multiselect);
      return array(
        '#title' => t('Web'),
        'form' => array($from_web_form),
        '#attached' => array(
          //'js' => array($path . '/js/plugins/media.fromurl.js'),
        ),
      );
      break;
  }

  return array();
}

/**
 * Implements hook_media_internet_providers();
 *
 * Provides a very basic handler which copies files from remote sources to the
 * local files directory.
 */
function media_internet_media_internet_providers() {
  return array(
    'MediaInternetFileHandler' => array(
      'title' => 'Files',
      'hidden' => TRUE,
      // Make it go last.
      'weight' => 10000,
    ),
  );
}
