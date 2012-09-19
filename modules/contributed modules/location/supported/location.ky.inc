<?php

// Cayman Islands

function location_province_list_ky() {
  return array('CR' => "Creek",
    'EA' => "Eastern",
    'ML' => "Midland",
    'ST' => "South Town",
    'SP' => "Spot Bay",
    'SK' => "Stake Bay",
    'WD' => "West End",
    'WN' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ky() {
  return array(
    'minlng' => -81.4223,
    'minlat' => 19.2398,
    'maxlng' => -79.75185,
    'maxlat' => 19.7329,
  );
}
