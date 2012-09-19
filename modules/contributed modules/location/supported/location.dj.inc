<?php

// Djibouti

function location_province_list_dj() {
  return array('S' => "'Ali Sabih",
    'K' => "Dikhil",
    'J' => "Djibouti",
    'O' => "Obock",
    'T' => "Tadjoura");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_dj() {
  return array(
    'minlng' => 41.7834,
    'minlat' => 10.874833,
    'maxlng' => 43.4419,
    'maxlat' => 12.718233,
  );
}
