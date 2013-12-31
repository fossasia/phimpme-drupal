<?php

function location_province_list_ca() {
  return array(
    'AB' => 'Alberta',
    'BC' => 'British Columbia',
    'MB' => 'Manitoba',
    'NB' => 'New Brunswick',
    'NL' => 'Newfoundland and Labrador',
    'NS' => 'Nova Scotia',
    'ON' => 'Ontario',
    'PE' => 'Prince Edward Island',
    'QC' => 'Quebec',
    'SK' => 'Saskatchewan',
    'NT' => 'Northwest Territories',
    'NU' => 'Nunavut',
    'YT' => 'Yukon Territory',
  );
}

function location_province_list_numeric_ca() {
  return array(
    '001' => 'Alberta',
    '002' => 'British Columbia',
    '003' => 'Manitoba',
    '004' => 'New Brunswick',
    '005' => 'Newfoundland and Labrador',
    '006' => 'Nova Scotia',
    '007' => 'Ontario',
    '008' => 'Prince Edward Island',
    '009' => 'Quebec',
    '010' => 'Saskatchewan',
    '011' => 'Northwest Territories',
    '012' => 'Nunavut',
    '013' => 'Yukon Territory',
  );
}



/**
 * Returns a lat/lon pair of the approximate center of the given postal code in the given country
 *
 * @param $location
 *   An associative array $location where
 *     'street'       => the street portion of the location
 *     'supplemental' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => the international postal code for this location (REQUIRED)
 *
 * @return
 *   An associative array where
 *      'lat' => approximate latitude of the center of the postal code's area
 *      'lon' => approximate longitude of the center of the postal code's area
 *
 */
function location_latlon_rough_ca($location = array()) {
  if (!isset($location['postal_code'])) {
    return NULL;
  }

  $row = db_query("SELECT latitude, longitude FROM {zipcodes} WHERE country = :country AND zip = :zip", array(':country' => $location['country'], ':zip' => $location['postal_code']))->fetchObject();

  if ($row) {
    return array('lat' => $row->latitude, 'lon' => $row->longitude);
  }
  else {
    return NULL;
  }
}

/**
 * Returns a lat/lon pair of the approximate center of the given postal code in the given country
 *
 * @param $location
 *   An associative array $location where only postal code and country are necessary, but can have the keys:
 *     'street'       => the street portion of the location
 *     'supplemental' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => the international postal code for this location (REQUIRED)
 *
 * @return
 *   An associative array where
 *      'lat' => approximate latitude of the center of the postal code's area
 *      'lon' => approximate longitude of the center of the postal code's area
 *      'city'     => the city
 *      'province'     => the province, state, or territory
 *      'country'      => lower-cased two-letter ISO code
 *
 */
function location_get_postalcode_data_ca($location = array()) {
  // Now we pad the thing and query.
  $row = db_query("SELECT * FROM {zipcodes} where country = :country AND zip = :zip", array(':country' => $location['country'], ':zip' => $location['postal_code']))->fetchObject();
  if ($row) {
    return array('lat' => $row->latitude, 'lon' => $row->longitude, 'city' => $row->city, 'province' => $row->state, 'country' => $row->country);
  }
  else {
    return NULL;
  }
}

/**
 * Parameters:
 *   An associative array $location where
 *     'street'       => the street portion of the location
 *     'supplemental' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => the international postal code for this location (REQUIRED)
 *
 *
 */
function location_latlon_exact_ca($location = array()) {
  // return location_geocode_ca_geocoder($location);
}

function location_map_link_ca_yahoo($location = array()) {
  $get_query = '?';
  if (isset($location['street'])) {
    $get_query .= 'addr='. urlencode($location['street']) .'&amp;';
  }
  if ($location['province'] != '' || $location['city'] != '' || $location['postal_code'] != '') {
    $get_query .= 'csz='. _location_ca_yahoo_csz_get_field($location) .'&amp;';
  }
  $get_query .= 'country='. urlencode($location['country']);
  return ('http://ca.maps.yahoo.com/maps_result'. $get_query);
}

function location_map_link_ca_google($location = array()) {
  $query_params = array();

  foreach (array('street', 'city', 'province', 'postal_code', 'country') as $field) {
    if (isset($location[$field])) {
      $query_params[] = $location[$field];
    }
  }

  if (count($query_params)) {
    return ('http://maps.google.ca?q='. urlencode(implode(", ", $query_params)));
  }
  else {
    return NULL;
  }
}

