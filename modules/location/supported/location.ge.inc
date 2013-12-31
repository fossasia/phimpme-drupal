<?php

// Georgia

function location_province_list_ge() {
  return array('AB' => "Abkhazia",
    'AJ' => "Ajaria",
    'TB' => "Tbilisi",
    'GU' => "Guria",
    'IM' => "Imereti",
    'KA' => "Kakheti",
    'KK' => "Kvemo Kartli",
    'MM' => "Mtskheta-Mtianeti",
    'RL' => "Racha Lechkhumi and Kvemo Svaneti",
    'SZ' => "Samegrelo-Zemo Svaneti",
    'SJ' => "Samtskhe-Javakheti",
    'SK' => "Shida Kartli");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ge() {
  return array(
    'minlng' => 39.923261,
    'minlat' => 41.1446,
    'maxlng' => 46.7882,
    'maxlat' => 43.6354,
  );
}
