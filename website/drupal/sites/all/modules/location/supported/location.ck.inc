<?php

// Cook Islands

function location_province_list_ck() {
  return array('PU' => "Pukapuka",
    'RK' => "Rakahanga",
    'MK' => "Manihiki",
    'PE' => "Penrhyn",
    'NI' => "Nassau Island",
    'SU' => "Surwarrow",
    'PA' => "Palmerston",
    'AI' => "Aitutaki",
    'MA' => "Manuae",
    'TA' => "Takutea",
    'MT' => "Mitiaro",
    'AT' => "Atiu",
    'MU' => "Mauke",
    'RR' => "Rarotonga",
    'MG' => "Mangaia");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ck() {
  return array(
    'minlng' => -165.7749,
    'minlat' => -11.650367,
    'maxlng' => -165.37025,
    'maxlat' => -10.815933,
  );
}
