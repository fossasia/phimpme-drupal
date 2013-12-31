<?php

// Andorra

function location_province_list_ad() {
  return array('ALV' => "Andorra la Vella",
    'CAN' => "Canillo",
    'ENC' => "Encamp",
    'ESE' => "Escaldes-Engordany",
    'LMA' => "La Massana",
    'ORD' => "Ordino",
    'SJL' => "Sant Julia de L�ria");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ad() {
  return array(
    'minlng' => 1.3726,
    'minlat' => 42.404867,
    'maxlng' => 1.781219,
    'maxlat' => 42.621636,
  );
}
