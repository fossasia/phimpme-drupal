<?php

// Yemen

function location_province_list_ye() {
  return array('AB' => "Abyan",
    'AD' => "Adan",
    'AM' => "Amran",
    'BA' => "Al Bayda",
    'DA' => "Ad Dali",
    'DH' => "Dhamar",
    'HD' => "Hadramawt",
    'HJ' => "Hajjah",
    'HU' => "Al Hudaydah",
    'IB' => "Ibb",
    'JA' => "Al Jawf",
    'LA' => "Lahij",
    'MA' => "Ma'rib",
    'MR' => "Al Mahrah",
    'MW' => "Al Mahwit",
    'SD' => "Sa'dah",
    'SN' => "San'a",
    'SH' => "Shabwah",
    'TA' => "Ta'izz");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ye() {
  return array(
    'minlng' => 42.05785,
    'minlat' => 12.1411,
    'maxlng' => 54.45815,
    'maxlat' => 18.033511,
  );
}
