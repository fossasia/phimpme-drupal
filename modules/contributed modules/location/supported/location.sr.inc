<?php

// Suriname

function location_province_list_sr() {
  return array('BR' => "Brokopondo",
    'CM' => "Commewijne",
    'CR' => "Coronie",
    'MA' => "Marowijne",
    'NI' => "Nickerie",
    'PA' => "Para",
    'PM' => "Paramaribo",
    'SA' => "Saramacca",
    'SI' => "Sipaliwini",
    'WA' => "Wanica");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sr() {
  return array(
    'minlng' => -58.09095,
    'minlat' => 1.9214,
    'maxlng' => -53.90215,
    'maxlat' => 6.035667,
  );
}
