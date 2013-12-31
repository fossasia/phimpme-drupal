<?php

// Finland

function location_province_list_fi() {
  return array('ES' => "Etela-Suomen laani",
    'LS' => "Lansi-Suomen laani",
    'IS' => "Ita-Suomen laani",
    'OU' => "Oulun laani",
    'LL' => "Lapin laani",
    'AH' => "Ahvenanmaan laani");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_fi() {
  return array(
    'minlng' => 19.45165,
    'minlat' => 59.806267,
    'maxlng' => 31.42675,
    'maxlat' => 70.137467,
  );
}
