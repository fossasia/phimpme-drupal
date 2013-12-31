<?php

// Taiwan, Province of China

function location_province_list_tw() {
  return array(
    'CH' => "Chang-hua",
    'CI' => "Chia-i",
    'HS' => "Hsin-chu",
    'HL' => "Hua-lien",
    'IL' => "I-lan",
    'KH' => "Kao-hsiung county",
    'KM' => "Kin-men",
    'LC' => "Lien-chiang",
    'ML' => "Miao-li",
    'NT' => "Nan-t'ou",
    'PH' => "P'eng-hu",
    'PT' => "P'ing-tung",
    'TG' => "T'ai-chung",
    'TA' => "T'ai-nan",
    'TP' => "T'ai-pei county",
    'TT' => "T'ai-tung",
    'TY' => "T'ao-yuan",
    'YL' => "Yun-lin",
    'CC' => "Chia-i city",
    'CL' => "Chi-lung",
    'HC' => "Hsin-chu",
    'TH' => "T'ai-chung",
    'TN' => "T'ai-nan",
    'KC' => "Kao-hsiung city",
    'TC' => "T'ai-pei city",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tw() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 118.279839,
    'minlat' => 21.9044005,
    'maxlng' => 122.005606,
    'maxlat' => 25.2871914,
  );
}
