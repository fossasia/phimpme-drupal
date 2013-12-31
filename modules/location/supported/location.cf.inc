<?php

// Central African Republic

function location_province_list_cf() {
  return array('BBA' => "Bamingui-Bangoran",
    'BKO' => "Basse-Kotto",
    'HKO' => "Haute-Kotto",
    'HMB' => "Haut-Mbomou",
    'KEM' => "Kemo",
    'LOB' => "Lobaye",
    'MKD' => "Mambere-Kade�",
    'MBO' => "Mbomou",
    'NMM' => "Nana-Mambere",
    'OMP' => "Ombella-M'Poko",
    'OUK' => "Ouaka",
    'OUH' => "Ouham",
    'OPE' => "Ouham-Pende",
    'VAK' => "Vakaga",
    'NGR' => "Nana-Grebizi",
    'SMB' => "Sangha-Mbaere",
    'BAN' => "Bangui");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cf() {
  return array(
    'minlng' => 14.512,
    'minlat' => 2.727,
    'maxlng' => 27.4848,
    'maxlat' => 11.062967,
  );
}
