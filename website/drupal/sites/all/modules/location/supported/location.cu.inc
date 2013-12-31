<?php

// Cuba

function location_province_list_cu() {
  return array('IJU' => "Isla de la Juventud",
    'PRI' => "Pinar del Rio",
    'LHA' => "La Habana",
    'CLH' => "Ciudad de La Habana",
    'MAT' => "Matanzas",
    'CFU' => "Cienfuegos",
    'VCL' => "Villa Clara",
    'SSP' => "Sancti Spiritus",
    'CAV' => "Ciego de Avila",
    'CAM' => "Camaguey",
    'LTU' => "Las Tunas",
    'GRA' => "Granma",
    'HOL' => "Holguin",
    'SCU' => "Santiago de Cuba",
    'GUA' => "Guantanamo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cu() {
  return array(
    'minlng' => -84.88225,
    'minlat' => 19.796767,
    'maxlng' => -74.10875,
    'maxlat' => 23.263867,
  );
}
