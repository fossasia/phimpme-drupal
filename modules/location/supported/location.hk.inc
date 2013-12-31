<?php

// Hong Kong

function location_province_list_hk() {
  return array(
    'HCW' => "Central and Western Hong Kong Island",
    'HEA' => "Eastern Hong Kong Island",
    'HSO' => "Southern Hong Kong Island",
    'HWC' => "Wan Chai Hong Kong Island",
    'KKC' => "Kowloon City Kowloon",
    'KKT' => "Kwun Tong Kowloon",
    'KSS' => "Sham Shui Po Kowloon",
    'KWT' => "Wong Tai Sin Kowloon",
    'KYT' => "Yau Tsim Mong Kowloon",
    'NIS' => "Islands New Territories",
    'NKT' => "Kwai Tsing New Territories",
    'NNO' => "North New Territories",
    'NSK' => "Sai Kung New Territories",
    'NST' => "Sha Tin New Territories",
    'NTP' => "Tai Po New Territories",
    'NTW' => "Tsuen Wan New Territories",
    'NTM' => "Tuen Mun New Territories",
    'NYL' => "Yuen Long New Territories",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_hk() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 113.837319,
    'minlat' => 22.1786990,
    'maxlng' => 114.401316,
    'maxlat' => 22.5605361,
  );
}
