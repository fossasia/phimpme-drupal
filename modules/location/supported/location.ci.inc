<?php

// Cote D'Ivoire

function location_province_list_ci() {
  return array(
    'ABE' => "Abengourou",
    'ABI' => "Abidjan",
    'ABO' => "Aboisso",
    'ADI' => "Adiake",
    'ADZ' => "Adzope",
    'AGB' => "Agboville",
    'AGN' => "Agnibilekrou",
    'ALE' => "Alepe",
    'BOC' => "Bocanda",
    'BAN' => "Bangolo",
    'BEO' => "Beoumi",
    'BIA' => "Biankouma",
    'BDK' => "Bondoukou",
    'BGN' => "Bongouanou",
    'BFL' => "Bouafle",
    'BKE' => "Bouake",
    'BNA' => "Bouna",
    'BDL' => "Boundiali",
    'DKL' => "Dabakala",
    'DBU' => "Dabou",
    'DAL' => "Daloa",
    'DAN' => "Danane",
    'DAO' => "Daoukro",
    'DIM' => "Dimbokro",
    'DIV' => "Divo",
    'DUE' => "Duekoue",
    'FER' => "Ferkessedougou",
    'GAG' => "Gagnoa",
    'GBA' => "Grand-Bassam",
    'GLA' => "Grand-Lahou",
    'GUI' => "Guiglo",
    'ISS' => "Issia",
    'JAC' => "Jacqueville",
    'KAT' => "Katiola",
    'KOR' => "Korhogo",
    'LAK' => "Lakota",
    'MAN' => "Man",
    'MKN' => "Mankono",
    'MBA' => "Mbahiakro",
    'ODI' => "Odienne",
    'OUM' => "Oume",
    'SAK' => "Sakassou",
    'SPE' => "San-Pedro",
    'SAS' => "Sassandra",
    'SEG' => "Seguela",
    'SIN' => "Sinfra",
    'SOU' => "Soubre",
    'TAB' => "Tabou",
    'TAN' => "Tanda",
    'TIE' => "Tiebissou",
    'TIN' => "Tingrela",
    'TIA' => "Tiassale",
    'TBA' => "Touba",
    'TLP' => "Toulepleu",
    'TMD' => "Toumodi",
    'VAV' => "Vavoua",
    'YAM' => "Yamoussoukro",
    'ZUE' => "Zuenoula",
  );
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_ci() {
  // NaturalEarth 10m Admin 0 - Countries (v1.3.0)
  // EPSG:900913
  return array(
    'minlng' => -8.6187198,
    'minlat' => 4.34392201,
    'maxlng' => -2.5063280,
    'maxlat' => 10.7264781,
  );
}
