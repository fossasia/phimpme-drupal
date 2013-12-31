<?php

// Solomon Islands

function location_province_list_sb() {
  return array('CE' => "Central",
    'CH' => "Choiseul",
    'GC' => "Guadalcanal",
    'HO' => "Honiara",
    'IS' => "Isabel",
    'MK' => "Makira",
    'ML' => "Malaita",
    'RB' => "Rennell and Bellona",
    'TM' => "Temotu",
    'WE' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sb() {
  return array(
    'minlng' => 155.4885,
    'minlat' => -12.2589,
    'maxlng' => 170.23905,
    'maxlat' => -5.210433,
  );
}
