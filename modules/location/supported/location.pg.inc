<?php

// Papua New Guinea

function location_province_list_pg() {
  return array('BV' => "Bougainville",
    'CE' => "Central",
    'CH' => "Chimbu",
    'EH' => "Eastern Highlands",
    'EB' => "East New Britain",
    'ES' => "East Sepik",
    'EN' => "Enga",
    'GU' => "Gulf",
    'MD' => "Madang",
    'MN' => "Manus",
    'MB' => "Milne Bay",
    'MR' => "Morobe",
    'NC' => "National Capital",
    'NI' => "New Ireland",
    'NO' => "Northern",
    'SA' => "Sandaun",
    'SH' => "Southern Highlands",
    'WE' => "Western",
    'WH' => "Western Highlands",
    'WB' => "West New Britain");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pg() {
  return array(
    'minlng' => 140.8018,
    'minlat' => -11.691233,
    'maxlng' => 159.50015,
    'maxlat' => -0.784233,
  );
}
