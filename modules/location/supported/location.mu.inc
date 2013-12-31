<?php

// Mauritius

function location_province_list_mu() {
  return array('BR' => "Beau Bassin-Rose Hill",
    'CU' => "Curepipe",
    'PU' => "Port Louis",
    'QB' => "Quatre Bornes",
    'VP' => "Vacoas-Phoenix",
    'AG' => "Agalega Islands",
    'CC' => "Cargados Carajos Shoals (Saint Brandon Islands)",
    'RO' => "Rodrigues",
    'BL' => "Black River",
    'FL' => "Flacq",
    'GP' => "Grand Port",
    'MO' => "Moka",
    'PA' => "Pamplemousses",
    'PW' => "Plaines Wilhems",
    'PL' => "Port Louis",
    'RR' => "Riviere du Rempart",
    'SA' => "Savanne");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mu() {
  return array(
    'minlng' => 56.401,
    'minlat' => -20.633133,
    'maxlng' => 63.50495,
    'maxlat' => -10.402433,
  );
}
