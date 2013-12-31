<?php

// Uzbekistan

function location_province_list_uz() {
  return array('AN' => "Andijon",
    'BU' => "Buxoro",
    'FA' => "Farg'ona",
    'JI' => "Jizzax",
    'NG' => "Namangan",
    'NW' => "Navoiy",
    'QA' => "Qashqadaryo",
    'QR' => "Qoraqalpog'iston Republikasi",
    'SA' => "Samarqand",
    'SI' => "Sirdaryo",
    'SU' => "Surxondaryo",
    'TK' => "Toshkent city",
    'TO' => "Toshkent region",
    'XO' => "Xorazm");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_uz() {
  return array(
    'minlng' => 55.975,
    'minlat' => 37.1818,
    'maxlng' => 73.0411,
    'maxlat' => 45.6219,
  );
}
