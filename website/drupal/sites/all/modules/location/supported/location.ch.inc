<?php

// Switzerland

function location_province_list_ch() {
  return array(
    'ZH' => "Zurich",
    'BE' => "Bern",
    'LU' => "Lucerne",
    'UR' => "Uri",
    'SZ' => "Schwyz",
    'OW' => "Obwalden",
    'NW' => "Nidwalden",
    'GL' => "Glarus",
    'ZG' => "Zug",
    'FR' => "Fribourg",
    'SO' => "Solothurn",
    'BS' => "Basel-Stadt",
    'BL' => "Basel-Landschaft",
    'SH' => "Schaffhausen",
    'AR' => "Appenzell Ausserrhoden",
    'AI' => "Appenzell Innerhoden",
    'SG' => "St. Gallen",
    'GR' => "Graubunden",
    'AG' => "Aargau",
    'TG' => "Thurgau",
    'TI' => "Ticino",
    'VD' => "Vaud",
    'VS' => "Valais",
    'NE' => "Neuchatel",
    'GE' => "Geneva",
    'JU' => "Jura",
  );
}

function location_map_link_ch_providers() {
  return array(
    'search' => array(
      'name' => 'map.search.ch',
      'url' => 'http://map.search.ch',
      'tos' => 'http://map.search.ch/terms.html',
    ),
    'google' => array(
      'name' => 'Google Maps',
      'url' => 'http://maps.google.ch',
      'tos' => 'http://www.google.ch/help/terms_maps.html',
    ),
  );
}

function location_map_link_ch_default_providers() {
  return array('search', 'google');
}

function location_map_link_ch_google($location = array()) {
  $query_params = array();

  foreach (array('street', 'city', 'postal_code', 'country') as $field) {
    if (isset($location[$field])) {
      $query_params[] = $location[$field];
    }
  }

  if (count($query_params)) {
    return ('http://maps.google.ch?q='. urlencode(implode(', ', $query_params)));
  }
  else {
    return NULL;
  }
}

function location_map_link_ch_search($location = array()) {
  $query_param = '';

  if ($location['postal_code'] || $location['city'] || $location['street']) {
    $query_param = $location['postal_code'] . '-' . $location['city'] . '/' . $location['street'];
    return 'http://map.search.ch/' . $query_param;
  }
  else {
    return NULL;
  }
}

function location_driving_directions_link_ch($location_a, $location_b) {
  return _location_driving_directions_link_ch_search($location_a, $location_b);
}

function _location_driving_directions_link_ch_search($location_a, $location_b) {
  $query_params_a = array();
  $query_params_b = array();

  foreach (array('street', 'postal_code', 'city') as $field) {
    if (isset($location_a[$field])) {
      $query_params_a[] = $location_a[$field];
    }
    if (isset($location_b[$field])) {
      $query_params_b[] = $location_b[$field];
    }
  }
  return 'http://route.search.ch/?route=' . urlencode(implode(' ', $query_params_a) . ' to ' .implode(' ', $query_params_b));
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ch() {
  return array(
    'minlng' => 5.8814,
    'minlat' => 45.7606,
    'maxlng' => 10.58005,
    'maxlat' => 47.693367,
  );
}
