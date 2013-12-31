<?php

// Kenya

function location_province_list_ke() {
  return array('CE' => "Central",
    'CO' => "Coast",
    'EA' => "Eastern",
    'NA' => "Nairobi Area",
    'NE' => "North Eastern",
    'NY' => "Nyanza",
    'RV' => "Rift Valley",
    'WE' => "Western");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ke() {
  return array(
    'minlng' => 33.7914,
    'minlat' => -4.729033,
    'maxlng' => 42.20675,
    'maxlat' => 4.542067,
  );
}
