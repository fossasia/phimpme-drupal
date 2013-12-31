<?php

// Guinea-Bissau

function location_province_list_gw() {
  return array('BF' => "Bafata Region",
    'BB' => "Biombo Region",
    'BS' => "Bissau Region",
    'BL' => "Bolama Region",
    'CA' => "Cacheu Region",
    'GA' => "Gabu Region",
    'OI' => "Oio Region",
    'QU' => "Quinara Region",
    'TO' => "Tombali Region");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gw() {
  return array(
    'minlng' => -16.92935,
    'minlat' => 10.839167,
    'maxlng' => -13.6464,
    'maxlat' => 12.672033,
  );
}
