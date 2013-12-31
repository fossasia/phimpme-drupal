<?php

// Bolivia

function location_province_list_bo() {
  return array('BEN' => "Departmento Beni",
    'CHU' => "Departmento Chuquisaca",
    'COC' => "Departmento Cochabamba",
    'LPZ' => "Departmento La Paz",
    'ORU' => "Departmento Oruro",
    'PAN' => "Departmento Pando",
    'POT' => "Departmento Potosi",
    'SCZ' => "Departmento Santa Cruz",
    'TAR' => "Departmento Tarija");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bo() {
  return array(
    'minlng' => -69.706,
    'minlat' => -22.762867,
    'maxlng' => -57.49985,
    'maxlat' => -9.6598,
  );
}
