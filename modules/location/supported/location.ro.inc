<?php

// Romania

function location_province_list_ro() {
  return array(
    'AB' => 'Alba',
    'AR' => 'Arad',
    'AG' => 'Argeș',
    'BC' => 'Bacău',
    'BH' => 'Bihor',
    'BN' => 'Bistrița-Năsăud',
    'BT' => 'Botoșani',
    'BR' => 'Brăila',
    'BV' => 'Brașov',
    'B'  => 'București',
    'BZ' => 'Buzău',
    'CL' => 'Călărași',
    'CS' => 'Caraș-Severin',
    'CJ' => 'Cluj',
    'CT' => 'Constanța',
    'CV' => 'Covasna',
    'DB' => 'Dâmbovița',
    'DJ' => 'Dolj',
    'GL' => 'Galați',
    'GJ' => 'Gorj',
    'GR' => 'Giurgiu',
    'HR' => 'Harghita',
    'HD' => 'Hunedoara',
    'IL' => 'Ialomița',
    'IS' => 'Iași',
    'IF' => 'Ilfov',
    'MM' => 'Maramureș',
    'MH' => 'Mehedinți',
    'MS' => 'Mureș',
    'NT' => 'Neamț',
    'OT' => 'Olt',
    'PH' => 'Prahova',
    'SJ' => 'Sălaj',
    'SM' => 'Satu Mare',
    'SB' => 'Sibiu',
    'SV' => 'Suceava',
    'TR' => 'Teleorman',
    'TM' => 'Timiș',
    'TL' => 'Tulcea',
    'VS' => 'Vaslui',
    'VL' => 'Vâlcea',
    'VN' => 'Vrancea');
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ro() {
  return array(
    'minlng' => 20.2073,
    'minlat' => 43.7213,
    'maxlng' => 30.20235,
    'maxlat' => 48.21,
  );
}
