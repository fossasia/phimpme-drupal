<?php

// Panama

function location_province_list_pa() {
  return array('BT' => "Bocas del Toro",
    'CH' => "Chiriqui",
    'CC' => "Cocle",
    'CL' => "Colon",
    'DA' => "Darien",
    'HE' => "Herrera",
    'LS' => "Los Santos",
    'PA' => "Panama",
    'SB' => "San Blas",
    'VG' => "Veraguas");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pa() {
  return array(
    'minlng' => -83.0577,
    'minlat' => 7.1904,
    'maxlng' => -77.17625,
    'maxlat' => 9.6265,
  );
}
