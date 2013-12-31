<?php

// Jersey
// See http://www.statoids.com/uje.html

function location_province_list_je() {
  return array(
    'GR' => 'Grouville',
    'BR' => 'Saint Brelade',
    'CL' => 'Saint Clement',
    'HE' => 'Saint Helier',
    'JO' => 'Saint John',
    'LA' => 'Saint Lawrence',
    'MT' => 'Saint Martin',
    'MY' => 'Saint Mary',
    'OU' => 'Saint Ouen',
    'PE' => 'Saint Peter',
    'SA' => 'Saint Saviour',
    'TR' => 'Trinity',
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_je() {
  return array(
    'minlng' => -2.40735,
    'minlat' => 49.181767,
    'maxlng' => -2.15155,
    'maxlat' => 49.2844,
  );
}
