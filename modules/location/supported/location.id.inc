<?php

// Indonesia

function location_province_list_id() {
  return array('BA' => "Bali",
    'BB' => "Bangka-Belitung",
    'BT' => "Banten",
    'BE' => "Bengkulu",
    'GO' => "Gorontalo",
    'JA' => "Jambi",
    'JR' => "Jawa Barat",
    'JT' => "Jawa Tengah",
    'JI' => "Jawa Timur",
    'KB' => "Kalimantan Barat",
    'KS' => "Kalimantan Selatan",
    'KT' => "Kalimantan Tengah",
    'KI' => "Kalimantan Timur",
    'LA' => "Lampung",
    'MA' => "Maluku",
    'MU' => "Maluku Utara",
    'NB' => "Nusa Tenggara Barat",
    'NT' => "Nusa Tenggara Timur",
    'IJ' => "Papua",
    'RI' => "Riau",
    'SN' => "Sulawesi Selatan",
    'ST' => "Sulawesi Tengah",
    'SG' => "Sulawesi Tenggara",
    'SW' => "Sulawesi Utara",
    'SB' => "Sumatera Barat",
    'SL' => "Sumatera Selatan",
    'SU' => "Sumatera Utara",
    'JK' => "Jakarta Raya",
    'AC' => "Aceh",
    'YO' => "Yogyakarta");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_id() {
  return array(
    'minlng' => 95.1087,
    'minlat' => -10.878967,
    'maxlng' => 141.0309,
    'maxlat' => 5.978267,
  );
}
