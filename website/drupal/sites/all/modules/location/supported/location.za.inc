<?php

// South Africa

function location_province_list_za() {
  return array('WC' => "Western Cape",
    'NC' => "Northern Cape",
    'EC' => "Eastern Cape",
    'KN' => "KwaZulu-Natal",
    'FS' => "Free State",
    'NW' => "North West",
    'GT' => "Gauteng",
    'MP' => "Mpumalanga",
    'LP' => "Limpopo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_za() {
  return array(
    'minlng' => 16.50735,
    'minlat' => -34.7871,
    'maxlng' => 32.80535,
    'maxlat' => -22.152867,
  );
}
