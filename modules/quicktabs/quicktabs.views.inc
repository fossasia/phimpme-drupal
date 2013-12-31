<?php
// Id:$

/**
 * @file Add Views module hooks to Quicktabs.
 */
 
/**
 * Implementation of hook_views_plugins().
 */
function quicktabs_views_plugins() {
  $path = drupal_get_path('module', 'quicktabs');
  
  return array(
    'style' => array(
      'quicktabs' => array(
        'title' => t('Quicktabs'),
        'help' => t('Display view in Quicktabs.'),
        'handler' => 'quicktabs_style_plugin',
        'path' => "$path/includes",
        'theme' => 'quicktabs_view',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
