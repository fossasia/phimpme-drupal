<?php

// South Georgia and the South Sandwich Islands

function location_province_list_gs() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gs() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -42.118768,
    'minlat' => -59.472802,
    'maxlng' => -26.238677,
    'maxlat' => -53.456787,
  );
}
