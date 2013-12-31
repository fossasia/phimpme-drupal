<?php

// Palestinian Territory, Occupied

function location_province_list_ps() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ps() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 34.8671529,
    'minlat' => 31.3449095,
    'maxlng' => 35.5725362,
    'maxlat' => 32.5426400,
  );
}
