<?php

// Vietnam

function location_province_list_vn() {
  return array('AG' => "An Giang",
    'BG' => "Bac Giang",
    'BK' => "Bac Kan",
    'BL' => "Bac Lieu",
    'BC' => "Bac Ninh",
    'BR' => "Ba Ria-Vung Tau",
    'BN' => "Ben Tre",
    'BH' => "Binh Dinh",
    'BU' => "Binh Duong",
    'BP' => "Binh Phuoc",
    'BT' => "Binh Thuan",
    'CM' => "Ca Mau",
    'CT' => "Can Tho",
    'CB' => "Cao Bang",
    'DL' => "Dak Lak",
    'DG' => "Dak Nong",
    'DN' => "Da Nang",
    'DB' => "Dien Bien",
    'DI' => "Dong Nai",
    'DT' => "Dong Thap",
    'GL' => "Gia Lai",
    'HG' => "Ha Giang",
    'HD' => "Hai Duong",
    'HP' => "Hai Phong",
    'HM' => "Ha Nam",
    'HI' => "Ha Noi",
    'HT' => "Ha Tay",
    'HH' => "Ha Tinh",
    'HB' => "Hoa Binh",
    'HC' => "Ho Chin Minh",
    'HU' => "Hau Giang",
    'HY' => "Hung Yen");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_vn() {
  return array(
    'minlng' => 102.2111,
    'minlat' => 8.589833,
    'maxlng' => 109.412,
    'maxlat' => 23.4265,
  );
}
