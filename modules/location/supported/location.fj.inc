<?php

// Fiji

function location_province_list_fj() {
  return array('C' => "Central Division",
    'N' => "Northern Division",
    'E' => "Eastern Division",
    'W' => "Western Division",
    'R' => "Rotuma");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_fj() {
  return array(
    'minlng' => -180,
    'minlat' => -21.808767,
    'maxlng' => 180,
    'maxlat' => -15.7846,
  );
}
