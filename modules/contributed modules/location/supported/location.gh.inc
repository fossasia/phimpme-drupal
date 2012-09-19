<?php

// Ghana

function location_province_list_gh() {
  return array('AS' => "Ashanti Region",
    'BA' => "Brong-Ahafo Region",
    'CE' => "Central Region",
    'EA' => "Eastern Region",
    'GA' => "Greater Accra Region",
    'NO' => "Northern Region",
    'UE' => "Upper East Region",
    'UW' => "Upper West Region",
    'VO' => "Volta Region",
    'WE' => "Western Region");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gh() {
  return array(
    'minlng' => -3.3352,
    'minlat' => 4.678367,
    'maxlng' => 1.25875,
    'maxlat' => 11.169167,
  );
}
