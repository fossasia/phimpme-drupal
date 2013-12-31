<?php

// New Caledonia

function location_province_list_nc() {
  return array('L' => "Iles Loyaute",
    'N' => "Nord",
    'S' => "Sud");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_nc() {
  return array(
    'minlng' => 159.8613,
    'minlat' => -22.9837,
    'maxlng' => 168.914,
    'maxlat' => -18.4219,
  );
}
