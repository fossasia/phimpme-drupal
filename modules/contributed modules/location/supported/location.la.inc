<?php

// Lao People's Democratic Republic

function location_province_list_la() {
  return array(
    'VT' => "Vientiane",
    'AT' => "Attapu",
    'BK' => "Bokeo",
    'BL' => "Bolikhamxai",
    'CH' => "Champasak",
    'HO' => "Houaphan",
    'KH' => "Khammouan",
    'LM' => "Louang Namtha",
    'LP' => "Louangphabang",
    'OU' => "Oudomxai",
    'PH' => "Phongsali",
    'SL' => "Salavan",
    'SV' => "Savannakhet",
    'VI' => "Vientiane",
    'XA' => "Xaignabouli",
    'XE' => "Xekong",
    'XI' => "Xiangkhoang",
    'XN' => "Xaisomboun",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_la() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 100.097073,
    'minlat' => 13.9154566,
    'maxlng' => 107.664363,
    'maxlat' => 22.4960440,
  );
}
