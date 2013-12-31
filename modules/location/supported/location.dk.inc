<?php

// Denmark

function location_province_list_dk() {
  return array('CC' => "Copenhagen (municipality)",
    'FC' => "Frederiksberg (municipality)",
    'CO' => "Copenhagen",
    'FR' => "Frederiksborg",
    'RO' => "Roskilde",
    'WZ' => "West Zealand",
    'ST' => "Storstrom",
    'FU' => "Funen",
    'SJ' => "South Jutland",
    'RB' => "Ribe",
    'VK' => "Vejle",
    'RK' => "Ringkjobing",
    'VB' => "Viborg",
    'NJ' => "North Jutland",
    'AR' => "Arhus",
    'BH' => "Bornholm",
    'GL' => "Greenland",
    'FO' => "Faroe Islands");
}

/**
 * Generate findvej.dk map link.
 *
 * @param array $location
 *    The location array we're trying to generate a link for.
 * @return string or null
 *    Map link if generation successful, otherwise null.
 */
function location_map_link_dk_findvej($location = array()) {
  if (isset($location['street']) && !empty($location['street']) &&
      isset($location['postal_code']) && !empty($location['postal_code'])) {
    return url('http://findvej.dk/' . $location['street'] . ',' . $location['postal_code']);
  }
  elseif (isset($location['street']) && !empty($location['street']) &&
          isset($location['city']) && !empty($location['city'])) {
    return url('http://findvej.dk/' . $location['street'] . ',' . $location['city']);
  }
  elseif (location_has_coordinates($location)) {
    return url('http://findvej.dk/', array('query' => array(
      'latitude' => $location['latitude'],
      'longitude' => $location['longitude'],
    )));
  }
  else {
    return NULL;
  }
}

/**
 * Generate a link to Rejseplanen.
 *
 * Rejseplanen is a route planner for Danish public transportation.
 *
 * @param array $location
 *    The location array we're trying to generate a link for.
 * @return string or null
 *    Map link if generation successful, otherwise null.
 */
function location_map_link_dk_rejseplanen($location = array()) {
  $keys = array();

  // Iterate over all the possible details, and if set, add them to an array.
  foreach (array('street', 'postal_code', 'city') as $name) {
    if (isset($location[$name]) && !empty($location[$name])) {
      $keys[$name] = $location[$name];
    }
  }

  // Merge postal code and city if both are available, since they 
  // shouldn't be comma separated in Danish addresses.
  if (isset($keys['postal_code']) && isset($keys['city'])) {
    $keys['postal_code'] = $keys['postal_code'] . ' ' . $keys['city'];
    unset($keys['city']);
  }

  // If any of the key values were available, generate a URL.
  if (!empty($keys)) {
    return url('http://www.rejseplanen.dk/bin/query.exe/mn?Z=' . implode(', ', $keys) . '&ZADR=1');
  }
  else {
    return NULL;
  }
}

/**
 * @return
 *   An array where
 *     -> the key is the word that helps identify the name of function that builds the link.  For example, a key of 'yahoo' means the name of the
 *        the function that builds a link to a map on Yahoo! Maps would be 'location_map_link_us_yahoo'
 *     -> the value is itself an array with 3 key/value pairs:
 *          'name' => points to the name of the mapping service.  For 'yahoo', this would be 'Yahoo! Maps'
 *          'url' => the url of the main page of the mapping service.  For 'yahoo', this would be 'http://maps.yahoo.com'
 *          'tos' => the url of the page that explains the map providers Terms of Service, or Terms of Use. For 'yahoo', this would be
 *                   'http://help.yahoo.com/help/us/maps/maps-24.html'
 */
function location_map_link_dk_providers() {
  return array(
    'findvej' => array(
      'name' => 'findvej.dk',
      'url' => 'http://findvej.dk',
      'tos' => 'http://www.google.com/intl/da_ALL/help/terms_maps.html',
    ),
    'rejseplanen' => array(
      'name' => 'Rejseplanen',
      'url' => 'http://www.rejseplanen.dk/',
      'tos' => 'http://info.rejseplanen.dk/om_rejseplanen', // As close as I could find.
    ),
  );
}

/**
 * @return
 *   An array of values that work as keys to the array returned by location_map_link_us_providers.  The idea is that if the
 *   administrator of the site has not yet had a chance to visit the "Map Links" subtab on the location module's settings page,
 *   that we can provide deep-linking to a relatively safe default.  By 'relatively safe', we mean that the Terms Of Service of
 *   the provider of the maps are flexible enough for most parties.
 *
 *   For the case of the U.S., 'google' has relatively flexible Terms Of Service, whereas Yahoo! Maps and MapQuest have more
 *   restrictive Terms Of Service.
 *
 */
function location_map_link_dk_default_providers() {
  return array('findvej', 'rejseplanen');
}


/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_dk() {
  return array(
    'minlng' => 8.00875,
    'minlat' => 54.590067,
    'maxlng' => 15.15975,
    'maxlat' => 57.805567,
  );
}
