<?php

// Aland Islands

function location_province_list_ax() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ax() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 19.5137748,
    'minlat' => 59.9044583,
    'maxlng' => 21.0966239,
    'maxlat' => 60.4807538,
  );
}
