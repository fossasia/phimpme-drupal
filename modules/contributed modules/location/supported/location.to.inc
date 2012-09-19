<?php

// Tonga

function location_province_list_to() {
  return array('H' => "Ha'apai",
    'T' => "Tongatapu",
    'V' => "Vava'u");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_to() {
  return array(
    'minlng' => -176.29485,
    'minlat' => -22.319633,
    'maxlng' => -173.6335,
    'maxlat' => -15.5681,
  );
}