function location_map_link_ca_mapquest($location = array()) {
  if (isset($location['street'])) {
    $get_query .= 'address='. urlencode($location['street']) .'&amp;';
  }

  if (isset($location['city'])) {
    $get_query .= 'city='. urlencode($location['city']) .'&amp;';
  }

  if (isset($location['province'])) {
    $get_query .= 'state='. urlencode($location['province']) .'&amp;';
  }

  if (isset($location['postal_code'])) {
    $get_query .= 'zipcode='. urlencode($location['postal_code']);
  }

  if (strlen($get_query)) {
    return 'http://www.mapquest.com/maps/map.adp?searchtype=address&amp;country=CA&amp;'. $get_query;
  }
  else {
    return NULL;
  }
}

function location_map_link_ca_providers() {
  return array('google' => array('name' => 'Google Maps', 'url' => 'http://maps.google.ca', 'tos' => 'http://www.google.ca/help/terms_local.html'),
               'yahoo' => array('name' => 'Yahoo! Maps', 'url' => 'http://ca.maps.yahoo.com' , 'tos' => 'http://help.yahoo.com/help/ca/maps/maps-25.html'),
               'mapquest' => array('name' => 'MapQuest', 'url' => 'http://www.mapquest.com', 'tos' => 'http://www.mapquest.com/features/main.adp?page=legal')
              );
}


function location_map_link_ca_default_providers() {
  return array('google');
}

/**
 * Parameters:
 *   -> $location_a is an associative array that represents a full location where
 *        'street'       => the street portions of the location
 *        'supplemental' => additional street portion of the location
 *        'province'     => the province, state, or territory
 *        'country'      => lower-cased two-letter ISO code (REQUIRED)
 *   -> $location_b is associative array that represents a full location in the same way that
 *       parameter $location_b does.
 *
 * Returns: a link to driving directions
 *
 * For now, assume site-admin wants American driving directions linked to Yahoo! Driving Directions.
 * Maybe later, we can add some kind of country-specific settings page that allows the site-admin to
 * decide which site to link to for driving directions.
 */
function location_driving_directions_link_ca($location_a, $location_b) {
  return _location_driving_directions_link_ca_yahoo($location_a, $location_b);
}


/**
 * Parameters:
 *    Function that is called by location_driving_directions_link_ca() under assumption that it
 *    is the chosen function
 *
 * Returns:
 *    a URL with HTTP GET variables
 *    Depending on how full the locationes are, the URL will either point to the driving directions
 *    on Yahoo! or, if only partial locationes are provided, a URL that points to the *form* for
 *    Yahoo! driving directions where the form is filled with whatever fields have been provided
 *    for the partial location(es).
 */
function _location_driving_directions_link_ca_yahoo($location_a, $location_b) {
  if (trim($location_b['country']) != 'ca' && trim($location_b['country']) != 'us') {
    return '';
  }

  // These are the fields that need to be in each location if we are to provide a direct
  // link to yahoo directions.  If all of these fields don't have values, then we generate
  // a link to the *form* for Yahoo! driving directions rather than directly to the driving
  // directions themselves.


  foreach ($location_a as $field => $value) {
    $location_a[$field] = trim($value);
  }

  foreach ($location_b as $field => $value) {
    $location_b[$field] = trim($value);
  }

  if (_location_ca_enough_fields_for_yahoo($location_a) && _location_ca_enough_fields_for_yahoo($location_b)) {
    $yahoo_maps_path = 'dd_result';
  }
  else {
    $yahoo_maps_path = 'dd';
  }

  $get_query  = '?';

  $get_query .= 'addr='. urlencode($location_a['street']) .'&amp;';
  $get_query .= 'csz='. _location_ca_yahoo_csz_get_field($location_a) .'&amp;';
  $get_query .= 'country='. urlencode($location_a['country']) .'&amp;';

  $get_query .= 'taddr='. urlencode($location_b['street']) .'&amp;';
  $get_query .= 'tcsz='. _location_ca_yahoo_csz_get_field($location_b) .'&amp;';
  $get_query .= 'tcountry='. urlencode($location_b['country']);

  $get_query .= '&amp;getrte='. urlencode('Get Directions');

  return ('http://ca.maps.yahoo.com/'. $yahoo_maps_path . $get_query);
}

