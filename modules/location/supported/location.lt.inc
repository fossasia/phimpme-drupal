<?php

// Lithuania

function location_province_list_lt() {
  return array('AL' => "Alytus",
    'KA' => "Kaunas",
    'KL' => "Klaipeda",
    'MA' => "Marijampole",
    'PA' => "Panevezys",
    'SI' => "Siauliai",
    'TA' => "Taurage",
    'TE' => "Telsiai",
    'UT' => "Utena",
    'VI' => "Vilnius");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lt() {
  return array(
    'minlng' => 20.76725,
    'minlat' => 53.949833,
    'maxlng' => 26.86295,
    'maxlat' => 56.478033,
  );
}
