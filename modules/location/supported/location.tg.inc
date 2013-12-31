<?php

// Togo

function location_province_list_tg() {
  return array('K' => "Kara",
    'P' => "Plateaux",
    'S' => "Savanes",
    'C' => "Centrale",
    'M' => "Maritime");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tg() {
  return array(
    'minlng' => -0.0933,
    'minlat' => 6.086033,
    'maxlng' => 1.7728,
    'maxlat' => 11.124,
  );
}
