<?php

// Belize

function location_province_list_bz() {
  return array('BZ' => "Belize District",
    'CY' => "Cayo District",
    'CR' => "Corozal District",
    'OW' => "Orange Walk District",
    'SC' => "Stann Creek District",
    'TO' => "Toledo District");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bz() {
  return array(
    'minlng' => -89.19635,
    'minlat' => 15.799133,
    'maxlng' => -87.64955,
    'maxlat' => 18.4742,
  );
}
