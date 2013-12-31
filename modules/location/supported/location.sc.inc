<?php

// Seychelles

function location_province_list_sc() {
  return array('AP' => "Anse aux Pins",
    'AB' => "Anse Boileau",
    'AE' => "Anse Etoile",
    'AL' => "Anse Louis",
    'AR' => "Anse Royale",
    'BL' => "Baie Lazare",
    'BS' => "Baie Sainte Anne",
    'BV' => "Beau Vallon",
    'BA' => "Bel Air",
    'BO' => "Bel Ombre",
    'CA' => "Cascade",
    'GL' => "Glacis",
    'GM' => "Grand' Anse (on Mahe)",
    'GP' => "Grand' Anse (on Praslin)",
    'DG' => "La Digue",
    'RA' => "La Riviere Anglaise",
    'MB' => "Mont Buxton",
    'MF' => "Mont Fleuri",
    'PL' => "Plaisance",
    'PR' => "Pointe La Rue",
    'PG' => "Port Glaud",
    'SL' => "Saint Louis",
    'TA' => "Takamaka");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_sc() {
  return array(
    'minlng' => 46.04125,
    'minlat' => -11.5018,
    'maxlng' => 56.26545,
    'maxlat' => -4.231767,
  );
}
