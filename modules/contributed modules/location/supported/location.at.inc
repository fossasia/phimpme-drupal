<?php

// Austria

function location_province_list_at() {
  return array(
    'BUR' => "Burgenland",
    'KAR' => "Kärnten",
    'NOS' => "Niederösterreich",
    'OOS' => "Oberösterreich",
    'SAL' => "Salzburg",
    'STE' => "Steiermark",
    'TIR' => "Tirol",
    'VOR' => "Vorarlberg",
    'WIE' => "Wien",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_at() {
  return array(
    'minlng' => 9.47885,
    'minlat' => 46.427367,
    'maxlng' => 17.15825,
    'maxlat' => 49.010433,
  );
}
