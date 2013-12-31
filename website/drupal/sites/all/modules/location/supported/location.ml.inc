<?php

// Mali

function location_province_list_ml() {
  return array('GA' => "Gao",
    'KY' => "Kayes",
    'KD' => "Kidal",
    'KL' => "Koulikoro",
    'MP' => "Mopti",
    'SG' => "Segou",
    'SK' => "Sikasso",
    'TB' => "Tombouctou",
    'CD' => "Bamako Capital District");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ml() {
  return array(
    'minlng' => -12.08985,
    'minlat' => 10.1315,
    'maxlng' => 4.34825,
    'maxlat' => 24.997733,
  );
}
