<?php

// Pakistan

function location_province_list_pk() {
  return array('B' => "Balochistan",
    'T' => "Federally Administered Tribal Areas",
    'I' => "Islamabad Capital Territory",
    'N' => "North-West Frontier Province",
    'P' => "Punjab",
    'S' => "Sindh");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pk() {
  return array(
    'minlng' => 60.9339,
    'minlat' => 23.683133,
    'maxlng' => 75.52705,
    'maxlat' => 36.9695,
  );
}
