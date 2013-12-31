<?php

/**
 * @defgroup Location Location: An API for working with locations.
 *
 * Public API for the Location module.
 */


/**
 * @file
 * An implementation of a universal API for location manipulation.  Provides functions for
 * postal_code proximity searching, deep-linking into online mapping services.  Currently,
 * some options are configured through an interface provided by location.module.
 */

include_once(DRUPAL_ROOT . '/' . drupal_get_path('module', 'location') . '/earth.inc');

/**
 * Get a deep-link to a mapping service such as Yahoo! Maps or MapPoint given an location.  The
 * call is delegated based on the 'country' value in the $location parameter.
 *
 * @param $location
 *   An associative array where
 *      'street'       => A string representing the street location
 *      'additional'   => A string for any additional portion of the street location
 *      'city'         => A string for the city name
 *      'province'     => The standard postal abbreviation for the province
 *      'country'      => The two-letter ISO code for the country of the location (REQUIRED)
 *      'postal_code'  => The international postal code for the location
 *
 * @return
 *   A link to a map provided by a third-party.  The idea is to encode the appropriate
 *   parameters as HTTP GET variables to the URL.
 *
 * @ingroup Location
 */
function location_map_link($location = array(), $link_text = 'See map: ') {
  if (!isset($location['country']) || $location['country'] == 'xx') {
    return '';
  }

  location_load_country($location['country']);

  $default_func = 'location_map_link_'. $location['country'] .'_default_providers';
  $providers_func = 'location_map_link_'. $location['country'] .'_providers';
  $providers = function_exists($providers_func) ? $providers_func() : array();
  $selected_providers = variable_get('location_map_link_'. $location['country'], function_exists($default_func) ? $default_func() : array());

  $links = array();
  foreach ($selected_providers as $mapper) {
    $link_func = 'location_map_link_'. $location['country'] .'_'. $mapper;
    if (function_exists($link_func)) {
      if ($link = $link_func($location)) {
        $links[] = '<a href="'. $link .'"'. (variable_get('location_maplink_external', 0) ? ' '. variable_get('location_maplink_external_method', 'target="_blank"') : '') .'>'. $providers[$mapper]['name'] .'</a>';
      }
    }
  }
  if (count($links)) {
    return '<div class="location map-link">'. t($link_text) . implode($links, ", ") .'</div>';
  }
  else {
    return NULL;
  }
}

/**
 * Try to extract the the Latitude and Longitude data from the
 * postal code.
 *
 * @param $location
 *   Array. the location data
 *   -> the values are:
 *     'street'         => the string representing the street location (REQUIRED)
 *     'additional'     => the string representing the additional street location portion in the location form
 *     'city'           => the city name (REQUIRED)
 *     'province'       => the province code defined in the country-specific include file
 *     'country'        => the lower-case of the two-letter ISO code (REQUIRED)
 *     'postal_code'    => the postal-code (REQUIRED)
 *
 * @return
 *   Array or NULL. NULL if the delegated-to function that does the
 *   actual look-up does not exist. If the appropriate function exists,
 *   then this function returns an associative array where
 *    'lon' => A floating point number for the longitude coordinate of the parameter location
 *    'lat' => A floating point number for the latitude coordinate of the parameter location
 *
 * @ingroup Location
 */
function location_get_postalcode_data($location = array()) {
  $location['country'] = isset($location['country']) ? trim($location['country']) : NULL;
  $location['postal_code'] = isset($location['postal_code']) ? trim($location['postal_code']) : NULL;
  if (is_null($location['postal_code']) || is_null($location['country']) || empty($location['country']) || empty($location['postal_code']) || $location['postal_code'] == 'xx') {
    return NULL;
  }
  location_load_country($location['country']);
  $country_specific_function = 'location_get_postalcode_data_'. $location['country'];
  if (function_exists($country_specific_function)) {
    return $country_specific_function($location);
  }
  else {
    return NULL;
  }
}

