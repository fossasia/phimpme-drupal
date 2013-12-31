<?php

// Korea, Republic Of (South Korea)

function location_province_list_kr() {
  return array(
    '11' => "Seoul Special City",
    '26' => "Busan Metropolitan City",
    '27' => "Daegu Metropolitan City",
    '28' => "Incheon Metropolitan City",
    '29' => "Gwangju Metropolitan City",
    '30' => "Daejeon Metropolitan City",
    '31' => "Ulsan Metropolitan City",
    '41' => "Gyeonggi-do",
    '42' => "Gangwon-do",
    '43' => "Chungcheongbuk-do",
    '44' => "Chungcheongnam-do",
    '45' => "Jeollabuk-do",
    '46' => "Jeollanam-do",
    '47' => "Gyeongsangbuk-do",
    '48' => "Gyeongsangnam-do",
    '49' => "Jeju-do",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_kr() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => 124.613974,
    'minlat' => 33.1973029,
    'maxlng' => 130.920928,
    'maxlat' => 38.5783435,
  );
}
