<?php

// Western Sahara

function location_province_list_eh() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_eh() {
  return array(
    'minlng' => -17.08725,
    'minlat' => 20.761433,
    'maxlng' => -8.6836,
    'maxlat' => 27.654567,
  );
}
