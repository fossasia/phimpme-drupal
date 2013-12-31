<?php

// San Marino

function location_province_list_sm() {
  return array('AC' => "Acquaviva",
    'BM' => "Borgo Maggiore",
    'CH' => "Chiesanuova",
    'DO' => "Domagnano",
    'FA' => "Faetano",
    'FI' => "Fiorentino",
    'MO' => "Montegiardino",
    'SM' => "Citta di San Marino",
    'SE' => "Serravalle");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sm() {
  return array(
    'minlng' => 12.23665,
    'minlat' => 43.882767,
    'maxlng' => 12.4976,
    'maxlat' => 44.070533,
  );
}
