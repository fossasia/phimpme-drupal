<?php

// Swaziland

function location_province_list_sz() {
  return array('H' => "Hhohho",
    'L' => "Lubombo",
    'M' => "Manzini",
    'S' => "Shishelweni");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sz() {
  return array(
    'minlng' => 30.7093,
    'minlat' => -27.274367,
    'maxlng' => 32.2211,
    'maxlat' => -25.720333,
  );
}
