<?php

// Palau

function location_province_list_pw() {
  return array('AM' => "Aimeliik",
    'AR' => "Airai",
    'AN' => "Angaur",
    'HA' => "Hatohobei",
    'KA' => "Kayangel",
    'KO' => "Koror",
    'ME' => "Melekeok",
    'NA' => "Ngaraard",
    'NG' => "Ngarchelong",
    'ND' => "Ngardmau",
    'NT' => "Ngatpang",
    'NC' => "Ngchesar",
    'NR' => "Ngeremlengui",
    'NW' => "Ngiwal",
    'PE' => "Peleliu",
    'SO' => "Sonsorol");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pw() {
  return array(
    'minlng' => 134.1386,
    'minlat' => 6.798967,
    'maxlng' => 134.8889,
    'maxlat' => 7.697967,
  );
}
