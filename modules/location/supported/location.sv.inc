<?php

// El Salvador

function location_province_list_sv() {
  return array('AH' => "Ahuachapan",
    'CA' => "Cabanas",
    'CH' => "Chalatenango",
    'CU' => "Cuscatlan",
    'LB' => "La Libertad",
    'PZ' => "La Paz",
    'UN' => "La Union",
    'MO' => "Morazan",
    'SM' => "San Miguel",
    'SS' => "San Salvador",
    'SA' => "Santa Ana",
    'SV' => "San Vicente",
    'SO' => "Sonsonate",
    'US' => "Usulutan");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sv() {
  return array(
    'minlng' => -90.12705,
    'minlat' => 13.0919,
    'maxlng' => -87.6757,
    'maxlat' => 14.478067,
  );
}
