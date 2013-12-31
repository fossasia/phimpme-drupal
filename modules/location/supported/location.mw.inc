<?php

// Malawi

function location_province_list_mw() {
  return array('BLK' => "Balaka",
    'BLT' => "Blantyre",
    'CKW' => "Chikwawa",
    'CRD' => "Chiradzulu",
    'CTP' => "Chitipa",
    'DDZ' => "Dedza",
    'DWA' => "Dowa",
    'KRG' => "Karonga",
    'KSG' => "Kasungu",
    'LKM' => "Likoma",
    'LLG' => "Lilongwe",
    'MCG' => "Machinga",
    'MGC' => "Mangochi",
    'MCH' => "Mchinji",
    'MLJ' => "Mulanje",
    'MWZ' => "Mwanza",
    'MZM' => "Mzimba",
    'NTU' => "Ntcheu",
    'NKB' => "Nkhata Bay",
    'NKH' => "Nkhotakota",
    'NSJ' => "Nsanje",
    'NTI' => "Ntchisi",
    'PHL' => "Phalombe",
    'RMP' => "Rumphi",
    'SLM' => "Salima",
    'THY' => "Thyolo",
    'ZBA' => "Zomba");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mw() {
  return array(
    'minlng' => 32.7198,
    'minlat' => -17.0924,
    'maxlng' => 35.93715,
    'maxlat' => -9.500267,
  );
}
