<?php

// Sri Lanka

function location_province_list_lk() {
  return array('CE' => "Central",
    'EA' => "Eastern",
    'NC' => "North Central",
    'NO' => "Northern",
    'NW' => "North Western",
    'SA' => "Sabaragamuwa",
    'SO' => "Southern",
    'UV' => "Uva",
    'WE' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lk() {
  return array(
    'minlng' => 79.5574,
    'minlat' => 6.0023,
    'maxlng' => 81.89185,
    'maxlat' => 9.856367,
  );
}
