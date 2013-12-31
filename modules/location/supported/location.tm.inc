<?php

// Turkmenistan

function location_province_list_tm() {
  return array('A' => "Ahal Welayaty",
    'B' => "Balkan Welayaty",
    'D' => "Dashhowuz Welayaty",
    'L' => "Lebap Welayaty",
    'M' => "Mary Welayaty");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tm() {
  return array(
    'minlng' => 52.48045,
    'minlat' => 35.1277,
    'maxlng' => 66.558,
    'maxlat' => 42.754,
  );
}
