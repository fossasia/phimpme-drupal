<?php

// Senegal

function location_province_list_sn() {
  return array('DA' => "Dakar",
    'DI' => "Diourbel",
    'FA' => "Fatick",
    'KA' => "Kaolack",
    'KO' => "Kolda",
    'LO' => "Louga",
    'MA' => "Matam",
    'SL' => "Saint-Louis",
    'TA' => "Tambacounda",
    'TH' => "Thies",
    'ZI' => "Ziguinchor");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sn() {
  return array(
    'minlng' => -17.7009,
    'minlat' => 12.283667,
    'maxlng' => -11.4326,
    'maxlat' => 16.624867,
  );
}
