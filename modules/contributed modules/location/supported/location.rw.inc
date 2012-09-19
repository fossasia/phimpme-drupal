<?php

// Rwanda

function location_province_list_rw() {
  return array('BU' => "Butare",
    'BY' => "Byumba",
    'CY' => "Cyangugu",
    'GK' => "Gikongoro",
    'GS' => "Gisenyi",
    'GT' => "Gitarama",
    'KG' => "Kibungo",
    'KY' => "Kibuye",
    'KR' => "Kigali Rurale",
    'KV' => "Kigali-ville",
    'RU' => "Ruhengeri",
    'UM' => "Umutara");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_rw() {
  return array(
    'minlng' => 28.8737,
    'minlat' => -2.890967,
    'maxlng' => 30.87085,
    'maxlat' => -1.116267,
  );
}
