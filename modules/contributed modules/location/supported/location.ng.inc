<?php

// Nigeria

function location_province_list_ng() {
  return array('AB' => "Abia",
    'AD' => "Adamawa",
    'AK' => "Akwa Ibom",
    'AN' => "Anambra",
    'BC' => "Bauchi",
    'BY' => "Bayelsa",
    'BN' => "Benue",
    'BO' => "Borno",
    'CR' => "Cross River",
    'DE' => "Delta",
    'EB' => "Ebonyi",
    'ED' => "Edo",
    'EK' => "Ekiti",
    'EN' => "Enugu",
    'CT' => "Federal Capital Territory",
    'GO' => "Gombe",
    'IM' => "Imo",
    'JI' => "Jigawa",
    'KD' => "Kaduna",
    'KN' => "Kano",
    'KT' => "Katsina",
    'KE' => "Kebbi",
    'KO' => "Kogi",
    'KW' => "Kwara",
    'LA' => "Lagos",
    'NA' => "Nassarawa",
    'NI' => "Niger",
    'OG' => "Ogun",
    'ONG' => "Ondo",
    'OS' => "Osun",
    'OY' => "Oyo",
    'PL' => "Plateau",
    'RI' => "Rivers",
    'SO' => "Sokoto",
    'TA' => "Taraba",
    'YO' => "Yobe",
    'ZA' => "Zamfara");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ng() {
  return array(
    'minlng' => 2.64745,
    'minlat' => 4.226367,
    'maxlng' => 14.7131,
    'maxlat' => 13.807067,
  );
}
