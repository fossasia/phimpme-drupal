<?php

// Congo, The Democratic Republic of the

function location_province_list_cd() {
  return array(
    'BC' => "Bas-Congo",
    'BN' => "Bandundu",
    'EQ' => "Equateur",
    'KA' => "Katanga",
    'KE' => "Kasai-Oriental",
    'KN' => "Kinshasa",
    'KW' => "Kasai-Occidental",
    'MA' => "Maniema",
    'NK' => "Nord-Kivu",
    'OR' => "Orientale",
    'SK' => "Sud-Kivu",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cd() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 12.2109131,
    'minlat' => -13.458350,
    'maxlng' => 31.2804468,
    'maxlat' => 5.37528025,
  );
}
