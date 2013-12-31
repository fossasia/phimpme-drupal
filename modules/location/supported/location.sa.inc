<?php

// Saudia Arabia

function location_province_list_sa() {
  return array('BH' => "Al Bahah",
    'HS' => "Al Hudud ash Shamaliyah",
    'JF' => "Al Jawf",
    'MD' => "Al Madinah",
    'QS' => "Al Qasim",
    'RD' => "Ar Riyad",
    'AQ' => "Ash Sharqiyah (Eastern Province)",
    'AS' => "'Asir",
    'HL' => "Ha'il",
    'JZ' => "Jizan",
    'ML' => "Makkah",
    'NR' => "Najran",
    'TB' => "Tabuk");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sa() {
  return array(
    'minlng' => 34.5647,
    'minlat' => 15.518117,
    'maxlng' => 55.028463,
    'maxlat' => 32.275,
  );
}
