<?php

// Ecuador

function location_province_list_ec() {
  return array('A' => "Azuay",
    'B' => "Bolivar",
    'C' => "Carchi",
    'D' => "Orellana",
    'E' => "Esmeraldas",
    'F' => "Canar",
    'G' => "Guayas",
    'H' => "Chimborazo",
    'I' => "Imbabura",
    'L' => "Loja",
    'M' => "Manabi",
    'N' => "Napo",
    'O' => "El Oro",
    'P' => "Pichincha",
    'R' => "Los Rios",
    'S' => "Morona-Santiago",
    'T' => "Tungurahua",
    'U' => "Sucumbios",
    'W' => "Galapagos",
    'X' => "Cotopaxi",
    'Y' => "Pastaza",
    'Z' => "Zamora-Chinchipe");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ec() {
  return array(
    'minlng' => -92.0364,
    'minlat' => -4.891367,
    'maxlng' => -75.23925,
    'maxlat' => 1.715733,
  );
}
