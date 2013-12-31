<?php

// Guinea

function location_province_list_gn() {
  return array('CNK' => "Conakry",
    'BYL' => "Beyla",
    'BFA' => "Boffa",
    'BOK' => "Boke",
    'COY' => "Coyah",
    'DBL' => "Dabola",
    'DLB' => "Dalaba",
    'DGR' => "Dinguiraye",
    'DBR' => "Dubreka",
    'FRN' => "Faranah",
    'FRC' => "Forecariah",
    'FRI' => "Fria",
    'GAO' => "Gaoual",
    'GCD' => "Gueckedou",
    'KNK' => "Kankan",
    'KRN' => "Kerouane",
    'KND' => "Kindia",
    'KSD' => "Kissidougou",
    'KBA' => "Koubia",
    'KDA' => "Koundara",
    'KRA' => "Kouroussa",
    'LAB' => "Labe",
    'LLM' => "Lelouma",
    'LOL' => "Lola",
    'MCT' => "Macenta",
    'MAL' => "Mali",
    'MAM' => "Mamou",
    'MAN' => "Mandiana",
    'NZR' => "Nzerekore",
    'PIT' => "Pita",
    'SIG' => "Siguiri",
    'TLM' => "Telimele",
    'TOG' => "Tougue",
    'YOM' => "Yomou");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gn() {
  return array(
    'minlng' => -15.1616,
    'minlat' => 7.179467,
    'maxlng' => -7.68565,
    'maxlat' => 12.672033,
  );
}
