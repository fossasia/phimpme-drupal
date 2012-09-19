<?php

// Mauritania

function location_province_list_mr() {
  return array('AD' => "Adrar",
    'AS' => "Assaba",
    'BR' => "Brakna",
    'DN' => "Dakhlet Nouadhibou",
    'GO' => "Gorgol",
    'GM' => "Guidimaka",
    'HC' => "Hodh Ech Chargui",
    'HG' => "Hodh El Gharbi",
    'IN' => "Inchiri",
    'TA' => "Tagant",
    'TZ' => "Tiris Zemmour",
    'TR' => "Trarza",
    'NO' => "Nouakchott");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mr() {
  return array(
    'minlng' => -17.0583,
    'minlat' => 14.669667,
    'maxlng' => -5.00065,
    'maxlat' => 27.406533,
  );
}
