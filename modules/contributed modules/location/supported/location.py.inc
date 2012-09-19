<?php

// Paraguay

function location_province_list_py() {
  return array('AG' => "Alto Paraguay",
    'AN' => "Alto Parana",
    'AM' => "Amambay",
    'AS' => "Asuncion",
    'BO' => "Boqueron",
    'CG' => "Caaguazu",
    'CZ' => "Caazapa",
    'CN' => "Canindeyu",
    'CE' => "Central",
    'CC' => "Concepcion",
    'CD' => "Cordillera",
    'GU' => "Guaira",
    'IT' => "Itapua",
    'MI' => "Misiones",
    'NE' => "Neembucu",
    'PA' => "Paraguari",
    'PH' => "Presidente Hayes",
    'SP' => "San Pedro");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_py() {
  return array(
    'minlng' => -62.7526,
    'minlat' => -27.5863,
    'maxlng' => -54.24475,
    'maxlat' => -19.248333,
  );
}
