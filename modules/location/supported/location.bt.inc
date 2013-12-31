<?php

// Bhutan

function location_province_list_bt() {
  return array('BUM' => "Bumthang",
    'CHU' => "Chukha",
    'DAG' => "Dagana",
    'GAS' => "Gasa",
    'HAA' => "Haa",
    'LHU' => "Lhuntse",
    'MON' => "Mongar",
    'PAR' => "Paro",
    'PEM' => "Pemagatshel",
    'PUN' => "Punakha",
    'SJO' => "Samdrup Jongkhar",
    'SAT' => "Samtse",
    'SAR' => "Sarpang",
    'THI' => "Thimphu",
    'TRG' => "Trashigang",
    'TRY' => "Trashiyangste",
    'TRO' => "Trongsa",
    'TSI' => "Tsirang",
    'WPH' => "Wangdue Phodrang",
    'ZHE' => "Zhemgang");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bt() {
  return array(
    'minlng' => 88.668547,
    'minlat' => 26.804233,
    'maxlng' => 92.08275,
    'maxlat' => 28.342233,
  );
}
