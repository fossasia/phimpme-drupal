<?php

// Reunion

function location_province_list_re() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_re() {
  return array(
    'minlng' => 55.2646,
    'minlat' => -21.451133,
    'maxlng' => 55.8696,
    'maxlat' => -20.883,
  );
}
