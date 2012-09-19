<?php

// Croatia

function location_province_list_hr() {
  return array('01' => "Zagreb county",
    '02' => "Krapina-Zagorje county",
    '03' => "Sisak-Moslavina county",
    '04' => "Karlovac county",
    '05' => "Varazdin county",
    '06' => "Koprivnica-Krizevci county",
    '07' => "Bjelovar-Bilogora county",
    '08' => "Primorje-Gorski Kotar county",
    '09' => "Lika-Senj county",
    '10' => "Virovitica-Podravina county",
    '11' => "Pozega-Slavonia county",
    '12' => "Brod-Posavina county",
    '13' => "Zadar county",
    '14' => "Osijek-Baranja county",
    '15' => "Sibenik-Knin county",
    '16' => "Vukovar-Srijem county",
    '17' => "Split-Dalmatia county",
    '18' => "Istria county",
    '19' => "Dubrovnik-Neretva county",
    '20' => "Medjimurje county",
    '21' => "Zagreb (city)");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_hr() {
  return array(
    'minlng' => 13.50481,
    'minlat' => 42.4555,
    'maxlng' => 19.2728,
    'maxlat' => 46.583016,
  );
}
