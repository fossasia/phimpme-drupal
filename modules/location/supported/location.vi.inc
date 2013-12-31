<?php

// Virgin Islands, U. S.

function location_province_list_vi() {
  return array(
    'C' => "Saint Croix",
    'J' => "Saint John",
    'T' => "Saint Thomas",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_vi() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -65.020315,
    'minlat' => 17.6825393,
    'maxlng' => -64.558664,
    'maxlat' => 18.3888528,
  );
}
