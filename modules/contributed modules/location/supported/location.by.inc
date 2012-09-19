<?php

// Belarus

function location_province_list_by() {
  return array('BR' => "Brest voblast",
    'HO' => "Homyel voblast",
    'HR' => "Hrodna voblast",
    'MA' => "Mahilyow voblast",
    'MI' => "Minsk voblast",
    'VI' => "Vitsebsk voblast");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_by() {
  return array(
    'minlng' => 23.1605,
    'minlat' => 51.331367,
    'maxlng' => 32.6273,
    'maxlat' => 56.223167,
  );
}
