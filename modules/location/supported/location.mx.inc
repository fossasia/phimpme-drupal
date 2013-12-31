<?php

// Mexico

function location_province_list_mx() {
  return array('AGU' => "Aguascalientes",
    'BCN' => "Baja California",
    'BCS' => "Baja California Sur",
    'CAM' => "Campeche",
    'CHP' => "Chiapas",
    'CHH' => "Chihuahua",
    'COA' => "Coahuila",
    'COL' => "Colima",
    'DUR' => "Durango",
    'GUA' => "Guanajuato",
    'GRO' => "Guerrero",
    'HID' => "Hidalgo",
    'JAL' => "Jalisco",
    'MEX' => "Mexico",
    'MIC' => "Michoacan",
    'MOR' => "Morelos",
    'NAY' => "Nayarit",
    'NLE' => "Nuevo Leon",
    'OAX' => "Oaxaca",
    'PUE' => "Puebla",
    'QUE' => "Queretaro",
    'ROO' => "Quintana Roo",
    'SLP' => "San Luis Potosi",
    'SIN' => "Sinaloa",
    'SON' => "Sonora",
    'TAB' => "Tabasco",
    'TAM' => "Tamaulipas",
    'TLA' => "Tlaxcala",
    'VER' => "Veracruz",
    'YUC' => "Yucatan",
    'ZAC' => "Zacatecas",
    'DIF' => "Distrito Federal");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mx() {
  return array(
    'minlng' => -118.34045,
    'minlat' => 14.519233,
    'maxlng' => -86.63915,
    'maxlat' => 32.755267,
  );
}
