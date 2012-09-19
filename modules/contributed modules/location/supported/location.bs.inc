<?php

// Bahamas

function location_province_list_bs() {
  return array('ACK' => "Acklins",
    'BER' => "Berry Islands",
    'BIM' => "Bimini",
    'BLK' => "Black Point",
    'CAT' => "Cat Island",
    'CAB' => "Central Abaco",
    'CAN' => "Central Andros",
    'CEL' => "Central Eleuthera",
    'FRE' => "City of Freeport",
    'CRO' => "Crooked Island",
    'EGB' => "East Grand Bahama",
    'EXU' => "Exuma",
    'GRD' => "Grand Cay",
    'HAR' => "Harbour Island",
    'HOP' => "Hope Town",
    'INA' => "Inagua",
    'LNG' => "Long Island",
    'MAN' => "Mangrove Cay",
    'MAY' => "Mayaguana",
    'MOO' => "Moore's Island",
    'NAB' => "North Abaco",
    'NAN' => "North Andros",
    'NEL' => "North Eleuthera",
    'RAG' => "Ragged Island",
    'RUM' => "Rum Cay",
    'SAL' => "San Salvador",
    'SAB' => "South Abaco",
    'SAN' => "South Andros",
    'SEL' => "South Eleuthera",
    'SWE' => "Spanish Wells",
    'WGB' => "West Grand Bahama");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bs() {
  return array(
    'minlng' => -79.2951,
    'minlat' => 20.8592,
    'maxlng' => -72.6932,
    'maxlat' => 27.0249,
  );
}
