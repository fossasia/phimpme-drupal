<?php

// Tunisia

function location_province_list_tn() {
  return array('AR' => "Ariana",
    'BJ' => "Beja",
    'BA' => "Ben Arous",
    'BI' => "Bizerte",
    'GB' => "Gabes",
    'GF' => "Gafsa",
    'JE' => "Jendouba",
    'KR' => "Kairouan",
    'KS' => "Kasserine",
    'KB' => "Kebili",
    'KF' => "Kef",
    'MH' => "Mahdia",
    'MN' => "Manouba",
    'ME' => "Medenine",
    'MO' => "Monastir",
    'NA' => "Nabeul",
    'SF' => "Sfax",
    'SD' => "Sidi",
    'SL' => "Siliana",
    'SO' => "Sousse",
    'TA' => "Tataouine",
    'TO' => "Tozeur",
    'TU' => "Tunis",
    'ZA' => "Zaghouan");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tn() {
  return array(
    'minlng' => 7.521,
    'minlat' => 30.1174,
    'maxlng' => 11.59235,
    'maxlat' => 37.291,
  );
}
