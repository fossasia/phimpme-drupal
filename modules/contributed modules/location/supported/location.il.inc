<?php

// Israel

function location_province_list_il() {
  return array('C' => "Central",
    'H' => "Haifa",
    'J' => "Jerusalem",
    'N' => "Northern",
    'S' => "Southern",
    'T' => "Tel Aviv");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_il() {
  return array(
    'minlng' => 34.177243,
    'minlat' => 29.519233,
    'maxlng' => 35.8844,
    'maxlat' => 33.314233,
  );
}
