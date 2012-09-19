<?php

// Sao Tome and Principe

function location_province_list_st() {
  return array('S' => "Sao Tome",
    'P' => "Principe");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_st() {
  return array(
    'minlng' => 6.3808,
    'minlat' => 0.075233,
    'maxlng' => 7.44865,
    'maxlat' => 1.704133,
  );
}
