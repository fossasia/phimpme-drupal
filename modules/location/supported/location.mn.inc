<?php

// Mongolia

function location_province_list_mn() {
  return array('1' => "Ulanbaatar",
    '035' => "Orhon",
    '037' => "Darhan uul",
    '039' => "Hentiy",
    '041' => "Hovsgol",
    '043' => "Hovd",
    '046' => "Uvs",
    '047' => "Tov",
    '049' => "Selenge",
    '051' => "Suhbaatar",
    '053' => "Omnogovi",
    '055' => "Ovorhangay",
    '057' => "Dzavhan",
    '059' => "DundgovL",
    '061' => "Dornod",
    '063' => "Dornogov",
    '064' => "Govi-Sumber",
    '065' => "Govi-Altay",
    '067' => "Bulgan",
    '069' => "Bayanhongor",
    '071' => "Bayan-Olgiy",
    '073' => "Arhangay");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mn() {
  return array(
    'minlng' => 87.6924,
    'minlat' => 41.553167,
    'maxlng' => 119.899,
    'maxlat' => 52.096,
  );
}
