<?php

// Tuvalu

function location_province_list_tv() {
  return array('NMG' => "Nanumanga",
    'NLK' => "Niulakita",
    'NTO' => "Niutao",
    'FUN' => "Funafuti",
    'NME' => "Nanumea",
    'NUI' => "Nui",
    'NFT' => "Nukufetau",
    'NLL' => "Nukulaelae",
    'VAI' => "Vaitupu");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tv() {
  return array(
    'minlng' => 176.1042,
    'minlat' => -12.571533,
    'maxlng' => 179.8403,
    'maxlat' => -5.725,
  );
}
