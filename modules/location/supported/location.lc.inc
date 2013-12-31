<?php

// Saint Lucia

function location_province_list_lc() {
  return array('AR' => "Anse-la-Raye",
    'CA' => "Castries",
    'CH' => "Choiseul",
    'DA' => "Dauphin",
    'DE' => "Dennery",
    'GI' => "Gros-Islet",
    'LA' => "Laborie",
    'MI' => "Micoud",
    'PR' => "Praslin",
    'SO' => "Soufriere",
    'VF' => "Vieux-Fort");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_lc() {
  return array(
    'minlng' => -61.1531,
    'minlat' => 13.692333,
    'maxlng' => -60.8714,
    'maxlat' => 14.0824,
  );
}
