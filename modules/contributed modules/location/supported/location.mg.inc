<?php

// Madagascar

function location_province_list_mg() {
  return array('AN' => "Antananarivo province",
    'AS' => "Antsiranana province",
    'FN' => "Fianarantsoa province",
    'MJ' => "Mahajanga province",
    'TM' => "Toamasina province",
    'TL' => "Toliara province");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_mg() {
  return array(
    'minlng' => 42.67535,
    'minlat' => -25.580067,
    'maxlng' => 50.4287,
    'maxlat' => -12.0137,
  );
}
