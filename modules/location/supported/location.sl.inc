<?php

// Sierra Leone

function location_province_list_sl() {
  return array('E' => "Eastern",
    'N' => "Northern",
    'S' => "Southern",
    'W' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sl() {
  return array(
    'minlng' => -13.39655,
    'minlat' => 6.905,
    'maxlng' => -10.2617,
    'maxlat' => 10.008,
  );
}
