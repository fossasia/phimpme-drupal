<?php

// Guernsey
// See http://www.statoids.com/ugg.html

function location_province_list_gg() {
  return array(
    'AL' => 'Alderney',
    'BQ' => 'Brecqhou',
    'CA' => 'Castel',
    'FO' => 'Forest',
    'HM' => 'Herm',
    'JT' => 'Jethou',
    'LH' => 'Lihou',
    'AN' => 'Saint Andrew',
    'MA' => 'Saint Martin',
    'PP' => 'Saint Peter Port',
    'PB' => 'Saint Pierre du Bois',
    'SM' => 'Saint Sampson',
    'SV' => 'Saint Saviour',
    'SK' => 'Sark',
    'TV' => 'Torteval',
    'VA' => 'Vale',
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gg() {
  return array(
    'minlng' => -2.77195,
    'minlat' => 49.381867,
    'maxlng' => -2.3856,
    'maxlat' => 49.4828,
  );
}
