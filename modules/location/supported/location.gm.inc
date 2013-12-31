<?php

// Gambia

function location_province_list_gm() {
  return array('BJ' => "Banjul",
    'BS' => "Basse",
    'BR' => "Brikama",
    'JA' => "Janjangbure",
    'KA' => "Kanifeng",
    'KE' => "Kerewan",
    'KU' => "Kuntaur",
    'MA' => "Mansakonko",
    'LR' => "Lower River",
    'CR' => "Central River",
    'NB' => "North Bank",
    'UR' => "Upper River",
    'WE' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gm() {
  return array(
    'minlng' => -16.79745,
    'minlat' => 13.105767,
    'maxlng' => -13.8271,
    'maxlat' => 13.7983,
  );
}
