<?php

// Venezuela

function location_province_list_ve() {
  return array('Z' => "Amazonas",
    'B' => "Anzoategui",
    'C' => "Apure",
    'D' => "Aragua",
    'E' => "Barinas",
    'F' => "Bolivar",
    'G' => "Carabobo",
    'H' => "Cojedes",
    'Y' => "Delta Amacuro",
    'I' => "Falcon",
    'J' => "Guarico",
    'K' => "Lara",
    'L' => "Merida",
    'M' => "Miranda",
    'N' => "Monagas",
    'O' => "Nueva Esparta",
    'P' => "Portuguesa",
    'R' => "Sucre",
    'S' => "Tachira",
    'T' => "Trujillo",
    'X' => "Vargas",
    'U' => "Yaracuy",
    'V' => "Zulia",
    'A' => "Federal District",
    'W' => "Federal Dependency");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ve() {
  return array(
    'minlng' => -73.37305,
    'minlat' => 0.793967,
    'maxlng' => -59.8755,
    'maxlat' => 15.7633,
  );
}
