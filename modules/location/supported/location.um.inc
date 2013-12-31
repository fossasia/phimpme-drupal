<?php

// United States Minor Outlying Islands

function location_province_list_um() {
  return array(
    'BI' => "Baker Island",
    'HI' => "Howland Island",
    'JI' => "Jarvis Island",
    'JA' => "Johnston Atoll",
    'KR' => "Kingman Reef",
    'MA' => "Midway Atoll",
    'NI' => "Navassa Island",
    'PA' => "Palmyra Atoll",
    'WI' => "Wake Island",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_um() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -177.38854,
    'minlat' => -0.3889673,
    'maxlng' => 166.652597,
    'maxlat' => 28.2151006,
  );
}