function _location_ca_enough_fields_for_yahoo($location) {
  // These are the fields that need to be in each location if we are to provide a direct
  // link to yahoo directions.  If all of these fields don't have values, then we generate
  // a link to the *form* for Yahoo! driving directions rather than directly to the driving
  // directions themselves.
  if (strlen($location['street']) && strlen($location['city']) && strlen($location['province'])) {
    return TRUE;
  }

  if (strlen($location['street']) && strlen($location['postal_code'])) {
    return TRUE;
  }

  if (strlen($location['street']) && strlen($location['city']) && strlen($location['province'])) {
    return TRUE;
  }

  return FALSE;
}


// Don't mess with this function unless you understand its logic.  It has to do with
// the question of "to comma or not to comma?"
function _location_ca_yahoo_csz_get_field($location) {
  // For some reasons, to the end of pinpointing a location, Yahoo! Maps and Driving Directions
  // do better a better job with retrieving info based strictly on a Canadian city/province
  // than on a Canadian postal code.
  if ($location['country'] = 'ca') {
    if (strlen($location['city']) && strlen($location['province'])) {
      return urlencode($location['city'] .', '. $location['province']);
    }

    if (strlen($location['postal_code'])) {
      return urlencode($location['postal_code']);
    }
  }
  else {
    if (strlen($location['postal_code'])) {
      return urlencode($location['postal_code']);
    }

    if (strlen($location['city']) && strlen($location['province'])) {
      return urlencode($location['city'] .', '. $location['province']);
    }
  }

  if (strlen($location['city']) || strlen($location['province'])) {
    if (strlen($location['city'])) {
      return urlencode($location['city']);
    }
    else {
      return urlencode($location['province']);
    }
  }
  return '';
}

function location_geocode_ca_providers() {
  return array(
    'geocoder' => array('name' => 'GeoCode.ca geocoding service', 'url' => 'http://geocoder.ca/?api=1', 'tos' => 'http://geocoder.ca/?terms=1http://geocoder.ca/?terms=1'),
  );
}


function location_geocode_ca_geocoder_settings() {
  $form = array();

  $form['location_geocode_ca_geocoder_apikey'] = array(
    '#type' => 'textfield',
    '#title' => t('Geocoder API Key'),
    '#size' => 64,
    '#maxlength' => 128,
    '#default_value' => variable_get('location_geocode_ca_geocoder_apikey', ''),
    '#description' => t('In order to use the Geocoder.ca API geocoding web-service, if you are a commercial entity or wish to use the service without attribution, you will need a Geocoder API Key.  You can obtain one at the !sign_up_link for the !geocoder_api.', array('!sign_up_link' => '<a href="http://geocoder.ca/?register=1">sign-up page</a>', '!geocoder_api' => '<a href="http://geocoder.ca/?premium_api=1">Geocoder API</a>'))
  );

  return $form;
}


/**
 * This needs some more work to cover errors and such. I think showing a proper
 * Drupal error is a good idea. If an incorrect address is given to geocoder.ca
 * it offers a suggestion. If this happens the drupal user should be told.
 */
function location_geocode_ca_geocoder($location = array()) {
  $service_url = "http://geocoder.ca/?geoit=XML";

  if (variable_get('location_geocode_ca_geocoder_apikey', '')) {
    $service_url .= '&auth='. variable_get('location_geocode_ca_geocoder_apikey', NULL) .'&locate=';
  }
  else {
    $service_url .= '&locate=';
  }

  // geocoder gives better results when not usong postal code, country and ,
  unset($location['postal_code'], $location['country']);
  $address = strtr(location_address2singleline($location), array(', ' => ' '));

  $http_reply = drupal_http_request($service_url . urlencode($address));

  if ($http_reply->code == 400) {
    return NULL;
  }
  else {
    $matches = array();
    $lat_match = array();
    $lon_match = array();
    $latlon = array();

    if (preg_match('/<error>(.*)<\/error>/', $http_reply->data, $lat_match)) {
      return NULL;
    }

    if (preg_match('/<latt>(.*)<\/latt>/', $http_reply->data, $lat_match)) {
      $latlon['lat'] = $lat_match[1];
    }
    else {
      return NULL;
    }

    if (preg_match('/<longt>(.*)<\/longt>/', $http_reply->data, $lon_match)) {
      $latlon['lon'] = $lon_match[1];
      return $latlon;
    }
    else {
      return NULL;
    }
  }
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ca() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -141.00554,
    'minlat' => 41.6690855,
    'maxlng' => -52.615930,
    'maxlat' => 83.1161164,
  );
}
