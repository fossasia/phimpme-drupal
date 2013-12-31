<?php

// Albania

function location_province_list_al() {
  return array('BR' => "Berat",
    'BU' => "Bulqize",
    'DL' => "Delvine",
    'DV' => "Devoll",
    'DI' => "Diber",
    'DR' => "Durres",
    'EL' => "Elbasan",
    'ER' => "Kolonje",
    'FR' => "Fier",
    'GJ' => "Gjirokaster",
    'GR' => "Gramsh",
    'HA' => "Has",
    'KA' => "Kavaje",
    'KB' => "Kurbin",
    'KC' => "Kucove",
    'KO' => "Korce",
    'KR' => "Kruje",
    'KU' => "Kukes",
    'LB' => "Librazhd",
    'LE' => "Lezhe",
    'LU' => "Lushnje",
    'MM' => "Malesi e Madhe",
    'MK' => "Mallakaster",
    'MT' => "Mat",
    'MR' => "Mirdite",
    'PQ' => "Peqin",
    'PR' => "Permet",
    'PG' => "Pogradec",
    'PU' => "Puke",
    'SH' => "Shkoder",
    'SK' => "Skrapar",
    'SR' => "Sarande",
    'TE' => "Tepelene",
    'TP' => "Tropoje",
    'TR' => "Tirane",
    'VL' => "Vlore");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_al() {
  return array(
    'minlng' => 19.29795,
    'minlat' => 39.713733,
    'maxlng' => 21.072,
    'maxlat' => 42.7006,
  );
}
