<?php

// Bahrain

function location_province_list_bh() {
  return array('CAP' => "Capital",
    'CEN' => "Central",
    'MUH' => "Muharraq",
    'NOR' => "Northern",
    'SOU' => "Southern");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bh() {
  return array(
    'minlng' => 50.402,
    'minlat' => 25.862667,
    'maxlng' => 50.66065,
    'maxlat' => 26.339733,
  );
}
