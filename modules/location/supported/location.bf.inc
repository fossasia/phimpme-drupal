<?php

// Burkina Faso

function location_province_list_bf() {
  return array('BAL' => "Bale",
    'BAM' => "Bam",
    'BAN' => "Banwa",
    'BAZ' => "Bazega",
    'BOR' => "Bougouriba",
    'BLG' => "Boulgou",
    'BOK' => "Boulkiemde",
    'COM' => "Comoe",
    'GAN' => "Ganzourgou",
    'GNA' => "Gnagna",
    'GOU' => "Gourma",
    'HOU' => "Houet",
    'IOA' => "Ioba",
    'KAD' => "Kadiogo",
    'KEN' => "Kenedougou",
    'KOD' => "Komondjari",
    'KOP' => "Kompienga",
    'KOS' => "Kossi",
    'KOL' => "Koulpelogo",
    'KOT' => "Kouritenga",
    'KOW' => "Kourweogo",
    'LER' => "Leraba",
    'LOR' => "Loroum",
    'MOU' => "Mouhoun",
    'NAH' => "Nahouri",
    'NAM' => "Namentenga",
    'NAY' => "Nayala",
    'NOU' => "Noumbiel",
    'OUB' => "Oubritenga",
    'OUD' => "Oudalan",
    'PAS' => "Passore",
    'PON' => "Poni",
    'SAG' => "Sanguie",
    'SAM' => "Sanmatenga",
    'SEN' => "Seno",
    'SIS' => "Sissili",
    'SOM' => "Soum",
    'SOR' => "Sourou",
    'TAP' => "Tapoa",
    'TUY' => "Tuy",
    'YAG' => "Yagha",
    'YAT' => "Yatenga",
    'ZIR' => "Ziro",
    'ZOD' => "Zondoma",
    'ZOW' => "Zoundweogo");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bf() {
  return array(
    'minlng' => -5.5285,
    'minlat' => 9.4826,
    'maxlng' => 2.2205,
    'maxlat' => 15.1109,
  );
}
