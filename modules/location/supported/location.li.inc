<?php

// Liechtenstein

function location_province_list_li() {
  return array('V' => "Vaduz",
    'A' => "Schaan",
    'B' => "Balzers",
    'N' => "Triesen",
    'E' => "Eschen",
    'M' => "Mauren",
    'T' => "Triesenberg",
    'R' => "Ruggell",
    'G' => "Gamprin",
    'L' => "Schellenberg",
    'P' => "Planken");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_li() {
  return array(
    'minlng' => 9.417494,
    'minlat' => 46.997118,
    'maxlng' => 9.61825,
    'maxlat' => 47.206967,
  );
}
