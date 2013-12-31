<?php

// Iran, Islamic Republic Of

function location_province_list_ir() {
  return array('TEH' => "Tehran",
    'QOM' => "Qom",
    'MKZ' => "Markazi",
    'QAZ' => "Qazvin",
    'GIL' => "Gilan",
    'ARD' => "Ardabil",
    'ZAN' => "Zanjan",
    'EAZ' => "East Azarbaijan",
    'WEZ' => "West Azarbaijan",
    'KRD' => "Kurdistan",
    'HMD' => "Hamadan",
    'KRM' => "Kermanshah",
    'ILM' => "Ilam",
    'LRS' => "Lorestan",
    'KZT' => "Khuzestan",
    'CMB' => "Chahar Mahaal and Bakhtiari",
    'KBA' => "Kohkiluyeh and Buyer Ahmad",
    'BSH' => "Bushehr",
    'FAR' => "Fars",
    'HRM' => "Hormozgan",
    'SBL' => "Sistan and Baluchistan",
    'KRB' => "Kerman",
    'YZD' => "Yazd",
    'EFH' => "Esfahan",
    'SMN' => "Semnan",
    'MZD' => "Mazandaran",
    'GLS' => "Golestan",
    'NKH' => "North Khorasan",
    'RKH' => "Razavi Khorasan",
    'SKH' => "South Khorasan");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ir() {
  return array(
    'minlng' => 44.01905,
    'minlat' => 25.120267,
    'maxlng' => 63.3325,
    'maxlat' => 39.825267,
  );
}
