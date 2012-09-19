<?php

// This file contributed by Avi Alkalay <avi unix sh> to the
// Location Drupal module, based on other files on same folder.
//
// - Fixed Unicode chars and accents
// - Added support for Google Maps on location_map_link_*_providers()
//
// March, 2010
// São Paulo, Brazil

// Brazil

function location_province_list_br() {
  return array(
    'AC' => "Acre",
    'AL' => "Alagoas",
    'AM' => "Amazonas",
    'AP' => "Amapá",
    'BA' => "Bahia",
    'CE' => "Ceara",
    'DF' => "Distrito Federal",
    'ES' => "Espírito Santo",
    'GO' => "Goias",
    'MA' => "Maranhão",
    'MG' => "Minas Gerais",
    'MS' => "Mato Grosso do Sul",
    'MT' => "Mato Grosso",
    'PA' => "Pará",
    'PB' => "Paraíba",
    'PE' => "Pernambuco",
    'PI' => "Piaui",
    'PR' => "Paraná",
    'RJ' => "Rio de Janeiro",
    'RN' => "Rio Grande do Norte",
    'RO' => "Rondônia",
    'RR' => "Roraima",
    'RS' => "Rio Grande do Sul",
    'SC' => "Santa Catarina",
    'SE' => "Sergipe",
    'SP' => "São Paulo",
    'TO' => "Tocantins");
}

function location_map_link_br_providers() {
  return array(
    'google' => array(
      'name' => 'Google Maps',
      'url' => 'http://maps.google.com.br',
      'tos' => 'http://maps.google.com/intl/pt-BR/help/terms_maps.html',
    ),
  );
}

function location_map_link_br_default_providers() {
  return array('google');
}


function location_map_link_br_google($location = array()) {
  $query_params = array();
  $q = NULL;

  foreach (array('street', 'city', 'province', 'postal_code', 'country') as $field) {
    if (isset($location[$field])) {
      $query_params[] = $location[$field];
    }
  }

  if (location_has_coordinates($location)) {
    $q = $location['latitude'] . ' ' . $location['longitude'];
    if ($location[name] != "") {
       $q .= " ($location[name]," . implode(', ', $query_params) . ")";
    } else {
       $q .= ' (' . implode(', ', $query_params) . ')';
    }
  } else if (count($query_params) > 0) {
    $q = implode(", ", $query_params);
    if ($location[name] != "") {
       $q .= " ($location[name])";
    }
  }

  $q = urlencode($q);

  if ($q != NULL) {
    return ('http://maps.google.com.br?q='.$q );
  }
  else {
    return NULL;
  }
}




/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_br() {
  return array(
    'minlng' => -73.97965,
    'minlat' => -33.8089,
    'maxlng' => -28.8133,
    'maxlat' => 5.259233,
  );
}
