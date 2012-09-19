<?php

function location_province_list_de() {
  return array(
    'BB' => 'Brandenburg',
    'BE' => 'Berlin',
    'BW' => 'Baden-Württemberg',
    'BY' => 'Bayern',
    'HB' => 'Bremen',
    'HE' => 'Hessen',
    'HH' => 'Hamburg',
    'MV' => 'Mecklenburg-Vorpommern',
    'NI' => 'Niedersachsen',
    'NW' => 'Nordrhein-Westfalen',
    'RP' => 'Rheinland-Pfalz',
    'SH' => 'Schleswig-Holstein',
    'SL' => 'Saarland',
    'SN' => 'Sachsen',
    'ST' => 'Sachsen-Anhalt',
    'TH' => 'Thüringen'
  );
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
 */
function location_map_link_de($location = array(), $hide = array()) {
  $map_links = array();
  // For now, just call the suchen (tinfo) function.  May want to make this configurable on some level
  // in order to maintain freedom of choice so users and site-admins don't have to be slaves
  // to tinfo!.... not that I have anything personal against tinfo!.
  if ($link = _location_map_link_de_suchen($location)) {
    $map_links['suchen'] = $link;
  }
  return $map_links;
}

function _location_map_link_de_suchen($location = array()) {
  $get_query = '?';
  $get_query .= 'where=';
  $query_parts = array();
  if (isset($location['street'])) {
    $query_parts[] = $location['street'];
  }
  if ($location['postal_code'] != '') {
    $query_parts[] = $location['postal_code'];
  }
  if ($location['city'] != '') {
    $query_parts[] = $location['city'];
  }
//  if ($location['number'] != '') {
//    $query_parts[] = $location['number'];
//  }
  $get_query .= urlencode(implode(', ', $query_parts));
  return ('http://www.suchen.de/lokalmap'. $get_query);
}

function location_map_link_de_providers() {
  return array(
    'suchen' => array(
      'name' => 'suchen.de (T-Info)',
      'url' => 'http://www.suchen.de/',
      'tos' => 'http://www.suchen.de/agb',
    ),
    'google' => array(
      'name' => 'Google Maps',
      'url' => 'http://maps.google.com',
      'tos' => 'http://www.google.com/help/terms_local.html',
    ),
  );
}

function location_map_link_de_default_providers() {
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
 * For now, assume site-admin wants driving directions linked to tinfo!
 * Maybe later, we can add some kind of country-specific settings page that allows the site-admin to
 * decide which site to link to for driving directions.
 */
function location_driving_directions_link_de($location_a, $location_b) {
  return _location_driving_directions_link_de_suchen($location_a, $location_b);
}

/**
 * Parameters:
 *    Function that is called by location_driving_directions_link_ca() under assumption that it
 *    is the chosen function
 *
 * Returns:
 *    a URL with HTTP GET variables
 *    Depending on how full the locationes are, the URL will either point to the driving directions
 *    on tinfo! or, if only partial locationes are provided, a URL that points to the *form* for
 *    tinfo! driving directions where the form is filled with whatever fields have been provided
 *    for the partial location(es).
 */
function _location_driving_directions_link_de_suchen($location_a, $location_b) {

  foreach ($location_a as $field => $value) {
    $location_a[$field] = trim($value);
  }

  foreach ($location_b as $field => $value) {
    $location_b[$field] = trim($value);
  }

  if ($location_a['country'] == 'de' and $location_b['country'] == 'de') {
    $get_query = '?';

    // VON
    $query_parts = array();
    if (isset($location_a['street'])) {
      $query_parts[] = $location_a['street'];
    }
    if ($location_a['postal_code'] != '') {
      $query_parts[] = $location_a['postal_code'];
    }
    if ($location_a['city'] != '') {
      $query_parts[] = $location_a['city'];
    }
    $get_query .= 'route_from='. urlencode(implode(', ', $query_parts));

    // NACH
    $query_parts = array();
    if (isset($location_b['street'])) {
      $query_parts[] = $location_b['street'];
    }
    if ($location_b['postal_code'] != '') {
      $query_parts[] = $location_b['postal_code'];
    }
    if ($location_b['city'] != '') {
      $query_parts[] = $location_b['city'];
    }
    $get_query .= '&amp;route_to='. urlencode(implode(', ', $query_parts));

    return ('http://www.suchen.de/route'. $get_query);
  }
}

function location_map_link_de_google($location = array()) {
  $query_params = array();

  foreach (array('street', 'postal_code', 'city', 'country') as $field) {
    if (isset($location[$field])) {
      $query_params[] = $location[$field];
    }
  }

  if (count($query_params)) {
    return ('http://maps.google.com?q='. urlencode(implode(', ', $query_params)));
  }
  else {
    return NULL;
  }
}

function theme_location_de($location = array(), $hide = array()) {
  $output = '';
  if (count($location)) {
    $output .= "\n";
    $output .= '<div class="location vcard"><div class="adr">'."\n";
    if (!empty($location['name']) && !in_array('name', $hide)) {
      $output .= '<div class="fn">'. $location['name'] .'</div>';
    }

    if (!empty($location['street']) && !in_array('street', $hide)) {
      $output .= '<div class="street-address">'. $location['street'];
      if (!empty($location['additional']) && !in_array('street', $hide)) {
        $output .= ' '. $location['additional'];
      }
      $output .='</div>';
    }

    if ((!empty($location['city']) && !in_array('city', $hide)) ||
        (!empty($location['postal_codet']) && !in_array('postal_code', $hide))) {

      $city_postal = array();

      if (!empty($location['postal_code']) && !in_array('postal_code', $hide)) {
        $city_postal[] = '<span class="postal-code">'. $location['postal_code'] .'</span>';
      }

      if (!empty($location['city']) && !in_array('city', $hide)) {
        $city_postal[] = '<span class="locality">'. $location['city'] .'</span>';
      }

      $output .= '<div>'. implode(' ', $city_postal) .'</div>';
    }

    if (!in_array('country', $hide)) {
      $output .= '<div class="country-name">'. t('Germany') .'</div>';
    }

    if (location_has_coordinates($location)) {
      $output .=  '<div class="geo"><abbr class="latitude" title="'. $location['latitude'] .'" /><abbr class="longitude" title="'. $location['latitude'] .'" /></div>';
    }

    $output .= '</div></div>';

  }
  return $output;
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
 *
 */
function location_get_postalcode_data_de($location = array()) {
  $dash_index = strpos($location['postal_code'], '-');
  // First we strip slash off if we're dealing with a 9-digit US zipcode
  if ($dash_index === FALSE) {
    $location['postal_code'] = substr($location['postal_code'], 0, $dash_index);
  }

  // Now we pad the thing and query.
  $row = db_query("SELECT * FROM {zipcodes} where country = :country AND zip = :zip", array(':country' => $location['country'], ':zip' => str_pad($location['postal_code'], 5, "0", STR_PAD_LEFT)))->fetchObject();
  if ($row) {
    return array('lat' => $row->latitude, 'lon' => $row->longitude, 'city' => $row->city, 'province' => $row->state, 'country' => $row->country);
  }
  else {
    return NULL;
  }
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_de() {
  return array(
    'minlng' => 5.87225,
    'minlat' => 47.2249,
    'maxlng' => 15.04765,
    'maxlat' => 55.0839,
  );
}
