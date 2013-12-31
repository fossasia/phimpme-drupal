<?php

// Nicaragua

function location_province_list_ni() {
  return array('BO' => "Boaco",
    'CA' => "Carazo",
    'CD' => "Chinandega",
    'CT' => "Chontales",
    'ES' => "Esteli",
    'GR' => "Granada",
    'JI' => "Jinotega",
    'LE' => "Leon",
    'MD' => "Madriz",
    'MN' => "Managua",
    'MS' => "Masaya",
    'MT' => "Matagalpa",
    'NS' => "Nueva Segovia",
    'RV' => "Rivas",
    'SJ' => "Rio San Juan",
    'AN' => "Region Autonoma del Atlantico Norte",
    'AS' => "Region Autonoma del Atlantico Sur");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ni() {
  return array(
    'minlng' => -87.74495,
    'minlat' => 10.7611,
    'maxlng' => -82.6318,
    'maxlat' => 14.9877,
  );
}
