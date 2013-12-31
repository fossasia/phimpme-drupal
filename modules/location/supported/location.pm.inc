<?php

// Saint Pierre and Miquelon

function location_province_list_pm() {
  return array('P' => "Saint Pierre",
    'M' => "Miquelon");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pm() {
  return array(
    'minlng' => -56.41835,
    'minlat' => 46.7927,
    'maxlng' => -56.1141,
    'maxlat' => 47.203467,
  );
}
