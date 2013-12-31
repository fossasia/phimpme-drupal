<?php

// Trinidad and Tobago

function location_province_list_tt() {
  return array('CT' => "Couva/Tabaquite/Talparo",
    'DM' => "Diego Martin",
    'MR' => "Mayaro/Rio Claro",
    'PD' => "Penal/Debe",
    'PT' => "Princes Town",
    'SG' => "Sangre Grande",
    'SL' => "San Juan/Laventille",
    'SI' => "Siparia",
    'TP' => "Tunapuna/Piarco",
    'PS' => "Port of Spain",
    'SF' => "San Fernando",
    'AR' => "Arima",
    'PF' => "Point Fortin",
    'CH' => "Chaguanas",
    'TO' => "Tobago");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tt() {
  return array(
    'minlng' => -61.93015,
    'minlat' => 10.0588,
    'maxlng' => -60.5064,
    'maxlat' => 11.2911,
  );
}
