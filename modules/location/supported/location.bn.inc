<?php

// Brunei Darussalam

function location_province_list_bn() {
  return array('BEL' => "Belait",
    'BRM' => "Brunei and Muara",
    'TEM' => "Temburong",
    'TUT' => "Tutong");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bn() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 113.999170,
    'minlat' => 4.01668101,
    'maxlng' => 115.360741,
    'maxlat' => 5.05705678,
  );
}
