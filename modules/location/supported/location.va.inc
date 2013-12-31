<?php

// Holy See (Vatican City State)

function location_province_list_va() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_va() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 12.4443363,
    'minlat' => 41.9001817,
    'maxlng' => 12.4583923,
    'maxlat' => 41.9084500,
  );
}
