<?php

// Greenland

function location_province_list_gl() {
  return array('A' => "Avannaa",
    'T' => "Tunu",
    'K' => "Kitaa");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gl() {
  return array(
    'minlng' => -73.39145,
    'minlat' => 59.7644,
    'maxlng' => -11.72105,
    'maxlat' => 83.674733,
  );
}
