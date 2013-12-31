<?php

// Poland

function location_province_list_pl() {
  return array('DO' => "Dolnoslaskie",
    'KP' => "Kujawsko-Pomorskie",
    'LO' => "Lodzkie",
    'LL' => "Lubelskie",
    'LU' => "Lubuskie",
    'ML' => "Malopolskie",
    'MZ' => "Mazowieckie",
    'OP' => "Opolskie",
    'PP' => "Podkarpackie",
    'PL' => "Podlaskie",
    'PM' => "Pomorskie",
    'SL' => "Slaskie",
    'SW' => "Swietokrzyskie",
    'WM' => "Warminsko-Mazurskie",
    'WP' => "Wielkopolskie",
    'ZA' => "Zachodniopomorskie");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pl() {
  return array(
    'minlng' => 14.10525,
    'minlat' => 49.0147,
    'maxlng' => 24.13405,
    'maxlat' => 54.922967,
  );
}
