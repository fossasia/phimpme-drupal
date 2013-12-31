<?php

// Zambia

function location_province_list_zm() {
  return array('CE' => "Central Province",
    'CB' => "Copperbelt Province",
    'EA' => "Eastern Province",
    'LP' => "Luapula Province",
    'LK' => "Lusaka Province",
    'NO' => "Northern Province",
    'NW' => "North-Western Province",
    'SO' => "Southern Province",
    'WE' => "Western Province");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_zm() {
  return array(
    'minlng' => 21.92875,
    'minlat' => -18.0931,
    'maxlng' => 33.53855,
    'maxlat' => -8.221467,
  );
}
