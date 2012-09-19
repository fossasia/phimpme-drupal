<?php

// Northern Mariana Islands

function location_province_list_mp() {
  return array('N' => "Northern Islands",
    'R' => "Rota",
    'S' => "Saipan",
    'T' => "Tinian");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mp() {
  return array(
    'minlng' => 143.2344,
    'minlat' => 14.1388,
    'maxlng' => 146.2014,
    'maxlat' => 20.6741,
  );
}
