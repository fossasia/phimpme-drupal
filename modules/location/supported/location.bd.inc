<?php

// Bangladesh

function location_province_list_bd() {
  return array('BAR' => "Barisal",
    'CHI' => "Chittagong",
    'DHA' => "Dhaka",
    'KHU' => "Khulna",
    'RAJ' => "Rajshahi",
    'SYL' => "Sylhet");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_bd() {
  return array(
    'minlng' => 87.8338,
    'minlat' => 20.794167,
    'maxlng' => 92.67495,
    'maxlat' => 26.491433,
  );
}
