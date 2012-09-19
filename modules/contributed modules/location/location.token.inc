<?php

/**
 * @file
 * Short description.
 */


/**
 * Implementation of hook_token_values().
 */
function _location_token_values($type, $object = NULL) {
  if ($type == 'node' || $type == 'user') {
    if (!empty($object->locations)) {
      // @@@ Strip virtual fields?
      foreach ($object->locations as $key => $location) {

        // Normally, location_load_location() will take care of this.
        // However, token is commonly used during *saving*.
        // See #322328.
        if (empty($location['country_name'])) {
          if (!empty($location['country'])) {
            $location['country_name'] = location_country_name($location['country']);
            if (!empty($location['province'])) {
              $location['province_name'] = location_province_name($location['country'], $location['province']);
            }
          }
        }

        foreach ($location as $field => $value) {
          if (!is_array($value)) {
            $values["location-{$field}_{$key}"] = check_plain($value);
          }
        }
        // Backwards compatibility.
        $values["location-provincename_$key"] = $values["location-province_name_$key"];
        $values["location-countryname_$key"] = $values["location-country_name_$key"];
      }
      return $values;
    }
  }
}

/**
 * Implementation of hook_token_list().
 */
function _location_token_list($type = 'all') {
  if ($type == 'node' || $type == 'user' || $type == 'all') {
    $tokens['location']['location-name_N']            = t('Location Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-street_N']          = t('Street (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-additional_N']      = t('Additional (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-city_N']            = t('City (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-province_N']        = t('State/Province (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-province_name_N']   = t('State/Province Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-postal_code_N']     = t('Postal Code (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-latitude_N']        = t('Latitude (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-longitude_N']       = t('Longitude (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-country_N']         = t('Country (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-country_name_N']    = t('Country Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    return $tokens;
  }
}
