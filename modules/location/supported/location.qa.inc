<?php

// Qatar

function location_province_list_qa() {
  return array('DW' => "Ad Dawhah",
    'GW' => "Al Ghuwayriyah",
    'JM' => "Al Jumayliyah",
    'KR' => "Al Khawr",
    'WK' => "Al Wakrah",
    'RN' => "Ar Rayyan",
    'JB' => "Jarayan al Batinah",
    'MS' => "Madinat ash Shamal",
    'UD' => "Umm Sa'id",
    'UL' => "Umm Salal");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_qa() {
  return array(
    'minlng' => 50.70915,
    'minlat' => 24.6815,
    'maxlng' => 51.74715,
    'maxlat' => 26.170267,
  );
}
