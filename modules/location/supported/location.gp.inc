<?php

// Guadeloupe

function location_province_list_gp() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gp() {
  return array(
    'minlng' => -61.81885,
    'minlat' => 15.840433,
    'maxlng' => -61.1777,
    'maxlat' => 16.4324,
  );
}
