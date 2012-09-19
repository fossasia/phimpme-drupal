<?php

// Antarctica

function location_province_list_aq() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_aq() {
  return array(
    'minlng' => -179.999427042259,
    'minlat' => -89.9,
    'maxlng' => 179.999427042256,
    'maxlat' => -60.9997330347133,
  );
}
