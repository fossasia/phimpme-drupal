<?php

// Nauru

function location_province_list_nr() {
  return array('AO' => "Aiwo",
    'AA' => "Anabar",
    'AT' => "Anetan",
    'AI' => "Anibare",
    'BA' => "Baiti",
    'BO' => "Boe",
    'BU' => "Buada",
    'DE' => "Denigomodu",
    'EW' => "Ewa",
    'IJ' => "Ijuw",
    'ME' => "Meneng",
    'NI' => "Nibok",
    'UA' => "Uaboe",
    'YA' => "Yaren");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_nr() {
  return array(
    'minlng' => 167.0567,
    'minlat' => -0.589533,
    'maxlng' => 167.11675,
    'maxlat' => -0.5351,
  );
}
