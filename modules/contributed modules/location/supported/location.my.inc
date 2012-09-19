<?php

// Malaysia

function location_province_list_my() {
  return array('JH' => "Johor",
    'KD' => "Kedah",
    'KN' => "Kelantan",
    'KL' => "Kuala Lumpur",
    'ML' => "Malacca",
    'NS' => "Negeri Sembilan",
    'PH' => "Pahang",
    'PJ' => "Putrajaya",
    'PK' => "Perak",
    'PS' => "Perlis",
    'PG' => "Penang",
    'WP' => "Wilayah Persekutuan",
    'SL' => "Selangor",
    'TR' => "Terengganu",
    'LB' => "Labuan",
    'SB' => "Sabah",
    'SR' => "Sarawak");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_my() {
  return array(
    'minlng' => 99.6346,
    'minlat' => 0.9351,
    'maxlng' => 119.2795,
    'maxlat' => 7.417033,
  );
}
