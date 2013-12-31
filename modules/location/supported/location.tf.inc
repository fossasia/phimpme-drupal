<?php

// French Southern Territories

function location_province_list_tf() {
  return array('C' => "Iles Crozet",
    'K' => "Iles Kerguelen",
    'A' => "Ile Amsterdam",
    'P' => "Ile Saint-Paul",
    'D' => "Adelie Land");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tf() {
  return array(
    'minlng' => 50.0717,
    'minlat' => -49.642933,
    'maxlng' => 70.5273,
    'maxlat' => -45.8805,
  );
}
