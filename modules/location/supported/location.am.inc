<?php

// Armenia

function location_province_list_am() {
  return array('AGT' => "Aragatsotn",
    'ARR' => "Ararat",
    'ARM' => "Armavir",
    'GEG' => "Geghark'unik'",
    'KOT' => "Kotayk'",
    'LOR' => "Lorri",
    'SHI' => "Shirak",
    'SYU' => "Syunik'",
    'TAV' => "Tavush",
    'VAY' => "Vayots' Dzor",
    'YER' => "Yerevan");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_am() {
  return array(
    'minlng' => 43.4194,
    'minlat' => 38.9416,
    'maxlng' => 46.5982,
    'maxlat' => 41.3796,
  );
}
