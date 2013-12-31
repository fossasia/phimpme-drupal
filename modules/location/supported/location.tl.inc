<?php

// Timor Leste

function location_province_list_tl() {
  return array(
    'AL' => "Aileu",
    'AN' => "Ainaro",
    'BA' => "Baucau",
    'BO' => "Bobonaro",
    'CO' => "Cova Lima",
    'DI' => "Dili",
    'ER' => "Ermera",
    'LA' => "Lautem",
    'LI' => "Liquica",
    'MT' => "Manatuto",
    'MF' => "Manufahi",
    'OE' => "Oecussi",
    'VI' => "Viqueque",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tl() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 124.030237,
    'minlat' => -9.5012277,
    'maxlng' => 127.313551,
    'maxlat' => -8.1353162,
  );
}
