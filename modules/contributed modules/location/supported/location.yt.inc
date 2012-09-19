<?php

// Mayotte

function location_province_list_yt() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_yt() {
  return array(
    'minlng' => 45.0551,
    'minlat' => -13.045133,
    'maxlng' => 45.32135,
    'maxlat' => -12.6251,
  );
}
