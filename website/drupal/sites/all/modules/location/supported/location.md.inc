<?php

// Moldova, Republic of

function location_province_list_md() {
  return array(
    'GA' => "Gagauzia",
    'CU' => "Chisinau",
    'BA' => "Balti",
    'CA' => "Cahul",
    'ED' => "Edinet",
    'LA' => "Lapusna",
    'OR' => "Orhei",
    'SO' => "Soroca",
    'TI' => "Tighina",
    'UN' => "Ungheni",
    'SN' => "Stînga Nistrului",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_md() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 26.6178890,
    'minlat' => 45.4617739,
    'maxlng' => 30.1315763,
    'maxlat' => 48.4860338,
  );
}
