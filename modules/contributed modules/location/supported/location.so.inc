<?php

// Somalia

function location_province_list_so() {
  return array('AW' => "Awdal",
    'BK' => "Bakool",
    'BN' => "Banaadir",
    'BR' => "Bari",
    'BY' => "Bay",
    'GA' => "Galguduud",
    'GE' => "Gedo",
    'HI' => "Hiiraan",
    'JD' => "Jubbada Dhexe",
    'JH' => "Jubbada Hoose",
    'MU' => "Mudug",
    'NU' => "Nugaal",
    'SA' => "Sanaag",
    'SD' => "Shabeellaha Dhexe",
    'SH' => "Shabeellaha Hoose",
    'SL' => "Sool",
    'TO' => "Togdheer",
    'WG' => "Woqooyi Galbeed");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_so() {
  return array(
    'minlng' => 40.9763,
    'minlat' => -1.640967,
    'maxlng' => 51.41685,
    'maxlat' => 11.9692,
  );
}