/**
 * Given two points in lat/lon form, returns the distance between them.
 *
 * @param $latlon_a
 *   An associative array where
 *      'lon' => is a floating point of the longitude coordinate for the point given by latlonA
 *      'lat' => is a floating point of the latitude coordinate for the point given by latlonB
 *
 * @param $latlon_b
 *      Another point formatted like $latlon_b
 *
 * @param $distance_unit
 *      A string that is either 'km' or 'mile'.
 *      If neither 'km' or 'mile' is passed, the parameter is forced to 'km'
 *
 * @return
 *    NULL if sense can't be made of the parameters.
 *    An associative array where
 *      'scalar' => Is the distance between the two lat/lon parameter points
 *      'distance_unit' => Is the unit of distance being represented by 'scalar'.
 *                         This will be 'km' unless 'mile' is passed for the $distance_unit param
 *
 * @ingroup Location
 */
function location_distance_between($latlon_a = array(), $latlon_b = array(), $distance_unit = 'km') {
  if (!isset($latlon_a['lon']) || !isset($latlon_a['lat']) || !isset($latlon_b['lon']) || !isset($latlon_b['lat'])) {
    return NULL;
  }

  if ($distance_unit != 'km' && $distance_unit != 'mile') {
    return NULL;
  }

  // $conversion_factor = number to divide by to convert meters to $distance_unit
  // At this point, $distance_unit == 'km' or 'mile' and nothing else
  //$conversion_factor = ($distance_unit == 'km') ? 1000.0 : 1609.347;

  $meters = earth_distance($latlon_a['lon'], $latlon_a['lat'], $latlon_b['lon'], $latlon_b['lat']);
  return array('scalar' => round($meters/(($distance_unit == 'km') ? 1000.0 : 1609.347), 1), 'distance_unit' => $distance_unit);
}

/**
 * Takes two locations and tries to return a deep-link to driving directions.
 *
 * Parameters:
 * @param $location_a
 *   An associative array that represents an location where
 *      'street'       => the street portions of the location
 *      'additional'   => additional street portion of the location
 *      'city'         => the city name
 *      'province'     => the province, state, or territory
 *      'country'      => lower-cased two-letter ISO code (REQUIRED)
 *      'postal_code'  => the postal code
 *
 * @param $location_b
 *   An associative array that represents an location in the same way that
 *   parameter $location_a does.
 *
 * @param $link_text
 *   The text of the HTML link that is to be generated.
 *
 * @return
 *   A deep-link to driving directions on Yahoo! or some other mapping service, if enough fields are filled in the parameters.
 *   A deep-link to a form for driving directions with information pre-filled if not enough, but some fields are filled in the parameters.
 *   The empty string if no information is provided (or if so little information is provided that there is no function to which to delegate
 *   the call.
 *
 *   We dispatch the call to a country-specific function.  The country-specific function, in this case,
 *   will be the one reflected by the country parameter of the first function.  We require that
 *   both locationes supplied have a country field at the minimum.
 *
 *   The country-specific functions will ultimately decide, with the parameters given, whether to
 *   to link to a form for driving directions is provided, where this form will be
 *   pre-populated with whatever values were available or whether to link directly to the driving
 *   directions themselves if enough fields are filled for each location.
 *
 * @ingroup Location
 */
function location_driving_directions_link($location_a = array(), $location_b = array(), $link_text = 'Get directions') {
  if (!isset($location_a['country']) or !isset($location_b['country'])) {
    return '';
  }

  // For now, return empty string if starting-point and destinations are in different countries
  //if ($location_a['country'] != $location_b['country']) {
  //  return '';
  //}
  // Lines above commented out because I want to let the country-specific function of the departure point decide
  // what it will do with driving destination locationes from other countries.  As an example, Yahoo! Maps supports driving
  // direction queries for locations between the U.S. and Canada.

  $driving_direction_function = 'location_driving_directions_link_'. $location_a['country'];
  if (function_exists($driving_direction_function)) {
    $http_link = $driving_direction_function($location_a, $location_b);
    if (strlen($http_link)) {
      return '<a href="'. $http_link .'">'. $link_text .'</a>';
    }
    else {
      return '';
    }
  }

  return '';
}

/**
 * @param $distance
 *   A number in either miles or km.
 *
 * @param $distance_unit
 *   String (optional). Either 'mile' or 'km' (default)
 *
 * @return
 *   A floating point number where the number in meters after the initially passed scalar has been ceil()'d
 *   This is done after the $distance_unit parmeter is forced to be 'km' or 'mile'
 */
