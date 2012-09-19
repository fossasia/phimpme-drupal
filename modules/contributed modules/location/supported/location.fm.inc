<?php

// Micronesia, Federated States of

function location_province_list_fm() {
  return array(
    'C' => "Chuuk",
    'K' => "Kosrae",
    'P' => "Pohnpei",
    'Y' => "Yap",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_fm() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 138.064058,
    'minlat' => 5.26970530,
    'maxlng' => 163.046770,
    'maxlat' => 9.58869232,
  );
}
