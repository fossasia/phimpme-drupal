<?php

// Pitcairn

function location_province_list_pn() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pn() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -130.75307,
    'minlat' => -25.077124,
    'maxlng' => -124.77715,
    'maxlat' => -23.924326,
  );
}
