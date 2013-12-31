<?php

// Libyan Arab Jamahiriya

function location_province_list_ly() {
  return array(
    'AJ' => "Ajdabiya",
    'AZ' => "Al 'Aziziyah",
    'FA' => "Al Fatih",
    'JA' => "Al Jabal al Akhdar",
    'JU' => "Al Jufrah",
    'KH' => "Al Khums",
    'KU' => "Al Kufrah",
    'NK' => "An Nuqat al Khams",
    'AS' => "Ash Shati'",
    'AW' => "Awbari",
    'ZA' => "Az Zawiyah",
    'BA' => "Banghazi",
    'DA' => "Darnah",
    'GD' => "Ghadamis",
    'GY' => "Gharyan",
    'MI' => "Misratah",
    'MZ' => "Murzuq",
    'SB' => "Sabha",
    'SW' => "Sawfajjin",
    'SU' => "Surt",
    'TL' => "Tarabulus (Tripoli)",
    'TH' => "Tarhunah",
    'TU' => "Tubruq",
    'YA' => "Yafran",
    'ZL' => "Zlitan",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ly() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 9.28654382,
    'minlat' => 19.4961237,
    'maxlng' => 25.1568412,
    'maxlat' => 33.1810765,
  );
}
