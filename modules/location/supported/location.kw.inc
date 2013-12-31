<?php

// Kuwait

function location_province_list_kw() {
  return array('D' => "Al Ahmadi",
    'F' => "Al Farwaniyah",
    'S' => "Al Asimah",
    'J' => "Al Jahra",
    'H' => "Hawalli");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kw() {
  return array(
    'minlng' => 46.49765,
    'minlat' => 28.557733,
    'maxlng' => 48.4888,
    'maxlat' => 30.140667,
  );
}
