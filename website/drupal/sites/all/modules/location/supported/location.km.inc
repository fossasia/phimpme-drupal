<?php

// Comoros

function location_province_list_km() {
  return array('G' => "Grande Comore",
    'A' => "Anjouan",
    'M' => "Moheli");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_km() {
  return array(
    'minlng' => 43.121,
    'minlat' => -12.405267,
    'maxlng' => 44.527,
    'maxlat' => -11.3164,
  );
}
