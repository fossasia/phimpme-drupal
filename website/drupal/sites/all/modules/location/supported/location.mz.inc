<?php

// Mozambique

function location_province_list_mz() {
  return array('CD' => "Cabo Delgado",
    'GZ' => "Gaza",
    'IN' => "Inhambane",
    'MN' => "Manica",
    'MC' => "Maputo (city)",
    'MP' => "Maputo",
    'NA' => "Nampula",
    'NI' => "Niassa",
    'SO' => "Sofala",
    'TE' => "Tete",
    'ZA' => "Zambezia");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mz() {
  return array(
    'minlng' => 30.0867,
    'minlat' => -26.767533,
    'maxlng' => 40.87665,
    'maxlat' => -10.568067,
  );
}
