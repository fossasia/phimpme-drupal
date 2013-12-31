<?php

// Myanmar

function location_province_list_mm() {
  return array('AY' => "Ayeyarwady",
    'BG' => "Bago",
    'MG' => "Magway",
    'MD' => "Mandalay",
    'SG' => "Sagaing",
    'TN' => "Tanintharyi",
    'YG' => "Yangon",
    'CH' => "Chin State",
    'KC' => "Kachin State",
    'KH' => "Kayah State",
    'KN' => "Kayin State",
    'MN' => "Mon State",
    'RK' => "Rakhine State",
    'SH' => "Shan State");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mm() {
  return array(
    'minlng' => 92.1068,
    'minlat' => 9.755067,
    'maxlng' => 101.048,
    'maxlat' => 28.184067,
  );
}
