<?php

// Egypt

function location_province_list_eg() {
  return array('DHY' => "Ad Daqahliyah",
    'BAM' => "Al Bahr al Ahmar",
    'BHY' => "Al Buhayrah",
    'FYM' => "Al Fayyum",
    'GBY' => "Al Gharbiyah",
    'IDR' => "Al Iskandariyah",
    'IML' => "Al Isma'iliyah",
    'JZH' => "Al Jizah",
    'MFY' => "Al Minufiyah",
    'MNY' => "Al Minya",
    'QHR' => "Al Qahirah",
    'QLY' => "Al Qalyubiyah",
    'WJD' => "Al Wadi al Jadid",
    'SHQ' => "Ash Sharqiyah",
    'SWY' => "As Suways",
    'ASW' => "Aswan",
    'ASY' => "Asyut",
    'BSW' => "Bani Suwayf",
    'BSD' => "Bur Sa'id",
    'DMY' => "Dumyat",
    'JNS' => "Janub Sina'",
    'KSH' => "Kafr ash Shaykh",
    'MAT' => "Matruh",
    'QIN' => "Qina",
    'SHS' => "Shamal Sina'",
    'SUH' => "Suhaj");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_eg() {
  return array(
    'minlng' => 24.78595,
    'minlat' => 22.008123,
    'maxlng' => 36.88215,
    'maxlat' => 31.746867,
  );
}
