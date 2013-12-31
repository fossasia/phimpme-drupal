<?php

// Spain

function location_province_list_es() {
  return array('AL' => "Alava",
    'AB' => "Albacete",
    'AC' => "Alicante",
    'AM' => "Almeria",
    'AS' => "Asturias",
    'AV' => "Avila",
    'BJ' => "Badajoz",
    'IB' => "Illes Balears",
    'BA' => "Barcelona",
    'BU' => "Burgos",
    'CC' => "Caceres",
    'CZ' => "Cadiz",
    'CT' => "Cantabria",
    'CL' => "Castellon",
    'CR' => "Ciudad Real",
    'CD' => "Cordoba",
    'CA' => "A Coruna",
    'CU' => "Cuenca",
    'GI' => "Girona",
    'GD' => "Granada",
    'GJ' => "Guadalajara",
    'GP' => "Guipuzcoa",
    'HL' => "Huelva",
    'HS' => "Huesca",
    'JN' => "Jaen",
    'RJ' => "La Rioja",
    'LE' => "Leon",
    'LL' => "Lleida",
    'LG' => "Lugo",
    'MD' => "Madrid",
    'ML' => "Malaga",
    'MU' => "Murcia",
    'NV' => "Navarra",
    'OU' => "Ourense",
    'PL' => "Palencia",
    'PM' => "Las Palmas",
    'PO' => "Pontevedra",
    'SL' => "Salamanca",
    'SC' => "Santa Cruz de Tererife",
    'SG' => "Segovia",
    'SV' => "Sevilla",
    'SO' => "Soria",
    'TA' => "Tarragona",
    'TE' => "Teruel",
    'TO' => "Toledo",
    'VC' => "Valencia",
    'VD' => "Valladolid",
    'VZ' => "Vizcaya",
    'ZM' => "Zamora",
    'ZR' => "Zaragoza");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_es() {
  return array(
    'minlng' => -18.26745,
    'minlat' => 27.5218,
    'maxlng' => 4.2802,
    'maxlat' => 43.739867,
  );
}
