<?php

// Serbia

function location_province_list_rs() {
  return array(
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_rs() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 18.8448339,
    'minlat' => 42.2349530,
    'maxlng' => 22.9844779,
    'maxlat' => 46.1740665,
  );
}
