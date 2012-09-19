<?php

// Haiti

function location_province_list_ht() {
  return array('AR' => "Artibonite",
    'CE' => "Centre",
    'GA' => "Grand'Anse",
    'ND' => "Nord",
    'NE' => "Nord-Est",
    'NO' => "Nord-Ouest",
    'OU' => "Ouest",
    'SD' => "Sud",
    'SE' => "Sud-Est");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ht() {
  return array(
    'minlng' => -74.49565,
    'minlat' => 18.0219,
    'maxlng' => -71.62015,
    'maxlat' => 20.107033,
  );
}
