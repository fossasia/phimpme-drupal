<?php

// Equatorial Guinea

function location_province_list_gq() {
  return array('AN' => "Provincia Annobon",
    'BN' => "Provincia Bioko Norte",
    'BS' => "Provincia Bioko Sur",
    'CS' => "Provincia Centro Sur",
    'KN' => "Provincia Kie-Ntem",
    'LI' => "Provincia Litoral",
    'WN' => "Provincia Wele-Nzas");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gq() {
  return array(
    'minlng' => 8.34785,
    'minlat' => 0.824267,
    'maxlng' => 11.456,
    'maxlat' => 3.7184,
  );
}
