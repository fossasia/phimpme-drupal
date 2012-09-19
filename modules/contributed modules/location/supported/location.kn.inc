<?php

// Saint Kitts and Nevis

function location_province_list_kn() {
  return array('CCN' => "Christ Church Nichola Town",
    'SAS' => "Saint Anne Sandy Point",
    'SGB' => "Saint George Basseterre",
    'SGG' => "Saint George Gingerland",
    'SJW' => "Saint James Windward",
    'SJC' => "Saint John Capesterre",
    'SJF' => "Saint John Figtree",
    'SMC' => "Saint Mary Cayon",
    'CAP' => "Saint Paul Capesterre",
    'CHA' => "Saint Paul Charlestown",
    'SPB' => "Saint Peter Basseterre",
    'STL' => "Saint Thomas Lowland",
    'STM' => "Saint Thomas Middle Island",
    'TPP' => "Trinity Palmetto Point");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kn() {
  return array(
    'minlng' => -62.92925,
    'minlat' => 17.095533,
    'maxlng' => -62.5697,
    'maxlat' => 17.3875,
  );
}
