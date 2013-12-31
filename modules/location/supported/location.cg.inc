<?php

// Congo

function location_province_list_cg() {
  return array(
    'BO' => "Bouenza",
    'BR' => "Brazzaville",
    'CU' => "Cuvette",
    'CO' => "Cuvette-Ouest",
    'KO' => "Kouilou",
    'LE' => "Lekoumou",
    'LI' => "Likouala",
    'NI' => "Niari",
    'PL' => "Plateaux",
    'PO' => "Pool",
    'SA' => "Sangha",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cg() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 11.1275683,
    'minlat' => -5.0134396,
    'maxlng' => 18.6424068,
    'maxlat' => 3.70827606,
  );
}
