<?php

// New Zealand

function location_province_list_nz() {
  return array('AUK' => "Auckland",
    'BOP' => "Bay of Plenty",
    'CAN' => "Canterbury",
    'GIS' => "Gisborne",
    'HKB' => "Hawke's Bay",
    'MBH' => "Marlborough",
    'MWT' => "Manawatu-Wanganui",
    'NSN' => "Nelson",
    'NTL' => "Northland",
    'OTA' => "Otago",
    'STL' => "Southland",
    'TAS' => "Tasman",
    'TKI' => "Taranaki",
    'WGN' => "Wellington",
    'WKO' => "Waikato",
    'WTC' => "West Coast");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_nz() {
  return array(
    'minlng' => -178.94835,
    'minlat' => -57.294267,
    'maxlng' => 179.22415,
    'maxlat' => -8.536767,
  );
}
