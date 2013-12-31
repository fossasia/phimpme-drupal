<?php

// Korea, Democratic People's Republic Of (North Korea)

function location_province_list_kp() {
  return array(
    'CHA' => "Chagang-do",
    'HAB' => "Hamgyong-bukto",
    'HAN' => "Hamgyong-namdo",
    'HWB' => "Hwanghae-bukto",
    'HWN' => "Hwanghae-namdo",
    'KAN' => "Kangwon-do",
    'PYB' => "P'yongan-bukto",
    'PYN' => "P'yongan-namdo",
    'YAN' => "Ryanggang-do (Yanggang-do)",
    'NAJ' => "Rason Directly Governed City",
    'PYO' => "P'yongyang Special City",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kp() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 124.322106,
    'minlat' => 37.6754275,
    'maxlng' => 130.700476,
    'maxlat' => 43.0102698,
  );
}
