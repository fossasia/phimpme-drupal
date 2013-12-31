<?php

// Monaco

function location_province_list_mc() {
  return array('FV' => "Fontvieille",
    'LC' => "La Condamine",
    'MV' => "Monaco-Ville",
    'MC' => "Monte-Carlo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mc() {
  return array(
    'minlng' => 7.267656,
    'minlat' => 43.6426,
    'maxlng' => 7.53715,
    'maxlat' => 43.927506,
  );
}
