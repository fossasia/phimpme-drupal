<?php

// Svalbard and Jan Mayen

function location_province_list_sj() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sj() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:4326
  // Note: I'm using the bounding box for Norway here, because it includes
  // both Svalbard and Jan Mayen.
  return array(
    'minlng' => -9.1168289,
    'minlat' => -54.462359,
    'maxlng' => 33.6410262,
    'maxlat' => 80.7697480,
  );
}
