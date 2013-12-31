<?php

// Bosnia and Herzegovina

function location_province_list_ba() {
  return array('BRO' => "Brcko district",
    'FUS' => "Unsko-Sanski Kanton",
    'FPO' => "Posavski Kanton",
    'FTU' => "Tuzlanski Kanton",
    'FZE' => "Zenicko-Dobojski Kanton",
    'FBP' => "Bosanskopodrinjski Kanton",
    'FSB' => "Srednjebosanski Kanton",
    'FHN' => "Hercegovacko-neretvanski Kanton",
    'FZH' => "Zapadnohercegovacka Zupanija",
    'FSA' => "Kanton Sarajevo",
    'FZA' => "Zapadnobosanska",
    'SBL' => "Banja Luka",
    'SDO' => "Doboj",
    'SBI' => "Bijeljina",
    'SVL' => "Vlasenica",
    'SSR' => "Sarajevo-Romanija or Sokolac",
    'SFO' => "Foca",
    'STR' => "Trebinje");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ba() {
  return array(
    'minlng' => 15.77965,
    'minlat' => 42.621233,
    'maxlng' => 19.61625,
    'maxlat' => 45.251767,
  );
}
