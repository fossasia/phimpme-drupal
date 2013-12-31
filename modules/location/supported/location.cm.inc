<?php

// Cameroon

function location_province_list_cm() {
  return array('ADA' => "Adamawa Province (Adamaoua)",
    'CEN' => "Centre Province",
    'EST' => "East Province (Est)",
    'EXN' => "Extreme North Province (Extr�me-Nord)",
    'LIT' => "Littoral Province",
    'NOR' => "North Province (Nord)",
    'NOT' => "Northwest Province (Nord-Ouest)",
    'OUE' => "West Province (Ouest)",
    'SUD' => "South Province (Sud)",
    'SOU' => "Southwest Province (Sud-Ouest).");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cm() {
  return array(
    'minlng' => 8.34605,
    'minlat' => 1.648433,
    'maxlng' => 16.32655,
    'maxlat' => 13.158733,
  );
}
