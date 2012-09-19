<?php

// Kyrgyzstan

function location_province_list_kg() {
  return array('GB' => "Bishkek",
    'B' => "Batken",
    'C' => "Chu",
    'J' => "Jalal-Abad",
    'N' => "Naryn",
    'O' => "Osh",
    'T' => "Talas",
    'Y' => "Ysyk-Kol");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kg() {
  return array(
    'minlng' => 69.22635,
    'minlat' => 39.292,
    'maxlng' => 80.3863,
    'maxlat' => 43.292233,
  );
}
