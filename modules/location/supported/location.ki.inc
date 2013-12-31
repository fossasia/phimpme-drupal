<?php

// Kiribati

function location_province_list_ki() {
  return array('AG' => "Abaiang",
    'AM' => "Abemama",
    'AK' => "Aranuka",
    'AO' => "Arorae",
    'BA' => "Banaba",
    'BE' => "Beru",
    'bT' => "Butaritari",
    'KA' => "Kanton",
    'KR' => "Kiritimati",
    'KU' => "Kuria",
    'MI' => "Maiana",
    'MN' => "Makin",
    'ME' => "Marakei",
    'NI' => "Nikunau",
    'NO' => "Nonouti",
    'ON' => "Onotoa",
    'TT' => "Tabiteuea",
    'TR' => "Tabuaeran",
    'TM' => "Tamana",
    'TW' => "Tarawa",
    'TE' => "Teraina");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ki() {
  return array(
    'minlng' => -175.8883,
    'minlat' => -11.523967,
    'maxlng' => 176.69935,
    'maxlat' => 4.8706,
  );
}
