<?php

// Botswana

function location_province_list_bw() {
  return array('CE' => "Central",
    'GH' => "Ghanzi",
    'KD' => "Kgalagadi",
    'KT' => "Kgatleng",
    'KW' => "Kweneng",
    'NG' => "Ngamiland",
    'NE' => "North East",
    'NW' => "North West",
    'SE' => "South East",
    'SO' => "Southern");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bw() {
  return array(
    'minlng' => 19.9836,
    'minlat' => -26.8509,
    'maxlng' => 29.2724,
    'maxlat' => -17.837133,
  );
}
