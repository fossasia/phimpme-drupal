<?php

// Ascension Island

function location_province_list_ac() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ac() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // Bounds manually eyeballed by bdragon in EPSG:4326 using the coordinate
  // readout in udig, because the poly is grouped with Saint Helena.
  return array(
    'minlng' => -14.4278,
    'minlat' => -7.9875,
    'maxlng' => -14.2581,
    'maxlat' => -7.8630,
  );
}