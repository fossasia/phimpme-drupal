<?php

// Oman

function location_province_list_om() {
  return array('DA' => "Ad Dakhiliyah",
    'BA' => "Al Batinah",
    'WU' => "Al Wusta",
    'SH' => "Ash Sharqiyah",
    'ZA' => "Az Zahirah",
    'MA' => "Masqat",
    'MU' => "Musandam",
    'ZU' => "Zufar");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_om() {
  return array(
    'minlng' => 52.003603,
    'minlat' => 16.691113,
    'maxlng' => 59.82915,
    'maxlat' => 26.417867,
  );
}
