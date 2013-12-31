<?php

// Iceland

function location_province_list_is() {
  return array('AL' => "Austurland",
    'HF' => "Hofuoborgarsvaeoi",
    'NE' => "Norourland eystra",
    'NV' => "Norourland vestra",
    'SL' => "Suourland",
    'SN' => "Suournes",
    'VF' => "Vestfiroir",
    'VL' => "Vesturland");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_is() {
  return array(
    'minlng' => -25.3873,
    'minlat' => 63.426633,
    'maxlng' => -13.5579,
    'maxlat' => 67.174067,
  );
}
