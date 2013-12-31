<?php

// Peru

function location_province_list_pe() {
  return array('AM' => "Amazonas",
    'AN' => "Ancash",
    'AP' => "Apurimac",
    'AR' => "Arequipa",
    'AY' => "Ayacucho",
    'CJ' => "Cajamarca",
    'CL' => "Callao",
    'CU' => "Cusco",
    'HV' => "Huancavelica",
    'HO' => "Huanuco",
    'IC' => "Ica",
    'JU' => "Junin",
    'LD' => "La Libertad",
    'LY' => "Lambayeque",
    'LI' => "Lima",
    'LO' => "Loreto",
    'MD' => "Madre de Dios",
    'MO' => "Moquegua",
    'PA' => "Pasco",
    'PI' => "Piura",
    'PU' => "Puno",
    'SM' => "San Martin",
    'TA' => "Tacna",
    'TU' => "Tumbes",
    'UC' => "Ucayali");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pe() {
  return array(
    'minlng' => -81.3752,
    'minlat' => -18.247967,
    'maxlng' => -68.75585,
    'maxlat' => -0.050133,
  );
}
