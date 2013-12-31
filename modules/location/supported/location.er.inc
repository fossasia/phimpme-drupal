<?php

// Eritrea

function location_province_list_er() {
  return array(
    'MA' => "Central (Maekel)",
    'KE' => "Anseba (Keren)",
    'DK' => "Southern Red Sea (Debub-Keih-Bahri)",
    'SK' => "Northern Red Sea (Semien-Keih-Bahri)",
    'DE' => "Southern (Debub)",
    'BR' => "Gash-Barka (Barentu)",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_er() {
  return array(
    'minlng' => 36.3629,
    'minlat' => 12.344214,
    'maxlng' => 43.0529,
    'maxlat' => 18.039133,
  );
}
