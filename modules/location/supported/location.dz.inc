<?php

// Algeria

function location_province_list_dz() {
  return array('ADR' => "Adrar",
    'ADE' => "Ain Defla",
    'ATE' => "Ain Temouchent",
    'ALG' => "Alger",
    'ANN' => "Annaba",
    'BAT' => "Batna",
    'BEC' => "Bechar",
    'BEJ' => "Bejaia",
    'BIS' => "Biskra",
    'BLI' => "Blida",
    'BBA' => "Bordj Bou Arreridj",
    'BOA' => "Bouira",
    'BMD' => "Boumerdes",
    'CHL' => "Chlef",
    'CON' => "Constantine",
    'DJE' => "Djelfa",
    'EBA' => "El Bayadh",
    'EOU' => "El Oued",
    'ETA' => "El Tarf",
    'GHA' => "Ghardaia",
    'GUE' => "Guelma",
    'ILL' => "Illizi",
    'JIJ' => "Jijel",
    'KHE' => "Khenchela",
    'LAG' => "Laghouat",
    'MUA' => "Muaskar",
    'MED' => "Medea",
    'MIL' => "Mila",
    'MOS' => "Mostaganem",
    'MSI' => "M'Sila",
    'NAA' => "Naama",
    'ORA' => "Oran",
    'OUA' => "Ouargla",
    'OEB' => "Oum el-Bouaghi",
    'REL' => "Relizane",
    'SAI' => "Saida",
    'SET' => "Setif",
    'SBA' => "Sidi Bel Abbes",
    'SKI' => "Skikda",
    'SAH' => "Souk Ahras",
    'TAM' => "Tamanghasset",
    'TEB' => "Tebessa",
    'TIA' => "Tiaret",
    'TIN' => "Tindouf",
    'TIP' => "Tipaza",
    'TIS' => "Tissemsilt",
    'TOU' => "Tizi Ouzou",
    'TLE' => "Tlemcen");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_dz() {
  return array(
    'minlng' => -8.69675,
    'minlat' => 18.852433,
    'maxlng' => 11.96305,
    'maxlat' => 37.025967,
  );
}
