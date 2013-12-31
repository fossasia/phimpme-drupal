<?php

// Turks and Caicos Islands

function location_province_list_tc() {
  return array('AC' => "Ambergris Cays",
    'DC' => "Dellis Cay",
    'FC' => "French Cay",
    'LW' => "Little Water Cay",
    'RC' => "Parrot Cay",
    'PN' => "Pine Cay",
    'SL' => "Salt Cay",
    'GT' => "Grand Turk",
    'SC' => "South Caicos",
    'EC' => "East Caicos",
    'MC' => "Middle Caicos",
    'NC' => "North Caicos",
    'PR' => "Providenciales",
    'WC' => "West Caicos");
}

/**
 * Returns minimum and maximum latitude and longitude needed to create a bounding box.
 */
function location_bounds_tc() {
  return array(
    'minlng' => -72.42375,
    'minlat' => 21.2068,
    'maxlng' => -70.9723,
    'maxlat' => 21.9866,
  );
}
