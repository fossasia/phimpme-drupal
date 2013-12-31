<?php

// Namibia

function location_province_list_na() {
  return array('CA' => "Caprivi",
    'ER' => "Erongo",
    'HA' => "Hardap",
    'KR' => "Karas",
    'KV' => "Kavango",
    'KH' => "Khomas",
    'KU' => "Kunene",
    'OW' => "Ohangwena",
    'OK' => "Omaheke",
    'OT' => "Omusati",
    'ON' => "Oshana",
    'OO' => "Oshikoto",
    'OJ' => "Otjozondjupa");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_na() {
  return array(
    'minlng' => 11.72,
    'minlat' => -28.881233,
    'maxlng' => 25.23535,
    'maxlat' => -16.866767,
  );
}
