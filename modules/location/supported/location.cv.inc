<?php

// Cape Verde

function location_province_list_cv() {
  return array('BV' => "Boa Vista",
    'BR' => "Brava",
    'CS' => "Calheta de Sao Miguel",
    'MA' => "Maio",
    'MO' => "Mosteiros",
    'PA' => "Paul",
    'PN' => "Porto Novo",
    'PR' => "Praia",
    'RG' => "Ribeira Grande",
    'SL' => "Sal",
    'CA' => "Santa Catarina",
    'CR' => "Santa Cruz",
    'SD' => "Sao Domingos",
    'SF' => "Sao Filipe",
    'SN' => "Sao Nicolau",
    'SV' => "Sao Vicente",
    'TA' => "Tarrafal");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cv() {
  return array(
    'minlng' => -25.45625,
    'minlat' => 14.702533,
    'maxlng' => -22.55505,
    'maxlat' => 17.15,
  );
}
