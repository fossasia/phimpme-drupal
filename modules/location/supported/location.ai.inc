<?php

// Anguilla

function location_province_list_ai() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ai() {
  return array(
    'minlng' => -63.1879,
    'minlat' => 18.137033,
    'maxlng' => -62.9537,
    'maxlat' => 18.248633,
  );
}
