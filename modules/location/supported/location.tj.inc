<?php

// Tajikistan

function location_province_list_tj() {
  return array('GB' => "Gorno-Badakhstan",
    'KT' => "Khatlon",
    'SU' => "Sughd");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tj() {
  return array(
    'minlng' => 67.42345,
    'minlat' => 36.7487,
    'maxlng' => 75.1309,
    'maxlat' => 41.066867,
  );
}
