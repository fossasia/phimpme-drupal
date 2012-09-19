<?php

// Samoa

function location_province_list_ws() {
  return array(
    'AN' => "A'ana",
    'AI' => "Aiga-i-le-Tai",
    'AT' => "Atua",
    'FA' => "Fa'asaleleaga",
    'GE' => "Gaga'emauga",
    'GF' => "Gagaifomauga",
    'PA' => "Palauli",
    'SA' => "Satupa'itea",
    'TU' => "Tuamasaga",
    'VF' => "Va'a-o-Fonoti",
    'VS' => "Vaisigano",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ws() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -172.78159,
    'minlat' => -14.052836,
    'maxlng' => -171.43666,
    'maxlat' => -13.462898,
  );
}