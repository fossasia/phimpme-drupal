<?php

// Ukraine

function location_province_list_ua() {
  return array('CK' => "Cherkasy",
    'CH' => "Chernihiv",
    'CV' => "Chernivtsi",
    'CR' => "Crimea",
    'DN' => "Dnipropetrovs'k",
    'DO' => "Donets'k",
    'IV' => "Ivano-Frankivs'k",
    'KL' => "Kharkiv Kherson",
    'KM' => "Khmel'nyts'kyy",
    'KR' => "Kirovohrad",
    'KV' => "Kiev",
    'KY' => "Kyyiv",
    'LU' => "Luhans'k",
    'LV' => "L'viv",
    'MY' => "Mykolayiv",
    'OD' => "Odesa",
    'PO' => "Poltava",
    'RI' => "Rivne",
    'SE' => "Sevastopol",
    'SU' => "Sumy",
    'TE' => "Ternopil'",
    'VI' => "Vinnytsya",
    'VO' => "Volyn'",
    'ZK' => "Zakarpattya",
    'ZA' => "Zaporizhzhya",
    'ZH' => "Zhytomyr");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ua() {
  return array(
    'minlng' => 22.0955,
    'minlat' => 44.4744,
    'maxlng' => 40.13205,
    'maxlat' => 52.452833,
  );
}
