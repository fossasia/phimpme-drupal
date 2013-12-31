<?php

// Luxembourg

function location_province_list_lu() {
  return array('DD' => "Diekirch",
    'DC' => "Clervaux",
    'DR' => "Redange",
    'DV' => "Vianden",
    'DW' => "Wiltz",
    'GG' => "Grevenmacher",
    'GE' => "Echternach",
    'GR' => "Remich",
    'LL' => "Luxembourg",
    'LC' => "Capellen",
    'LE' => "Esch-sur-Alzette",
    'LM' => "Mersch");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lu() {
  return array(
    'minlng' => 5.69075,
    'minlat' => 49.470867,
    'maxlng' => 6.43925,
    'maxlat' => 50.182633,
  );
}
