<?php

// Antigua and Barbuda

function location_province_list_ag() {
  return array('ASG' => "Saint George",
    'ASJ' => "Saint John",
    'ASM' => "Saint Mary",
    'ASL' => "Saint Paul",
    'ASR' => "Saint Peter",
    'ASH' => "Saint Philip",
    'BAR' => "Barbuda",
    'RED' => "Redonda");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ag() {
  return array(
    'minlng' => -61.89365,
    'minlat' => 16.9478,
    'maxlng' => -61.64325,
    'maxlat' => 17.735533,
  );
}
