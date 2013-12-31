<?php

// French Polynesia

function location_province_list_pf() {
  return array('M' => "Archipel des Marquises",
    'T' => "Archipel des Tuamotu",
    'I' => "Archipel des Tubuai",
    'V' => "Iles du Vent",
    'S' => "Iles Sous-le-Vent");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_pf() {
  return array(
    'minlng' => -150.8061,
    'minlat' => -18.0048,
    'maxlng' => -148.0971,
    'maxlat' => -17.036567,
  );
}
