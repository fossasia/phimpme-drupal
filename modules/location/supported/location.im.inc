<?php

// Isle of Man
// See http://www.statoids.com/uim.html

function location_province_list_im() {
  return array(
    'AY' => 'Ayre',
    'GR' => 'Garff',
    'GB' => 'Glenfaba',
    'MC' => 'Michael',
    'MD' => 'Middle',
    'RU' => 'Rushen',
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_im() {
  return array(
    'minlng' => -4.874,
    'minlat' => 54.0471,
    'maxlng' => -4.38405,
    'maxlat' => 54.424067,
  );
}
