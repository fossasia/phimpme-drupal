<?php

// Virgin Islands, British

function location_province_list_vg() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_vg() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -64.774000,
    'minlat' => 18.3346959,
    'maxlng' => -64.269999,
    'maxlat' => 18.7460403,
  );
}
