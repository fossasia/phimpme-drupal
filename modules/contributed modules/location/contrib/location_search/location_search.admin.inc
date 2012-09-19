<?php

/**
 * @file
 *   Administration related forms for the location_search module.
 */

/**
 * Admin settings form.
 *
 * @ingroup form
 */
function location_search_admin_settings($form, &$form_state) {
  $gmap = module_exists('gmap');
  $form['location_search_map'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use map to pick search coordinates'),
    '#return_value' => 1,
    '#default_value' => variable_get('location_search_map', 1),
    '#description' => $gmap ? t('Requires the gmap module.') : t('You must install the gmap module to enable this feature.'),
    '#disabled' => !$gmap,
  );
  $form['location_search_map_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add geocoding address field to search map'),
    '#return_value' => 1,
    '#default_value' => variable_get('location_search_map_address', 1),
  );
  $form['location_search_map_macro'] = array(
    '#type' => 'textfield',
    '#title' => t('Macro for search map'),
    '#default_value' => variable_get('location_search_map_macro', '[gmap |behavior=+collapsehack]'),
  );

  return system_settings_form($form);
}
