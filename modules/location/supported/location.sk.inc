<?php

// Slovakia

function location_province_list_sk() {
  return array('BA' => "Banskobystricky",
    'BR' => "Bratislavsky",
    'KO' => "Kosicky",
    'NI' => "Nitriansky",
    'PR' => "Presovsky",
    'TC' => "Trenciansky",
    'TV' => "Trnavsky",
    'ZI' => "Zilinsky");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sk() {
  return array(
    'minlng' => 16.8745,
    'minlat' => 47.792133,
    'maxlng' => 22.5518,
    'maxlat' => 49.5692,
  );
}
