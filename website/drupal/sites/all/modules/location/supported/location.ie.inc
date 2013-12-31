<?php

// Ireland

function location_province_list_ie() {
  return array('DB' => "Dublin",
    'WK' => "Wicklow",
    'WX' => "Wexford",
    'CW' => "Carlow",
    'KD' => "Kildare",
    'MT' => "Meath",
    'LT' => "Louth",
    'MG' => "Monaghan",
    'CV' => "Cavan",
    'LF' => "Longford",
    'WM' => "Westmeath",
    'OF' => "Offaly",
    'LA' => "Laois",
    'KK' => "Kilkenny",
    'WF' => "Waterford",
    'CK' => "Cork",
    'KR' => "Kerry",
    'LM' => "Limerick",
    'TP' => "Tipperary",
    'CL' => "Clare",
    'GW' => "Galway",
    'MY' => "Mayo",
    'RC' => "Roscommon",
    'SL' => "Sligo",
    'LR' => "Leitrim",
    'DG' => "Donegal");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ie() {
  return array(
    'minlng' => -10.571,
    'minlat' => 51.450467,
    'maxlng' => -6.1045,
    'maxlat' => 55.4358,
  );
}
