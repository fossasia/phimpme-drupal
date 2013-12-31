<?php

// Gibraltar

function location_province_list_gi() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gi() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -5.3851187,
    'minlat' => 36.1111819,
    'maxlng' => -5.3587637,
    'maxlat' => 36.1411025,
  );
}
