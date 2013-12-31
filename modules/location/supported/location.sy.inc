<?php

// Syrian Arab Republic

function location_province_list_sy() {
  return array('HA' => "Al Hasakah",
    'LA' => "Al Ladhiqiyah",
    'QU' => "Al Qunaytirah",
    'RQ' => "Ar Raqqah",
    'SU' => "As Suwayda",
    'DA' => "Dara",
    'DZ' => "Dayr az Zawr",
    'DI' => "Dimashq",
    'HL' => "Halab",
    'HM' => "Hamah",
    'HI' => "Hims",
    'ID' => "Idlib",
    'RD' => "Rif Dimashq",
    'TA' => "Tartus");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sy() {
  return array(
    'minlng' => 35.6348,
    'minlat' => 32.3535,
    'maxlng' => 42.21705,
    'maxlat' => 37.3552,
  );
}
