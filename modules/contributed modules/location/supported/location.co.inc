<?php

// Colombia

function location_province_list_co() {
  return array('AMZ' => "Amazonas",
    'ANT' => "Antioquia",
    'ARA' => "Arauca",
    'ATL' => "Atlantico",
    'BOL' => "Bolivar",
    'BOY' => "Boyaca",
    'CAL' => "Caldas",
    'CAQ' => "Caqueta",
    'CAS' => "Casanare",
    'CAU' => "Cauca",
    'CES' => "Cesar",
    'CHO' => "Choco",
    'COR' => "Cordoba",
    'CAM' => "Cundinamarca",
    'GNA' => "Guainia",
    'GJR' => "Guajira",
    'GVR' => "Guaviare",
    'HUI' => "Huila",
    'MAG' => "Magdalena",
    'MET' => "Meta",
    'NAR' => "Narino",
    'NDS' => "Norte de Santander",
    'PUT' => "Putumayo",
    'QUI' => "Quindio",
    'RIS' => "Risaralda",
    'SAP' => "San Andres y Providencia",
    'SAN' => "Santander",
    'SUC' => "Sucre",
    'TOL' => "Tolima",
    'VDC' => "Valle del Cauca",
    'VAU' => "Vaupes",
    'VIC' => "Vichada",
    'BDC' => "Bogota D.C.");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_co() {
  return array(
    'minlng' => -81.8458,
    'minlat' => -4.0608,
    'maxlng' => -66.8912,
    'maxlat' => 15.886067,
  );
}
