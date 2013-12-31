<?php

/**
 * @file
 * Creates a location context.
 */

/**
 * Implement hook_[context_name]_ctools_contexts().
 */
function location_location_ctools_contexts() {
  return array(
    'title' => t('Location'),
    'description' => t('Location'),
    'context' => 'location_context_create_location',
    'context name' => 'location',
    'settings form' => 'location_context_location_settings_form',
    'keyword' => 'location',
    'context name' => 'location',
    'convert list' => 'location_context_location_convert_list',
    'convert' => 'location_context_location_convert',
  );
}

/**
 * Create a context, either from configuration or an argument on the URL.
 *
 * @param $empty
 *   If true, just return an empty context.
 * @param $data
 *   If from settings form, a form values array. If from argument, a string.
 * @param $conf
 *   TRUE if the $data is coming from admin configuration, FALSE if it's
 *   from a URL arg.
 *
 * @return
 *   a Context object
 */
function location_context_create_location($empty, $data = NULL, $conf = FALSE) {
  $context = new ctools_context('location');
  $context->plugin = 'location';

  if ($empty) {
    return $context;
  }

  if ($conf) {
    $lid = is_array($data) && isset($data['lid']) ? $data['lid'] : (is_object($data) ? $data->lid : 0);
    if (is_array($data) || !empty($reload)) {
      $nid = $data['nid'];
      $data = location_load_location($lid);
      $data['nid'] = $nid;
    }
  }

  if (!empty($data))  {
    $context->data = $data;
    $context->title = $data['city'];
    $context->argument = $data['lid'];
    return $context;
  }
}

function location_settings_form($conf, $external = FALSE) {
  return array();
}

/**
 * Provide a list of ways that this context can be converted to a string.
 */
function location_context_location_convert_list() {
  $fields = location_field_names();
  $fields['country_name'] = t('Country name');
  $fields['province_name'] = t('State/Province name');
  return $fields;
}

/**
 * Convert a context into a string.
 */
function location_context_location_convert($context, $type) {
  switch ($type) {
    case 'name':
    case 'street':
    case 'additional':
    case 'city':
    case 'province':
    case 'postal_code':
    case 'country':
    case 'latitude':
    case 'longitude':
    case 'province_name':
    case 'country_name':
      if (isset($context->data[$type])) {
        return check_plain($context->data[$type]);
      }

    default:
      return t('Unknown location keyword');

  }
}
