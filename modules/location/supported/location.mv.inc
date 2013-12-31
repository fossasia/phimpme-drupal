<?php

// Maldives

function location_province_list_mv() {
  return array('THU' => "Thiladhunmathi Uthuru",
    'THD' => "Thiladhunmathi Dhekunu",
    'MLU' => "Miladhunmadulu Uthuru",
    'MLD' => "Miladhunmadulu Dhekunu",
    'MAU' => "Maalhosmadulu Uthuru",
    'MAD' => "Maalhosmadulu Dhekunu",
    'FAA' => "Faadhippolhu",
    'MAA' => "Male Atoll",
    'AAU' => "Ari Atoll Uthuru",
    'AAD' => "Ari Atoll Dheknu",
    'FEA' => "Felidhe Atoll",
    'MUA' => "Mulaku Atoll",
    'NAU' => "Nilandhe Atoll Uthuru",
    'NAD' => "Nilandhe Atoll Dhekunu",
    'KLH' => "Kolhumadulu",
    'HDH' => "Hadhdhunmathi",
    'HAU' => "Huvadhu Atoll Uthuru",
    'HAD' => "Huvadhu Atoll Dhekunu",
    'FMU' => "Fua Mulaku",
    'ADD' => "Addu");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mv() {
  return array(
    'minlng' => 72.5723,
    'minlat' => -0.739367,
    'maxlng' => 73.6987,
    'maxlat' => 7.194,
  );
}
