<?php

// British Indian Ocean Territory

function location_province_list_io() {
  return array('PB' => "Peros Banhos",
    'SI' => "Salomon Islands",
    'NI' => "Nelsons Island",
    'TB' => "Three Brothers",
    'EA' => "Eagle Islands",
    'DI' => "Danger Island",
    'EG' => "Egmont Islands",
    'DG' => "Diego Garcia");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_io() {
  return array(
    'minlng' => 71.2711,
    'minlat' => -7.505967,
    'maxlng' => 72.47375,
    'maxlat' => -5.220733,
  );
}
