<?php

// Tokelau

function location_province_list_tk() {
  return array(
    'A' => "Atafu",
    'F' => "Fakaofo",
    'N' => "Nukunonu",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tk() {
  // Note: I determined this bounding box manually using a visualization tool. --Bdragon
  return array(
    'minlng' => -172.68311,
    'minlat' => -9.50595,
    'maxlng' => -171.09833,
    'maxlat' => -8.43977,
  );
}

