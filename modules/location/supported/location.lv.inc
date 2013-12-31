<?php

// Latvia

function location_province_list_lv() {
  return array('AIZ' => "Aizkraukles Rajons",
    'ALU' => "Aluksnes Rajons",
    'BAL' => "Balvu Rajons",
    'BAU' => "Bauskas Rajons",
    'CES' => "Cesu Rajons",
    'DGR' => "Daugavpils Rajons",
    'DOB' => "Dobeles Rajons",
    'GUL' => "Gulbenes Rajons",
    'JEK' => "Jekabpils Rajons",
    'JGR' => "Jelgavas Rajons",
    'KRA' => "Kraslavas Rajons",
    'KUL' => "Kuldigas Rajons",
    'LPR' => "Liepajas Rajons",
    'LIM' => "Limbazu Rajons",
    'LUD' => "Ludzas Rajons",
    'MAD' => "Madonas Rajons",
    'OGR' => "Ogres Rajons",
    'PRE' => "Preilu Rajons",
    'RZR' => "Rezeknes Rajons",
    'RGR' => "Rigas Rajons",
    'SAL' => "Saldus Rajons",
    'TAL' => "Talsu Rajons",
    'TUK' => "Tukuma Rajons",
    'VLK' => "Valkas Rajons",
    'VLM' => "Valmieras Rajons",
    'VSR' => "Ventspils Rajons",
    'DGV' => "Daugavpils",
    'JGV' => "Jelgava",
    'JUR' => "Jurmala",
    'LPK' => "Liepaja",
    'RZK' => "Rezekne",
    'RGA' => "Riga",
    'VSL' => "Ventspils");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lv() {
  return array(
    'minlng' => 20.8848,
    'minlat' => 55.723733,
    'maxlng' => 28.1695,
    'maxlat' => 58.147367,
  );
}
