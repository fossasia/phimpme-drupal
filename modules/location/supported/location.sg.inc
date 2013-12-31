<?php

// Singapore

function location_province_list_sg() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sg() {
  return array(
    'minlng' => 103.6183,
    'minlat' => 1.3645,
    'maxlng' => 103.88895,
    'maxlat' => 1.4793,
  );
}
