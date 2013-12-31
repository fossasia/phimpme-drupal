<?php

// Vanuatu

function location_province_list_vu() {
  return array('MA' => "Malampa",
    'PE' => "Penama",
    'SA' => "Sanma",
    'SH' => "Shefa",
    'TA' => "Tafea",
    'TO' => "Torba");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_vu() {
  return array(
    'minlng' => 166.55505,
    'minlat' => -20.260633,
    'maxlng' => 170.30605,
    'maxlat' => -13.112333,
  );
}
