<?php

// Puerto Rico

function location_province_list_pr() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pr() {
  return array(
    'minlng' => -67.2807,
    'minlat' => 17.913933,
    'maxlng' => -65.38815,
    'maxlat' => 18.459233,
  );
}
