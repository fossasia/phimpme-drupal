<?php

// India

function location_province_list_in() {
  return array('AP' => "Andhra Pradesh",
    'AR' => "Arunachal Pradesh",
    'AS' => "Assam",
    'BR' => "Bihar",
    'CT' => "Chhattisgarh",
    'GA' => "Goa",
    'GJ' => "Gujarat",
    'HR' => "Haryana",
    'HP' => "Himachal Pradesh",
    'JK' => "Jammu and Kashmir",
    'JH' => "Jharkhand",
    'KA' => "Karnataka",
    'KL' => "Kerala",
    'MP' => "Madhya Pradesh",
    'MM' => "Maharashtra",
    'MN' => "Manipur",
    'ML' => "Meghalaya",
    'MZ' => "Mizoram",
    'NL' => "Nagaland",
    'OR' => "Orissa",
    'PB' => "Punjab",
    'RJ' => "Rajasthan",
    'SK' => "Sikkim",
    'TN' => "Tamil Nadu",
    'TR' => "Tripura",
    'UL' => "Uttaranchal",
    'UP' => "Uttar Pradesh",
    'WB' => "West Bengal",
    'AN' => "Andaman and Nicobar Islands",
    'CH' => "Chandigarh",
    'DN' => "Dadra and Nagar Haveli",
    'DD' => "Daman and Diu",
    'DL' => "Delhi",
    'LD' => "Lakshadweep",
    'PY' => "Pondicherry");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_in() {
  return array(
    'minlng' => 68.1721,
    'minlat' => 6.470867,
    'maxlng' => 97.27195,
    'maxlat' => 37.118133,
  );
}
