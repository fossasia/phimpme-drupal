<?php

// Dominican Republic

function location_province_list_do() {
  return array('DN' => "Distrito Nacional",
    'AZ' => "Azua",
    'BC' => "Baoruco",
    'BH' => "Barahona",
    'DJ' => "Dajabon",
    'DU' => "Duarte",
    'EL' => "Elias Pina",
    'SY' => "El Seybo",
    'ET' => "Espaillat",
    'HM' => "Hato Mayor",
    'IN' => "Independencia",
    'AL' => "La Altagracia",
    'RO' => "La Romana",
    'VE' => "La Vega",
    'MT' => "Maria Trinidad Sanchez",
    'MN' => "Monsenor Nouel",
    'MC' => "Monte Cristi",
    'MP' => "Monte Plata",
    'PD' => "Pedernales",
    'PR' => "Peravia (Bani)",
    'PP' => "Puerto Plata",
    'SL' => "Salcedo",
    'SM' => "Samana",
    'SH' => "Sanchez Ramirez",
    'SC' => "San Cristobal",
    'JO' => "San Jose de Ocoa",
    'SJ' => "San Juan",
    'PM' => "San Pedro de Macoris",
    'SA' => "Santiago",
    'ST' => "Santiago Rodriguez",
    'SD' => "Santo Domingo",
    'VA' => "Valverde");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_do() {
  return array(
    'minlng' => -71.90115,
    'minlat' => 17.506667,
    'maxlng' => -68.2548,
    'maxlat' => 19.880967,
  );
}
