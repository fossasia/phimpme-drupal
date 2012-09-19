<?php

// Guatemala

function location_province_list_gt() {
  return array('AV' => "Alta Verapaz",
    'BV' => "Baja Verapaz",
    'CM' => "Chimaltenango",
    'CQ' => "Chiquimula",
    'PE' => "El Peten",
    'PR' => "El Progreso",
    'QC' => "El Quiche",
    'ES' => "Escuintla",
    'GU' => "Guatemala",
    'HU' => "Huehuetenango",
    'IZ' => "Izabal",
    'JA' => "Jalapa",
    'JU' => "Jutiapa",
    'QZ' => "Quetzaltenango",
    'RE' => "Retalhuleu",
    'ST' => "Sacatepequez",
    'SM' => "San Marcos",
    'SR' => "Santa Rosa",
    'SO' => "Solola",
    'SU' => "Suchitepequez",
    'TO' => "Totonicapan",
    'ZA' => "Zacapa");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gt() {
  return array(
    'minlng' => -92.28845,
    'minlat' => 13.7392,
    'maxlng' => -88.23725,
    'maxlat' => 17.783133,
  );
}
