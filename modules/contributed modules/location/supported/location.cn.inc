<?php

// China

function location_province_list_cn() {
  return array(
    '11' => "Beijing",
    '12' => "Tianjin",
    '13' => "Hebei",
    '14' => "Shanxi",
    '15' => "Nei Mongol",
    '21' => "Liaoning",
    '22' => "Jilin",
    '23' => "Heilongjiang",
    '31' => "Shanghai",
    '32' => "Jiangsu",
    '33' => "Zhejiang",
    '34' => "Anhui",
    '35' => "Fujian",
    '36' => "Jiangxi",
    '37' => "Shandong",
    '41' => "Henan",
    '42' => "Hubei",
    '43' => "Hunan",
    '44' => "Guangdong",
    '45' => "Guangxi",
    '46' => "Hainan",
    '51' => "Sichuan",
    '52' => "Guizhou",
    '53' => "Yunnan",
    '54' => "Xizang (Tibet)",
    '61' => "Shaanxi",
    '62' => "Gansu",
    '63' => "Qinghai",
    '64' => "Ningxia",
    '65' => "Xinjiang",
    '71' => "Taiwan",
    '91' => "Xianggang",
    '92' => "Aomen",
    '97' => "Chongqing",
    '98' => "Gaoxiong",
    '99' => "Taibei",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_cn() {
  return array(
    'minlng' => 73.5403,
    'minlat' => 16.0968,
    'maxlng' => 134.7658,
    'maxlat' => 53.608867,
  );
}
