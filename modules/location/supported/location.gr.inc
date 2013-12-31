<?php

// Greece

function location_province_list_gr() {
  return array('AT' => "Attica",
    'CN' => "Central Greece",
    'CM' => "Central Macedonia",
    'CR' => "Crete",
    'EM' => "East Macedonia and Thrace",
    'EP' => "Epirus",
    'II' => "Ionian Islands",
    'NA' => "North Aegean",
    'PP' => "Peloponnesos",
    'SA' => "South Aegean",
    'TH' => "Thessaly",
    'WG' => "West Greece",
    'WM' => "West Macedonia");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gr() {
  return array(
    'minlng' => 19.65595,
    'minlat' => 34.851,
    'maxlng' => 28.16225,
    'maxlat' => 41.885033,
  );
}
