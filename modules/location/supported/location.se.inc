<?php

// Sweden

function location_province_list_se() {
  return array('ST' => "Stockholm",
    'UP' => "Uppsala",
    'SO' => "Södermanland",
    'OG' => "Östergotland",
    'JO' => "Jönköping",
    'KR' => "Kronoberg",
    'KA' => "Kalmar",
    'GO' => "Gotland",
    'BL' => "Blekinge",
    'SK' => "Skåne",
    'HA' => "Halland",
    'VG' => "Västra Götaland",
    'VL' => "Värmland",
    'OR' => "Örebro",
    'VM' => "Västmanland",
    'DA' => "Dalarna",
    'GA' => "Gävleborg",
    'VN' => "Västernorrland",
    'JA' => "Jämtland",
    'VB' => "Västerbotten",
    'NO' => "Norrbotten");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_se() {
  return array(
    'minlng' => 11.06005,
    'minlat' => 55.385433,
    'maxlng' => 23.989,
    'maxlat' => 69.126433,
  );
}
