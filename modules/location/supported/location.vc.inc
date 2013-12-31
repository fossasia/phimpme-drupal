<?php

// Saint Vincent and the Grenadines

function location_province_list_vc() {
  return array('C' => "Charlotte",
    'R' => "Grenadines",
    'A' => "Saint Andrew",
    'D' => "Saint David",
    'G' => "Saint George",
    'P' => "Saint Patrick");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_vc() {
  return array(
    'minlng' => -61.62325,
    'minlat' => 12.2329,
    'maxlng' => -61.1719,
    'maxlat' => 13.362333,
  );
}
