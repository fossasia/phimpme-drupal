<?php

// Cyprus

function location_province_list_cy() {
  return array('F' => "Famagusta",
    'K' => "Kyrenia",
    'A' => "Larnaca",
    'I' => "Limassol",
    'N' => "Nicosia",
    'P' => "Paphos");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cy() {
  return array(
    'minlng' => 32.196,
    'minlat' => 34.596467,
    'maxlng' => 34.5541,
    'maxlat' => 35.748533,
  );
}
