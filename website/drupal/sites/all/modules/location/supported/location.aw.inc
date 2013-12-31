<?php

// Aruba

function location_province_list_aw() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_aw() {
  return array(
    'minlng' => -70.08745,
    'minlat' => 12.4276,
    'maxlng' => -69.83805,
    'maxlat' => 12.647367,
  );
}
