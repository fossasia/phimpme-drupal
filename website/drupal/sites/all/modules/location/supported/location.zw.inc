<?php

// Zimbabwe

function location_province_list_zw() {
  return array('BU' => "Bulawayo (city)",
    'HA' => "Harare (city)",
    'ML' => "Manicaland",
    'MC' => "Mashonaland Central",
    'ME' => "Mashonaland East",
    'MW' => "Mashonaland West",
    'MV' => "Masvingo",
    'MN' => "Matabeleland North",
    'MS' => "Matabeleland South",
    'MD' => "Midlands");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_zw() {
  return array(
    'minlng' => 25.23535,
    'minlat' => -22.465267,
    'maxlng' => 33.06475,
    'maxlat' => -15.691267,
  );
}
