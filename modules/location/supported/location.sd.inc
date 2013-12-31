<?php

// Sudan

function location_province_list_sd() {
  return array('ANL' => "A'ali an Nil",
    'BAM' => "Al Bahr al Ahmar",
    'BRT' => "Al Buhayrat",
    'JZR' => "Al Jazirah",
    'KRT' => "Al Khartum",
    'QDR' => "Al Qadarif",
    'WDH' => "Al Wahdah",
    'ANB' => "An Nil al Abyad",
    'ANZ' => "An Nil al Azraq",
    'ASH' => "Ash Shamaliyah",
    'BJA' => "Bahr al Jabal",
    'GIS' => "Gharb al Istiwa'iyah",
    'GBG' => "Gharb Bahr al Ghazal",
    'GDA' => "Gharb Darfur",
    'GKU' => "Gharb Kurdufan",
    'JDA' => "Janub Darfur",
    'JKU' => "Janub Kurdufan",
    'JQL' => "Junqali",
    'KSL' => "Kassala",
    'NNL' => "Nahr an Nil",
    'SBG' => "Shamal Bahr al Ghazal",
    'SDA' => "Shamal Darfur",
    'SKU' => "Shamal Kurdufan",
    'SIS' => "Sharq al Istiwa'iyah",
    'SNR' => "Sinnar",
    'WRB' => "Warab");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sd() {
  return array(
    'minlng' => 21.8241,
    'minlat' => 3.426933,
    'maxlng' => 38.9644,
    'maxlat' => 22.009756,
  );
}
