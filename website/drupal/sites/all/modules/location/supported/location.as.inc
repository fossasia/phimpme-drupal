<?php

// American Samoa

function location_province_list_as() {
  return array('E' => "Eastern",
    'M' => "Manu'a",
    'R' => "Rose Island",
    'S' => "Swains Island",
    'W' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_as() {
  return array(
    'minlng' => -170.9021,
    'minlat' => -14.541433,
    'maxlng' => -168.23395,
    'maxlat' => -14.183633,
  );
}
