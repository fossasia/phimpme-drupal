<?php

// Marshall Islands

function location_province_list_mh() {
  return array(
    'ALG' => "Ailinginae",
    'ALL' => "Ailinglaplap",
    'ALK' => "Ailuk",
    'ARN' => "Arno",
    'AUR' => "Aur",
    'BKR' => "Bikar",
    'BKN' => "Bikini",
    'BKK' => "Bokak",
    'EBN' => "Ebon",
    'ENT' => "Enewetak",
    'EKB' => "Erikub",
    'JBT' => "Jabat",
    'JLT' => "Jaluit",
    'JEM' => "Jemo",
    'KIL' => "Kili",
    'KWJ' => "Kwajalein",
    'LAE' => "Lae",
    'LIB' => "Lib",
    'LKP' => "Likiep",
    'MJR' => "Majuro",
    'MLP' => "Maloelap",
    'MJT' => "Mejit",
    'MIL' => "Mili",
    'NMK' => "Namorik",
    'NAM' => "Namu",
    'RGL' => "Rongelap",
    'RGK' => "Rongrik",
    'TOK' => "Toke",
    'UJA' => "Ujae",
    'UJL' => "Ujelang",
    'UTK' => "Utirik",
    'WTH' => "Wotho",
    'WTJ' => "Wotje",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mh() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 165.282448,
    'minlat' => 4.57359792,
    'maxlng' => 172.029943,
    'maxlat' => 14.6102204,
  );
}
