<?php

// Cambodia

function location_province_list_kh() {
  return array('PP' => "Phnom Penh",
    'PS' => "Preah Seihanu (Kompong Som or Sihanoukville)",
    'PA' => "Pailin",
    'KB' => "Keb",
    'BM' => "Banteay Meanchey",
    'BA' => "Battambang",
    'KM' => "Kampong Cham",
    'KN' => "Kampong Chhnang",
    'KU' => "Kampong Speu",
    'KO' => "Kampong Som",
    'KT' => "Kampong Thom",
    'KP' => "Kampot",
    'KL' => "Kandal",
    'KK' => "Kaoh Kong",
    'KR' => "Kratie",
    'MK' => "Mondul Kiri",
    'OM' => "Oddar Meancheay",
    'PU' => "Pursat",
    'PR' => "Preah Vihear",
    'PG' => "Prey Veng",
    'RK' => "Ratanak Kiri",
    'SI' => "Siemreap",
    'ST' => "Stung Treng",
    'SR' => "Svay Rieng",
    'TK' => "Takeo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kh() {
  return array(
    'minlng' => 102.2838,
    'minlat' => 10.4398,
    'maxlng' => 107.52335,
    'maxlat' => 14.561533,
  );
}
