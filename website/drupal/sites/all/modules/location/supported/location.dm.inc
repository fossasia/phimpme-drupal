<?php

// Dominica

function location_province_list_dm() {
  return array('AND' => "Saint Andrew Parish",
    'DAV' => "Saint David Parish",
    'GEO' => "Saint George Parish",
    'JOH' => "Saint John Parish",
    'JOS' => "Saint Joseph Parish",
    'LUK' => "Saint Luke Parish",
    'MAR' => "Saint Mark Parish",
    'PAT' => "Saint Patrick Parish",
    'PAU' => "Saint Paul Parish",
    'PET' => "Saint Peter Parish");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_dm() {
  return array(
    'minlng' => -61.5093,
    'minlat' => 15.237433,
    'maxlng' => -61.20985,
    'maxlat' => 15.627967,
  );
}