function _location_convert_distance_to_meters($distance, $distance_unit = 'km') {
  if (!is_numeric($distance)) {
    return NULL;
  }

  if ($distance == 0) {
    return NULL;
  }

  if ($distance_unit != 'km' && $distance_unit != 'mile') {
    $distance_unit = 'km';
  }

  // Convert distance to meters
  $retval = round(floatval($distance) * (($distance_unit == 'km') ? 1000.0 : 1609.347), 2);
  return $retval;
}

/**
 * Takes an location and returns a "rough" latitude/longitude pair based on the postal code
 * data available for the given country.
 *
 * @param $location
 *   An associative array $location where
 *     'street'       => the street portion of the location
 *     'additional' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => international postal code (REQUIRED)
 *
 * @return
 *   NULL if data cannont be found.
 *   Otherwise, an associative array where
 *     'lat' => is a floating point of the latitude coordinate of this location
 *     'lon' => is a floating point of the longitude coordinate of this location
 *
 * @ingroup Location
 */
function location_latlon_rough($location = array()) {
  if (!isset($location['country']) || !isset($location['postal_code'])) {
    return NULL;
  }

  location_load_country($location['country']);

  $latlon_function = 'location_latlon_rough_'. $location['country'];
  if (function_exists($latlon_function) && $result = $latlon_function($location)) {
    return $result;
  }
  else {
    return location_latlon_rough_default($location);
  }
}

/**
 * Returns a lat/lon pair of the approximate center of the given postal code in the given country
 * This function is a default implementation, in case the country support doesn't implement a proper function for this.
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
function location_latlon_rough_default($location = array()) {
  if (!isset($location['country']) || !isset($location['postal_code'])) {
    return NULL;
  }

  $query = db_select('zipcodes', 'z');
  $query->addField('z', 'latitude', 'lat');
  $query->addField('z', 'longitude', 'lon');
  $coords = $query->condition('country', $location['country'])
    ->condition('zip', $location['postal_code'])
    ->execute()
    ->fetchAssoc();

  if ($coords) {
    return $coords;
  }
  // if nothing found internally (new zipcode, or incomplete zipcodes table)
  elseif (($newlatlong = location_latlon_exact($location)) != NULL) {
    // try a one-time external geocoding
    if ($newlatlong['lat']) {
      // and store results in zipcodes table for next lookups being internally handled
      // (yeah this is missing city/state info a.t.m., but is way better than nothing!)
      db_insert('zipcodes')
        ->fields(array(
          'latitude' => $newlatlong['lat'],
          'longitude' => $newlatlong['lon'],
          'country' => $location['country'],
          'zip' => $location['postal_code'],
        ))
        ->execute();
    }
    return $newlatlong;
  }
  else {
    return NULL;
  }
}

/**
 * Get an appropriate bounding box for showing an entire country on a map.
 * Target is a bounding box large enough to show the country in both spherical
 * mercator and mercator projections.
 * @param $location
 *   Either a location array or a country code.
 *
 * @return
 *   An array with 'minlng', 'minlat', 'maxlng', and 'maxlat' elements.
 */
function location_country_bounds($location = array()) {
  if (!is_array($location)) {
    $location = array('country' => $location);
  }
  if (!empty($location['country'])) {
    location_load_country($location['country']);
  }

  $country_bounds_function = 'location_bounds_' . $location['country'];

  if (function_exists($country_bounds_function)) {
    return $country_bounds_function();
  }
  else {
    return array(
      'minlng' => -180.0,
      'minlat' => -90.0,
      'maxlng' => 180.0,
      'maxlat' => 90.0,
    );
  }
}

/**
 * Currently, this is not a priority until there is an implementable use for exact longitude,
 * latitude coordinates for an location.  The idea is that this call will eventually retrieve
 * information through a web-service.  Whereas location_latlon_rough() returns an approximate
 * lat/lon pair based strictly on the postal code where this lat/lon pair is pulled from a
 * database table, this function is intended to send the entire location to a web-service and
 * to retrieve exact lat/lon coordinates.
 *
 * @param $location
 *   An array where
 *     -> the key values are 'street', 'additional', 'province', 'country', 'postal_code'
 *     -> the values are:
 *         'street'         => the string representing the street location (REQUIRED)
 *         'additional'     => the string representing the additional street location portion in the location form
 *         'city'           => the city name (REQUIRED)
 *         'province'       => the province code defined in the country-specific include file
 *         'country'        => the lower-case of the two-letter ISO code (REQUIRED)
 *         'postal_code'    => the postal-code (REQUIRED)
 *
 * @return
 *   NULL if the delegated-to function that does the actual look-up does not exist.
 *   If the appropriate function exists, then this function returns an associative array where
 *      'lon' => A floating point number for the longitude coordinate of the parameter location
 *      'lat' => A floating point number for the latitude coordinate of the parameter location
 *
 * @ingroup Location
 */
