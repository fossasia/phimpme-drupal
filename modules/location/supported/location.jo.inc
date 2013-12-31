<?php

// Jordan

function location_province_list_jo() {
  return array('AJ' => "Ajlun",
    'AQ' => "Al 'Aqabah",
    'BA' => "Al Balqa'",
    'KA' => "Al Karak",
    'MF' => "Al Mafraq",
    'AM' => "'Amman",
    'TA' => "At Tafilah",
    'ZA' => "Az Zarqa'",
    'IR' => "Irbid",
    'JA' => "Jarash",
    'MN' => "Ma'an",
    'MD' => "Madaba");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_jo() {
  return array(
    'minlng' => 34.8481,
    'minlat' => 29.204733,
    'maxlng' => 39.74045,
    'maxlat' => 33.319433,
  );
}
