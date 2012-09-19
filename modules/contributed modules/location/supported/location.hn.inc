<?php

// Honduras

function location_province_list_hn() {
  return array('AT' => "Atlantida",
    'CH' => "Choluteca",
    'CL' => "Colon",
    'CM' => "Comayagua",
    'CP' => "Copan",
    'CR' => "Cortes",
    'PA' => "El Paraiso",
    'FM' => "Francisco Morazan",
    'GD' => "Gracias a Dios",
    'IN' => "Intibuca",
    'IB' => "Islas de la Bahia (Bay Islands)",
    'PZ' => "La Paz",
    'LE' => "Lempira",
    'OC' => "Ocotepeque",
    'OL' => "Olancho",
    'SB' => "Santa Barbara",
    'VA' => "Valle",
    'YO' => "Yoro");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_hn() {
  return array(
    'minlng' => -89.3019,
    'minlat' => 12.961867,
    'maxlng' => -82.8413,
    'maxlat' => 17.467133,
  );
}
