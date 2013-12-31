<?php

// French Guiana

function location_province_list_gf() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gf() {
  return array(
    'minlng' => -54.54515,
    'minlat' => 1.999333,
    'maxlng' => -51.7183,
    'maxlat' => 5.779333,
  );
}
