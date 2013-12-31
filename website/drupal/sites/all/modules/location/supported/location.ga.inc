<?php

// Gabon

function location_province_list_ga() {
  return array('ES' => "Estuaire",
    'HO' => "Haut-Ogooue",
    'MO' => "Moyen-Ogooue",
    'NG' => "Ngounie",
    'NY' => "Nyanga",
    'OI' => "Ogooue-Ivindo",
    'OL' => "Ogooue-Lolo",
    'OM' => "Ogooue-Maritime",
    'WN' => "Woleu-Ntem");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ga() {
  return array(
    'minlng' => 8.59375,
    'minlat' => -3.9324,
    'maxlng' => 14.72285,
    'maxlat' => 2.299667,
  );
}
