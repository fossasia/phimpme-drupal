<?php

// Heard Island and McDonald Islands

function location_province_list_hm() {
  return array(
    'F' => "Flat Island",
    'M' => "McDonald Island",
    'S' => "Shag Island",
    'H' => "Heard Island",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_hm() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 73.2363871,
    'minlat' => -53.192669,
    'maxlng' => 73.8452388,
    'maxlat' => -52.961572,
  );
}
