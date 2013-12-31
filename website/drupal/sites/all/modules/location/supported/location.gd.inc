<?php

// Grenada

function location_province_list_gd() {
  return array('A' => "Saint Andrew",
    'D' => "Saint David",
    'G' => "Saint George",
    'J' => "Saint John",
    'M' => "Saint Mark",
    'P' => "Saint Patrick",
    'C' => "Carriacou",
    'Q' => "Petit Martinique");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_gd() {
  return array(
    'minlng' => -61.839279,
    'minlat' => 11.956733,
    'maxlng' => -61.59945,
    'maxlat' => 12.179133,
  );
}
