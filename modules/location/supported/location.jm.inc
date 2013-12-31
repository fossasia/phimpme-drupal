<?php

// Jamaica

function location_province_list_jm() {
  return array('CLA' => "Clarendon Parish",
    'HAN' => "Hanover Parish",
    'KIN' => "Kingston Parish",
    'MAN' => "Manchester Parish",
    'POR' => "Portland Parish",
    'AND' => "Saint Andrew Parish",
    'ANN' => "Saint Ann Parish",
    'CAT' => "Saint Catherine Parish",
    'ELI' => "Saint Elizabeth Parish",
    'JAM' => "Saint James Parish",
    'MAR' => "Saint Mary Parish",
    'THO' => "Saint Thomas Parish",
    'TRL' => "Trelawny Parish",
    'WML' => "Westmoreland Parish");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_jm() {
  return array(
    'minlng' => -78.4033,
    'minlat' => 16.911867,
    'maxlng' => -75.9029,
    'maxlat' => 18.462967,
  );
}
