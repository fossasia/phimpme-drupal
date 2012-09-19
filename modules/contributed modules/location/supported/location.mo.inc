<?php

// Macau

function location_province_list_mo() {
  return array(
    'OLF' => "Our Lady Fatima Parish",
    'ANT' => "St. Anthony Parish",
    'LAZ' => "St. Lazarus Parish",
    'CAT' => "Cathedral Parish",
    'LAW' => "St. Lawrence Parish",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mo() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 113.521060,
    'minlat' => 22.1053701,
    'maxlng' => 113.587515,
    'maxlat' => 22.2176630,
  );
}
