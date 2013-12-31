<?php

// Montserrat

function location_province_list_ms() {
  return array('A' => "Saint Anthony",
    'G' => "Saint Georges",
    'P' => "Saint Peter");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ms() {
  return array(
    'minlng' => -62.26695,
    'minlat' => 16.659067,
    'maxlng' => -62.20585,
    'maxlat' => 16.738,
  );
}
