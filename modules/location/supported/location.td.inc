<?php

// Chad

function location_province_list_td() {
  return array('BA' => "Batha",
    'BI' => "Biltine",
    'BE' => "Borkou-Ennedi-Tibesti",
    'CB' => "Chari-Baguirmi",
    'GU' => "Guera",
    'KA' => "Kanem",
    'LA' => "Lac",
    'LC' => "Logone Occidental",
    'LR' => "Logone Oriental",
    'MK' => "Mayo-Kebbi",
    'MC' => "Moyen-Chari",
    'OU' => "Ouaddai",
    'SA' => "Salamat",
    'TA' => "Tandjile");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_td() {
  return array(
    'minlng' => 13.5963,
    'minlat' => 7.523867,
    'maxlng' => 24.08065,
    'maxlat' => 23.507333,
  );
}
