<?php

// Martinique

function location_province_list_mq() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mq() {
  return array(
    'minlng' => -61.241,
    'minlat' => 14.391567,
    'maxlng' => -60.7837,
    'maxlat' => 14.8491,
  );
}
