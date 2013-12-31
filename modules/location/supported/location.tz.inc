<?php

// Tanzania, United Republic of

function location_province_list_tz() {
  return array('AR' => "Arusha",
    'DS' => "Dar es Salaam",
    'DO' => "Dodoma",
    'IR' => "Iringa",
    'KA' => "Kagera",
    'KI' => "Kigoma",
    'KJ' => "Kilimanjaro",
    'LN' => "Lindi",
    'MY' => "Manyara",
    'MR' => "Mara",
    'MB' => "Mbeya",
    'MO' => "Morogoro",
    'MT' => "Mtwara",
    'MW' => "Mwanza",
    'PN' => "Pemba North",
    'PS' => "Pemba South",
    'PW' => "Pwani",
    'RK' => "Rukwa",
    'RV' => "Ruvuma",
    'SH' => "Shinyanga",
    'SI' => "Singida",
    'TB' => "Tabora",
    'TN' => "Tanga",
    'ZC' => "Zanzibar Central/South",
    'ZN' => "Zanzibar North",
    'ZU' => "Zanzibar Urban/West");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tz() {
  return array(
    'minlng' => 29.29035,
    'minlat' => -11.737133,
    'maxlng' => 40.4958,
    'maxlat' => -1.0601,
  );
}
