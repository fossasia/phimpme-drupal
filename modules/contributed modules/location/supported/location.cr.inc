<?php

// Costa Rica

function location_province_list_cr() {
  return array('AL' => "Alajuela",
    'CA' => "Cartago",
    'GU' => "Guanacaste",
    'HE' => "Heredia",
    'LI' => "Limon",
    'PU' => "Puntarenas",
    'SJ' => "San Jose");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cr() {
  return array(
    'minlng' => -87.01515,
    'minlat' => 5.4829,
    'maxlng' => -82.6145,
    'maxlat' => 11.217367,
  );
}
