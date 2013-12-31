<?php

// Chile

function location_province_list_cl() {
  return array('AI' => "Aisen del General Carlos Ibanez del Campo (XI)",
    'AN' => "Antofagasta (II)",
    'AR' => "Araucania (IX)",
    'AT' => "Atacama (III)",
    'BI' => "Bio-Bio (VIII)",
    'CO' => "Coquimbo (IV)",
    'LI' => "Libertador General Bernardo O'Higgins (VI)",
    'LL' => "Los Lagos (X)",
    'MA' => "Magallanes (XII)",
    'ML' => "Maule (VII)",
    'RM' => "Region Metropolitana (RM)",
    'TA' => "Tarapaca (I)",
    'VS' => "Valparaiso (V)");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cl() {
  return array(
    'minlng' => -89.1566,
    'minlat' => -56.3359,
    'maxlng' => -66.53165,
    'maxlat' => -17.5216,
  );
}