function location_latlon_exact($location = array()) {
  $country = $location['country'];
  location_standardize_country_code($country);
  $service = variable_get('location_geocode_'. $country, 'none');
  if (!empty($country) && $service != 'none') {
    // figure out what the exact function should be
    if (strpos($service, '|')) {
      location_load_country($country);
      // The code change below fixes the problem of the country specific
      // function for geocoding not being correctly called (it removes any
      // text from the pipe (|) onwards)
      $exact_latlon_function = 'location_geocode_'. $country .'_'. substr($service, 0, strpos($service, '|'));
    }
    else {
      location_load_geocoder($service);
      $exact_latlon_function = $service .'_geocode_location';
    }
    if (function_exists($exact_latlon_function)) {
      return $exact_latlon_function($location);
    }
    else {
      return NULL;
    }
  }
  return NULL;
}

/**
 * Returns an associative array of countries currently supported
 * by the location system where
 *   -> the keys represent the two-letter ISO code and
 *   -> the values represent the English name of the country.
 * The array is sorted the index (i.e., by the short English name of the country).
 *
 * Please note the different between "supported" countries and "configured"
 * countries: A country being "supported" means that there is an include file
 * to support the country while "configure" implies that the site admin has
 * configured the site to actually use that country.
 *
 * @ingroup Location
 */
function _location_supported_countries() {
  $supported_countries = &drupal_static(__FUNCTION__, array());


  // If this function has already been called this request, we can avoid a DB hit.
  if (!empty($supported_countries)) {
    return $supported_countries;
  }

  // Try first to load from cache, it's much faster than the scan below.
  if ($cache = cache_get('location:supported-countries', 'cache_location')) {
    $supported_countries = $cache->data;
  }
  else {
    // '<ISO two-letter code>' => '<English name for country>'
    $iso_list = location_get_iso3166_list();
    $path = drupal_get_path('module', 'location') .'/supported/location.';
    foreach ($iso_list as $cc => $name) {
      if (file_exists($path . $cc .'.inc')) {
        $supported_countries[$cc] = $name;
      }
    }
    cache_set('location:supported-countries', $supported_countries, 'cache_location');
  }
  return $supported_countries;
}

// @@@ New in 3.x, document.
/**
 * Fetch the provinces for a country.
 */
function location_get_provinces($country = 'us') {
  $provinces = &drupal_static(__FUNCTION__, array());

  location_standardize_country_code($country);
  if (isset($provinces[$country])) {
    return $provinces[$country];
  }
  if ($cache = cache_get("provinces:$country", 'cache_location')) {
    $provinces[$country] = $cache->data;
    return $provinces[$country];
  }
  location_load_country($country);
  $func = 'location_province_list_'. $country;
  if (function_exists($func)) {
    $provinces[$country] = $func();
    cache_set("provinces:$country", $provinces[$country], 'cache_location');
    return $provinces[$country];
  }
  return array();
}

// @@@ New in 3.x, document.
/**
 * Get the translated name of a country code.
 */
function location_country_name($country = 'us') {
  location_standardize_country_code($country);
  $countries = location_get_iso3166_list();
  if (isset($countries[$country])) {
    return $countries[$country];
  }
  else {
    return '';
  }
}

// @@@ New in 3.x, document.
/**
 * Get the full name of a province code.
 */
function location_province_name($country = 'us', $province = 'xx') {
  $provinces = location_get_provinces($country);
  $province = strtoupper($province);
  if (isset($provinces[$province])) {
    return $provinces[$province];
  }
  else {
    return '';
  }
}

// @@@ New in 3.x, document.
/**
 * Get a province code from a code or full name and a country.
 */
