<?php

// Montenegro

function location_province_list_me() {
  return array(
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_me() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 18.4335307,
    'minlat' => 41.8520718,
    'maxlng' => 20.3551705,
    'maxlat' => 43.5478856,
  );
}
