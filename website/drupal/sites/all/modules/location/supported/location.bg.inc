<?php

// Bulgaria

function location_province_list_bg() {
  return array('01' => "Blagoevgrad",
    '02' => "Burgas",
    '03' => "Varna",
    '04' => "Veliko Turnovo",
    '05' => "Vidin",
    '06' => "Vratsa",
    '07' => "Gabrovo",
    '08' => "Dobrich",
    '09' => "Kurdzhali",
    '10' => "Kyustendil",
    '11' => "Lovech",
    '12' => "Montana",
    '13' => "Pazardzhik",
    '14' => "Pernik",
    '15' => "Pleven",
    '16' => "Plovdiv",
    '17' => "Razgrad",
    '18' => "Ruse",
    '19' => "Silistra",
    '20' => "Sliven",
    '21' => "Smolyan",
    '23' => "Sofia",
    '22' => "Sofia Region",
    '24' => "Stara Zagora",
    '25' => "Turgovishte",
    '26' => "Khaskovo",
    '27' => "Shumen",
    '28' => "Yambol");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bg() {
  return array(
    'minlng' => 22.34235,
    'minlat' => 41.2841,
    'maxlng' => 28.559,
    'maxlat' => 44.237,
  );
}
