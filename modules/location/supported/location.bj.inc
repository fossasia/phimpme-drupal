<?php

// Benin

function location_province_list_bj() {
  return array('AL' => "Alibori",
    'AK' => "Atakora",
    'AQ' => "Atlantique",
    'BO' => "Borgou",
    'CO' => "Collines",
    'DO' => "Donga",
    'KO' => "Kouffo",
    'LI' => "Littoral",
    'MO' => "Mono",
    'OU' => "Oueme",
    'PL' => "Plateau",
    'ZO' => "Zou");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bj() {
  return array(
    'minlng' => 0.7266,
    'minlat' => 6.189467,
    'maxlng' => 3.81025,
    'maxlat' => 12.340233,
  );
}
