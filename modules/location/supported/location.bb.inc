<?php

// Barbados

function location_province_list_bb() {
  return array('CC' => "Christ Church",
    'AND' => "Saint Andrew",
    'GEO' => "Saint George",
    'JAM' => "Saint James",
    'JOH' => "Saint John",
    'JOS' => "Saint Joseph",
    'LUC' => "Saint Lucy",
    'MIC' => "Saint Michael",
    'PET' => "Saint Peter",
    'PHI' => "Saint Philip",
    'THO' => "Saint Thomas");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bb() {
  return array(
    'minlng' => -59.6715,
    'minlat' => 13.018433,
    'maxlng' => -59.3994,
    'maxlat' => 13.285133,
  );
}
