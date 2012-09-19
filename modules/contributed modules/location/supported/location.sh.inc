<?php

// Saint Helena

function location_province_list_sh() {
  return array('A' => "Ascension",
    'S' => "Saint Helena",
    'T' => "Tristan da Cunha");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sh() {
  return array(
    'minlng' => -5.9486,
    'minlat' => -16.042567,
    'maxlng' => -5.7812,
    'maxlat' => -15.896867,
  );
}
