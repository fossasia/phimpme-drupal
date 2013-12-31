<?php

// France

function location_province_list_fr() {
  return array(
    'A67' => "Bas-Rhin - Alsace",
    'A68' => "Haut-Rhin - Alsace",
    'B24' => "Dordogne - Aquitaine",
    'B33' => "Gironde - Aquitaine",
    'B40' => "Landes - Aquitaine",
    'B47' => "Lot-et-Garonne - Aquitaine",
    'B64' => "Pyrenees-Atlantiques - Aquitaine",
    'B79' => "Deux-Sevres - Aquitaine",
    'C03' => "Allier - Auvergne",
    'C15' => "Cantal - Auvergne",
    'C43' => "Haute-Loire - Auvergne",
    'C63' => "Pu-de-Dme - Auvergne",
    'D21' => "Cote-d'Or - Bourgogne",
    'D58' => "Nievre - Bourgogne",
    'D71' => "Saone-et-Loire - Bourgogne",
    'D89' => "Yonne - Bourgogne",
    'E22' => "Cotes-d'Armor - Bretagne",
    'E29' => "Finistere - Bretagne",
    'E35' => "Ille-et-Vilaine - Bretagne",
    'E56' => "Morbihan - Bretagne",
    'F18' => "Cher - Centre",
    'F28' => "Eure-et-Loir - Centre",
    'F36' => "Indre - Centre",
    'F37' => "Indre-et-Loire - Centre",
    'F41' => "Loir-et-Cher - Centre",
    'F45' => "Loiret - Centre",
    'G08' => "Ardennes - Champagne-Ardenne",
    'G10' => "Aube - Champagne-Ardenne",
    'G51' => "Marne - Champagne-Ardenne",
    'G52' => "Haute-Marne - Champagne-Ardenne",
    'H2A' => "Corse-du-Sud - Corse",
    'H2B' => "Haute-Corse - Corse",
    'I25' => "Doubs - Franche-Comte",
    'I39' => "Jura - Franche-Comte",
    'I70' => "Haute-Saone - Franche-Comte",
    'I90' => "Haute-Saone - Territoire de Belfort",
    'J75' => "Paris - Ile-de-France",
    'J77' => "Seine-et-Marne - Ile-de-France",
    'J78' => "Yvelines - Ile-de-France",
    'J91' => "Essonne - Ile-de-France",
    'J92' => "Hauts-de-Seine - Ile-de-France",
    'J93' => "Seine-Saint-Denis - Ile-de-France",
    'J94' => "Val-de-Marne - Ile-de-France",
    'J95' => "Val-d'Oise - Ile-de-France",
    'U04' => "Alpes-de-Haute-Provence - Provence-Alpes-Cote d'Azur",
    'U05' => "Hautes-Alpes - Provence-Alpes-Cote d'Azur",
    'U06' => "Alpes-Maritimes - Provence-Alpes-Cote d'Azur",
    'U13' => "Bouches-du-Rhone - Provence-Alpes-Cote d'Azur",
    'U83' => "Var - Provence-Alpes-Cote d'Azur",
    'U84' => "Vaucluse - Provence-Alpes-Cote d'Azur",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_fr() {
  return array(
    'minlng' => -176.3726,
    'minlat' => -27.946767,
    'maxlng' => 172.15695,
    'maxlat' => 51.122333,
  );
}
