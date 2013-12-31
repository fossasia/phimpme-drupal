<?php

// Bouvet Island

function location_province_list_bv() {
  return array();
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bv() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // Bounds manually eyeballed by bdragon in EPSG:4326 using the coordinate
  // readout in udig, because the poly is grouped with Norway.
  return array(
    'minlng' => 3.31,
    'minlat' => -54.4823,
    'maxlng' => 3.5201,
    'maxlat' => -54.3444,
  );
}