<?php

// United Arab Emirates

function location_province_list_ae() {
  return array('AZ' => "Abu Zaby",
    'AJ' => "'Ajman",
    'FU' => "Al Fujayrah",
    'SH' => "Ash Shariqah",
    'DU' => "Dubayy",
    'RK' => "R'as al Khaymah",
    'UQ' => "Umm al Qaywayn");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ae() {
  return array(
    'minlng' => 50.7756,
    'minlat' => 22.540493,
    'maxlng' => 56.3684,
    'maxlat' => 26.241233,
  );
}
