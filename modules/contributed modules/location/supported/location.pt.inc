<?php

// Portugal

function location_province_list_pt() {
  return array('AV' => "Aveiro",
    'AR' => "Acores (Azores)",
    'BJ' => "Beja",
    'BA' => "Braga",
    'BN' => "Braganca",
    'CB' => "Castelo Branco",
    'CO' => "Coimbra",
    'EV' => "Evora",
    'FA' => "Faro",
    'GU' => "Guarda",
    'LE' => "Leiria",
    'LI' => "Lisboa",
    'MA' => "Madeira",
    'PG' => "Portalegre",
    'PO' => "Porto",
    'SA' => "Santarem",
    'SE' => "Setubal",
    'VC' => "Viana do Castelo",
    'VR' => "Vila Real",
    'VS' => "Viseu");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pt() {
  return array(
    'minlng' => -31.25925,
    'minlat' => 32.335633,
    'maxlng' => -6.25515,
    'maxlat' => 42.028267,
  );
}
