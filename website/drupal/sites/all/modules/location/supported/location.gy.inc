<?php

// Guyana

function location_province_list_gy() {
  return array('BW' => "Barima-Waini",
    'CM' => "Cuyuni-Mazaruni",
    'DM' => "Demerara-Mahaica",
    'EC' => "East Berbice-Corentyne",
    'EW' => "Essequibo Islands-West Demerara",
    'MB' => "Mahaica-Berbice",
    'PM' => "Pomeroon-Supenaam",
    'PI' => "Potaro-Siparuni",
    'UD' => "Upper Demerara-Berbice",
    'UT' => "Upper Takutu-Upper Essequibo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gy() {
  return array(
    'minlng' => -61.3364,
    'minlat' => 1.278567,
    'maxlng' => -56.5865,
    'maxlat' => 8.373633,
  );
}
