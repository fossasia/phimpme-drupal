<?php

// Afghanistan

function location_province_list_af() {
  return array('BDS' => "Badakhshan province",
    'BDG' => "Badghis province",
    'BGL' => "Baghlan province",
    'BAL' => "Balkh province",
    'BAM' => "Bamian province",
    'FRA' => "Farah province",
    'FYB' => "Faryab province",
    'GHA' => "Ghazni province",
    'GHO' => "Ghowr province",
    'HEL' => "Helmand province",
    'HER' => "Herat province",
    'JOW' => "Jowzjan province",
    'KAB' => "Kabul province",
    'KAN' => "Kandahar province",
    'KAP' => "Kapisa province",
    'KHO' => "Khost province",
    'KNR' => "Konar province",
    'KDZ' => "Kondoz province",
    'LAG' => "Laghman province",
    'LOW' => "Lowgar province",
    'NAN' => "Nangrahar province",
    'NIM' => "Nimruz province",
    'NUR' => "Nurestan province",
    'ORU' => "Oruzgan province",
    'PIA' => "Paktia province",
    'PKA' => "Paktika province",
    'PAR' => "Parwan province",
    'SAM' => "Samangan province",
    'SAR' => "Sar-e Pol province",
    'TAK' => "Takhar province",
    'WAR' => "Wardak province",
    'ZAB' => "Zabol province");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_af() {
  return array(
    'minlng' => 60.5054,
    'minlat' => 29.612733,
    'maxlng' => 74.93195,
    'maxlat' => 38.538967,
  );
}
