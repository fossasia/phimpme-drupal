<?php

// Kazakhstan

function location_province_list_kz() {
  return array('ALA' => "Almaty",
    'AST' => "Astana",
    'ALM' => "Almaty",
    'AKM' => "Aqmola",
    'AKT' => "Aqtobe",
    'ATY' => "Atyrau",
    'ZAP' => "Baty Qazaqstan",
    'MAN' => "Mangghystau",
    'YUZ' => "Ongtustik Qazaqstan",
    'PAV' => "Paylodar",
    'KAR' => "Qaraghandy",
    'KUS' => "Qustanay",
    'KZY' => "Qyzylorda",
    'VOS' => "Shyghys Qazaqstan",
    'SEV' => "Soltustik Qazaqstan",
    'ZHA' => "Zhambyl");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kz() {
  return array(
    'minlng' => 46.4657,
    'minlat' => 40.6715,
    'maxlng' => 87.40955,
    'maxlat' => 55.4617,
  );
}
