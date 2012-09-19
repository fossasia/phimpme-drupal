<?php

// Hungary

function location_province_list_hu() {
  return array('BU' => "Budapest",
    'BC' => "Bekescsaba",
    'DB' => "Debrecen",
    'DJ' => "Dunaujvaros",
    'EG' => "Eger",
    'GY' => "Gyor",
    'HM' => "Hodmezovasarhely",
    'KP' => "Kaposvar",
    'KC' => "Kecskemet",
    'MK' => "Miskolc",
    'NK' => "Nagykanizsa",
    'NY' => "Nyiregyhaza",
    'PC' => "Pecs",
    'SO' => "Sopron",
    'SG' => "Szeged",
    'SK' => "Szekesfehervar",
    'SL' => "Szolnok",
    'SB' => "Szombathely",
    'TB' => "Tatabanya",
    'ZG' => "Zalaegerszeg",
    'BK' => "Bacs-Kiskun",
    'BR' => "Baranya",
    'BS' => "Bekes",
    'BA' => "Borsod-Abauj-Zemplen",
    'CG' => "Csongrad",
    'FJ' => "Fejer",
    'GM' => "Gyor-Moson-Sopron",
    'HB' => "Hajdu-Bihar",
    'HV' => "Heves",
    'JN' => "Jasz-Nagykun-Szolnok",
    'KE' => "Komarom-Esztergom",
    'NG' => "Nograd",
    'PE' => "Pest",
    'SM' => "Somogy",
    'SS' => "Szabolcs-Szatmar-Bereg",
    'TO' => "Tolna",
    'VA' => "Vas",
    'VZ' => "Veszprem",
    'ZA' => "Zala");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_hu() {
  return array(
    'minlng' => 16.065,
    'minlat' => 45.768167,
    'maxlng' => 22.88665,
    'maxlat' => 48.577,
  );
}
