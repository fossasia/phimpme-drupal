<?php

// Cocos (Keeling) Islands

function location_province_list_cc() {
  return array('D' => "Direction Island",
    'H' => "Home Island",
    'O' => "Horsburgh Island",
    'S' => "South Island",
    'W' => "West Island");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cc() {
  return array(
    'minlng' => 95.2723,
    'minlat' => -12.2493,
    'maxlng' => 96.8062,
    'maxlat' => -11.742167,
  );
}
