<?php

/**
 * @file
 *  Provide the Galleriffic plugin definition.
 */

function views_galleriffic_views_plugins() {
  $path = drupal_get_path('module', 'views_galleriffic');
  return array(
    'module' => 'views_galleriffic',
    'style' => array(
      'views_galleriffic' => array(
        'title' => t('Galleriffic Gallery'),
        'help' => t('Display a view like a Galleriffic gallery.'),
        'handler' => 'views_plugin_style_views_galleriffic',
        'theme' => 'views_galleriffic_view',
        'theme path' => $path,
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'normal',
        'parent' => 'default',
      ),
    ),
    'row' => array(
      'gallerifficfields' => array(
        'title' => t('Galleriffic Fields'),
        'help' => t('Choose the fields to display in the Galleriffic gallery.'),
        'handler' => 'views_galleriffic_plugin_row_gallerifficrows',
        'theme' => 'views_galleriffic_view_gallerifficrows',
        'theme path' => $path,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
