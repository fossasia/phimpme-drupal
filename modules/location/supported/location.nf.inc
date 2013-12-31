<?php

// Norfolk Island

function location_province_list_nf() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_nf() {
  return array(
    'minlng' => 167.7933,
    'minlat' => -29.217267,
    'maxlng' => 167.94745,
    'maxlat' => -29.0449,
  );
}
