<?php

// Bermuda

function location_province_list_bm() {
  return array('DS' => "Devonshire",
    'HC' => "Hamilton City",
    'HA' => "Hamilton",
    'PG' => "Paget",
    'PB' => "Pembroke",
    'GC' => "Saint George City",
    'SG' => "Saint George's",
    'SA' => "Sandys",
    'SM' => "Smith's",
    'SH' => "Southampton",
    'WA' => "Warwick");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bm() {
  return array(
    'minlng' => -64.98725,
    'minlat' => 32.189967,
    'maxlng' => -64.6069,
    'maxlat' => 32.406167,
  );
}
