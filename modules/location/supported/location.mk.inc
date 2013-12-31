<?php

// Macedonia, The Former Yugoslav Republic of

function location_province_list_mk() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mk() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 20.4441573,
    'minlat' => 40.8493940,
    'maxlng' => 23.0095821,
    'maxlat' => 42.3703347,
  );
}
