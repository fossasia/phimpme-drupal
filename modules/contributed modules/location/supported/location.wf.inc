<?php

// Wallis and Futuna

function location_province_list_wf() {
  return array(
    'A' => "Alo",
    'S' => "Sigave",
    'W' => "Wallis",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_wf() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -178.18470,
    'minlat' => -14.378811,
    'maxlng' => -176.12456,
    'maxlat' => -13.209063,
  );
}