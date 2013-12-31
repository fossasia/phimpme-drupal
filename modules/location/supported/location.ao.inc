<?php

// Angola

function location_province_list_ao() {
  return array('BGO' => "Bengo",
    'BGU' => "Benguela province",
    'BIE' => "Bie",
    'CAB' => "Cabinda",
    'CCU' => "Cuando-Cubango",
    'CNO' => "Cuanza Norte",
    'CUS' => "Cuanza Sul",
    'CNN' => "Cunene",
    'HUA' => "Huambo province",
    'HUI' => "Huila province",
    'LUA' => "Luanda",
    'LNO' => "Lunda Norte",
    'LSU' => "Lunda Sul",
    'MAL' => "Malange",
    'MOX' => "Moxico",
    'NAM' => "Namibe",
    'UIG' => "Uige",
    'ZAI' => "Zaire");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ao() {
  return array(
    'minlng' => 11.57905,
    'minlat' => -18.112767,
    'maxlng' => 24.1105,
    'maxlat' => -4.424667,
  );
}
