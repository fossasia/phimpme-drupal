<?php

// Ethiopia

function location_province_list_et() {
  return array('AF' => "Afar",
    'AH' => "Amhara",
    'BG' => "Benishangul-Gumaz",
    'GB' => "Gambela",
    'HR' => "Hariai",
    'OR' => "Oromia",
    'SM' => "Somali",
    'SN' => "Southern Nations - Nationalities and Peoples Region",
    'TG' => "Tigray",
    'AA' => "Addis Ababa",
    'DD' => "Dire Dawa");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_et() {
  return array(
    'minlng' => 33.0563,
    'minlat' => 3.550567,
    'maxlng' => 47.97805,
    'maxlat' => 19.896167,
  );
}
