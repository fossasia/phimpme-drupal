<?php

// Burundi

function location_province_list_bi() {
  return array('BB' => "Bubanza",
    'BJ' => "Bujumbura",
    'BR' => "Bururi",
    'CA' => "Cankuzo",
    'CI' => "Cibitoke",
    'GI' => "Gitega",
    'KR' => "Karuzi",
    'KY' => "Kayanza",
    'KI' => "Kirundo",
    'MA' => "Makamba",
    'MU' => "Muramvya",
    'MY' => "Muyinga",
    'MW' => "Mwaro",
    'NG' => "Ngozi",
    'RT' => "Rutana",
    'RY' => "Ruyigi");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bi() {
  return array(
    'minlng' => 29.17565,
    'minlat' => -4.499667,
    'maxlng' => 30.8881,
    'maxlat' => -2.4882,
  );
}
