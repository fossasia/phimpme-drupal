<?php

// Iraq

function location_province_list_iq() {
  return array('BD' => "Baghdad",
    'SD' => "Salah ad Din",
    'DY' => "Diyala",
    'WS' => "Wasit",
    'MY' => "Maysan",
    'BA' => "Al Basrah",
    'DQ' => "Dhi Qar",
    'MU' => "Al Muthanna",
    'QA' => "Al Qadisyah",
    'BB' => "Babil",
    'KB' => "Al Karbala",
    'NJ' => "An Najaf",
    'AB' => "Al Anbar",
    'NN' => "Ninawa",
    'DH' => "Dahuk",
    'AL' => "Arbil",
    'TM' => "At Ta'mim",
    'SL' => "As Sulaymaniyah");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_iq() {
  return array(
    'minlng' => 38.63745,
    'minlat' => 29.119667,
    'maxlng' => 48.5924,
    'maxlat' => 37.4467,
  );
}
