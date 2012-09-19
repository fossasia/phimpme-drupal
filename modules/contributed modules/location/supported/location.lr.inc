<?php

// Liberia

function location_province_list_lr() {
  return array('BI' => "Bomi",
    'BG' => "Bong",
    'GB' => "Grand Bassa",
    'CM' => "Grand Cape Mount",
    'GG' => "Grand Gedeh",
    'GK' => "Grand Kru",
    'LO' => "Lofa",
    'MG' => "Margibi",
    'ML' => "Maryland",
    'MS' => "Montserrado",
    'NB' => "Nimba",
    'RC' => "River Cess",
    'SN' => "Sinoe");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lr() {
  return array(
    'minlng' => -11.5545,
    'minlat' => 4.278,
    'maxlng' => -7.4955,
    'maxlat' => 8.526333,
  );
}
