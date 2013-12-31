<?php

// Falkland Islands (Malvinas)

function location_province_list_fk() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_fk() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -61.317441,
    'minlat' => -52.341765,
    'maxlng' => -57.733603,
    'maxlat' => -51.027736,
  );
}
