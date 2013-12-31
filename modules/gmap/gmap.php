<?php

/**
 * @file
 * GMap plugin API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Change the way gmap works.
 *
 * @param $op
 *   The operation to be performed. Possible values:
 *   - "macro": Add macro behaviors (Not well documented yet...)
 *   - "pre_theme_map": A map is being themed. This is a good place to
 *     drupal_add_js() any additional files needed to run the map in question.
 *   - "macro_multiple": Add macro behaviors (Not well documented yet...)
 *   - "behaviors": Define behavior flags used in your code.
 * @param $map
 *   For the "pre_theme_map" operation, the map being themed. Not used in
 *   other operations.
 * @return
 *  This varies depending on the operation.
 *  - "macro": An associative array. The keys are the macro keys, the values
 *    are an array of flags for that macro key:
 *      "multiple": This key can appear multiple times in a macro.
 *  - "pre_theme_map": None.
 *  - "macro_multiple": An array of macro keys that can appear multiple times.
 *    (Oops, this appears to be redundant... Look into this. --Bdragon)
 *  - "behaviors": An associative array. The keys are the names of the behavior
 *    flags, and the values are associative arrays in the following format:
 *      - "title": The title to show on the settings page.
 *      - "default": The default state of the flag.
 *      - "help": A description of the flag to show on the settings page.
 *      - "internal": If TRUE, the flag will be marked as map specific, and
 *        will not be stored in the defaults.
 */
function hook_gmap($op, &$map) {
  switch ($op) {
    case 'macro':
      return array(
        'feed' => array(
          'multiple' => TRUE,
        ),
      );
    case 'pre_theme_map':
      $path = drupal_get_path('module', 'gmap') .'/js/';
      if (is_array($map['feed'])) {
        drupal_add_js($path .'markerloader_georss.js');
      }
      break;
    case 'macro_multiple':
      return array('feed');
    case 'behaviors':
      return array(
        'nomousezoom' => array(
          'title' => t('Disable mousezoom'),
          'default' => FALSE,
          'help' => t('Disable using the scroll wheel to zoom the map.'),
        ),
      );
  }
}

/**
 * @} End of "addtogroup hooks".
 */
