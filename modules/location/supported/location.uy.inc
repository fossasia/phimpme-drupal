<?php

// Uruguay

function location_province_list_uy() {
  return array('AR' => "Artigas",
    'CA' => "Canelones",
    'CL' => "Cerro Largo",
    'CO' => "Colonia",
    'DU' => "Durazno",
    'FS' => "Flores",
    'FA' => "Florida",
    'LA' => "Lavalleja",
    'MA' => "Maldonado",
    'MO' => "Montevideo",
    'PA' => "Paysandu",
    'RN' => "Rio Negro",
    'RV' => "Rivera",
    'RO' => "Rocha",
    'SL' => "Salto",
    'SO' => "Soriano",
    'SJ' => "San Jose",
    'TT' => "Treinta y Tres");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_uy() {
  return array(
    'minlng' => -58.39985,
    'minlat' => -34.9494,
    'maxlng' => -53.1923,
    'maxlat' => -30.1059,
  );
}