function location_province_code($country = 'us', $province = 'xx') {
  // An array of countries is useful if someone specified multiple countries
  // in an autoselect for example.
  // It *is* possibly ambiguous, especially if the province was already a code.
  // We make an array here for single (the usual case) for code simplicity reasons.
  if (!is_array($country)) {
    $country = array($country);
  }

  $p = strtoupper($province);
  foreach ($country as $c) {
    $provinces = location_get_provinces($c);
    foreach ($provinces as $k => $v) {
      if ($p == strtoupper($k) || $p == strtoupper($v)) {
        return $k;
      }
    }
  }
  return '';
}

// @@@ New in 3.x, document.
/**
 * Canonicalize a country code.
 */
function location_standardize_country_code(&$country) {
  $country = trim($country);
  // @@@ Double check the validity of this validity check. ;)
  if (!ctype_alpha($country) || strlen($country) != 2) {
    $country = 'xx';
    return FALSE;
  }
  else {
    $country = strtolower($country);
    return TRUE;
  }
}

/**
 * Load support for a country.
 *
 * This function will load support for a country identified by its two-letter ISO code.
 *
 * @param $country
 *   Two-letter ISO code for country.
 *
 * @return
 *   TRUE if the file was found and loaded, FALSE otherwise.
 */
function location_load_country($country) {
  location_standardize_country_code($country);

  $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'location') . '/supported/location.' . $country . '.inc';
  if (file_exists($file)) {
    include_once($file);
    return TRUE;
  }
  return FALSE;
}

// @@@ New in 3.x, document.
/**
 * Load a general geocoding service.
 */
function location_load_geocoder($geocoder) {
  include_once(DRUPAL_ROOT . '/' . drupal_get_path('module', 'location') . '/geocoding/' . $geocoder . '.inc');
}

/**
 * Create a single line address.
 *
 * @param $location
 *   Array. The address parts
 * @return
 *   String. The single line address
 */
function location_address2singleline($location = array()) {
  // Check if its a valid address
  if (empty($location)) {
    return '';
  }

  $address = '';
  if (!empty($location['street'])) {
    $address .= $location['street'];
  }

  if (!empty($location['city'])) {
    if (!empty($location['street'])) {
      $address .= ', ';
    }

    $address .= $location['city'];
  }

  if (!empty($location['province'])) {
    if (!empty($location['street']) || !empty($location['city'])) {
      $address .= ', ';
    }

    // @@@ Fix this!
    if (substr($location['province'], 0, 3) == $location['country'] .'-') {
      $address .= substr($location['province'], 3);
      watchdog('Location', 'BUG: Country found in province attribute.');
    }
    else {
      $address .= $location['province'];
    }
  }

  if (!empty($location['postal_code'])) {
    if (!empty($address)) {
      $address .= ' ';
    }
    $address .= $location['postal_code'];
  }

  if (!empty($location['country'])) {
    $address .= ', '. $location['country'];
  }

  return $address;
}

function location_get_general_geocoder_list() {
  $list = &drupal_static(__FUNCTION__, array());

  if (!count($list)) {
    $files = file_scan_directory(drupal_get_path('module', 'location') . '/geocoding', '/\.inc$/', array('nomask' => '/(\.\.?|CVS|\.svn)$/'));
    foreach ($files as $full_path_name => $fileinfo) {
      $list[] = $fileinfo->name;
    }
  }

  return $list;
}

/**
 * The following is an array of all
 * countrycode => country-name pairs as layed out in
 * ISO 3166-1 alpha-2
 */
function location_get_iso3166_list($upper = FALSE) {
  include_once DRUPAL_ROOT . '/includes/locale.inc';

  // Statically cache a version of the core Drupal list of countries
  // with lower case country codes for use by this module.
  $countries = &drupal_static(__FUNCTION__);

  if ($upper) {
    // Drupal core stores ISO 3166-1 alpha2 codes in upper case, as
    // per the ISO standard.
    return country_get_list();
  }
  elseif (!isset($countries)) {
    // Location module uses lower-case ISO 3166-1 alpha2 codes, so we need
    // to convert.
    $countries = array_change_key_case(country_get_list(), CASE_LOWER);
  }
  return $countries;
}
