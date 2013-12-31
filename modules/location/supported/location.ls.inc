<?php

// Lesotho

function location_province_list_ls() {
  return array('BE' => "Berea",
    'BB' => "Butha-Buthe",
    'LE' => "Leribe",
    'MF' => "Mafeteng",
    'MS' => "Maseru",
    'MH' => "Mohale's Hoek",
    'MK' => "Mokhotlong",
    'QN' => "Qacha's Nek",
    'QT' => "Quthing",
    'TT' => "Thaba-Tseka");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ls() {
  return array(
    'minlng' => 27.04245,
    'minlat' => -30.613033,
    'maxlng' => 29.4317,
    'maxlat' => -28.549033,
  );
}
