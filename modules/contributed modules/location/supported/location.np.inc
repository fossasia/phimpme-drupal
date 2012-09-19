<?php

// Nepal

function location_province_list_np() {
  return array('BA' => "Bagmati",
    'BH' => "Bheri",
    'DH' => "Dhawalagiri",
    'GA' => "Gandaki",
    'JA' => "Janakpur",
    'KA' => "Karnali",
    'KO' => "Kosi",
    'LU' => "Lumbini",
    'MA' => "Mahakali",
    'ME' => "Mechi",
    'NA' => "Narayani",
    'RA' => "Rapti",
    'SA' => "Sagarmatha",
    'SE' => "Seti");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_np() {
  return array(
    'minlng' => 80.0464,
    'minlat' => 26.411167,
    'maxlng' => 88.1283,
    'maxlat' => 30.364033,
  );
}
