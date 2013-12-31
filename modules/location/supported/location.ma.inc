<?php

// Morocco

function location_province_list_ma() {
  return array('AGD' => "Agadir",
    'HOC' => "Al Hoceima",
    'AZI' => "Azilal",
    'BME' => "Beni Mellal",
    'BSL' => "Ben Slimane",
    'BLM' => "Boulemane",
    'CBL' => "Casablanca",
    'CHA' => "Chaouen",
    'EJA' => "El Jadida",
    'EKS' => "El Kelaa des Sraghna",
    'ERA' => "Er Rachidia",
    'ESS' => "Essaouira",
    'FES' => "Fes",
    'FIG' => "Figuig",
    'GLM' => "Guelmim",
    'IFR' => "Ifrane",
    'KEN' => "Kenitra",
    'KHM' => "Khemisset",
    'KHN' => "Khenifra",
    'KHO' => "Khouribga",
    'LYN' => "Laayoune",
    'LAR' => "Larache",
    'MRK' => "Marrakech",
    'MKN' => "Meknes",
    'NAD' => "Nador",
    'ORZ' => "Ouarzazate",
    'OUJ' => "Oujda",
    'RSA' => "Rabat-Sale",
    'SAF' => "Safi",
    'SET' => "Settat",
    'SKA' => "Sidi Kacem",
    'TGR' => "Tangier",
    'TAN' => "Tan-Tan",
    'TAO' => "Taounate",
    'TRD' => "Taroudannt",
    'TAT' => "Tata",
    'TAZ' => "Taza",
    'TET' => "Tetouan",
    'TIZ' => "Tiznit",
    'ADK' => "Ad Dakhla",
    'BJD' => "Boujdour",
    'ESM' => "Es Smara");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ma() {
  return array(
    'minlng' => -13.26225,
    'minlat' => 27.6511,
    'maxlng' => -1.1251,
    'maxlat' => 35.87,
  );
}
