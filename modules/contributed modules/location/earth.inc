<?php

/**
 * License clarification:
 *
 * On Feb 13, 2005, in message <Pine.LNX.4.58.0502131827510.5072@server1.LFW.org>,
 * the creator of these routines, Ka-Ping Yee, authorized these routines to be
 * distributed under the GPL.
 */

/**
 * @file
 * Trigonometry for calculating geographical distances.
 * All function arguments and return values measure distances in metres
 * and angles in degrees.  The ellipsoid model is from the WGS-84 datum.
 * Ka-Ping Yee, 2003-08-11
 */

//$earth_radius_semimajor = 6378137.0;
//$earth_flattening = 1/298.257223563;
//$earth_radius_semiminor = $earth_radius_semimajor * (1 - $earth_flattening);
//$earth_eccentricity_sq = 2*$earth_flattening - pow($earth_flattening, 2);

// I don't know what's up: PHP is hating on my global variables (commented out above),
// so I have to write functions that return them! (-Ankur)
// Commenting out the global variables above and replacing them with functions that
// return the same values is the only thing I changed since, for some reason, my
// PHP wasn't acknowledging these global variables.
// This library is an original implementation of UCB CS graduate student, Ka-Ping Yee (http://www.zesty.ca).

function earth_radius_semimajor() {
  return 6378137.0;
}

function earth_flattening() {
  return (1/298.257223563);
}

function earth_radius_semiminor() {
  return (earth_radius_semimajor() * (1 - earth_flattening()));
}

function earth_eccentricity_sq() {
  return (2*earth_flattening() - pow(earth_flattening(), 2));
}

// Latitudes in all of U. S.: from -7.2 (American Samoa) to 70.5 (Alaska).
// Latitudes in continental U. S.: from 24.6 (Florida) to 49.0 (Washington).
// Average latitude of all U. S. zipcodes: 37.9.

function earth_radius($latitude=37.9) {
  //global $earth_radius_semimajor, $earth_radius_semiminor;
  // Estimate the Earth's radius at a given latitude.
  // Default to an approximate average radius for the United States.

  $lat = deg2rad($latitude);

  $x = cos($lat)/earth_radius_semimajor();
  $y = sin($lat)/earth_radius_semiminor();
  return 1 / (sqrt($x*$x + $y*$y));
}

function earth_xyz($longitude, $latitude, $height = 0) {
  // Convert longitude and latitude to earth-centered earth-fixed coordinates.
  // X axis is 0 long, 0 lat; Y axis is 90 deg E; Z axis is north pole.
  //global $earth_radius_semimajor, $earth_eccentricity_sq;
  $long = deg2rad($longitude);
  $lat = deg2rad($latitude);

  $coslong = cos($long);
  $coslat = cos($lat);
  $sinlong = sin($long);
  $sinlat = sin($lat);
  $radius = earth_radius_semimajor() /
    sqrt(1 - earth_eccentricity_sq() * $sinlat * $sinlat);
  $x = ($radius + $height) * $coslat * $coslong;
  $y = ($radius + $height) * $coslat * $sinlong;
  $z = ($radius * (1 - earth_eccentricity_sq()) + $height) * $sinlat;
  return array($x, $y, $z);
}

function earth_arclength($angle, $latitude=37.9) {
  // Convert a given angle to earth-surface distance.
  return deg2rad($angle) * earth_radius($latitude);
}

function earth_distance($longitude1, $latitude1, $longitude2, $latitude2) {
  // Estimate the earth-surface distance between two locations.
  $long1 = deg2rad($longitude1);
  $lat1 = deg2rad($latitude1);
  $long2 = deg2rad($longitude2);
  $lat2 = deg2rad($latitude2);
  $radius = earth_radius(($latitude1 + $latitude2) / 2);

  $cosangle = cos($lat1)*cos($lat2) *
    (cos($long1)*cos($long2) + sin($long1)*sin($long2)) +
    sin($lat1)*sin($lat2);
  return acos($cosangle) * $radius;
}

/*
 * Returns the SQL fragment needed to add a column called 'distance'
 * to a query that includes the location table
 *
 * @param $longitude   The measurement point
 * @param $latibude    The measurement point
 * @param $tbl_alias   If necessary, the alias name of the location table to work from.  Only required when working with named {location} tables
 */
function earth_distance_sql($longitude, $latitude, $tbl_alias = '') {
  // Make a SQL expression that estimates the distance to the given location.
  $long = deg2rad($longitude);
  $lat = deg2rad($latitude);
  $radius = earth_radius($latitude);

  // If the table alias is specified, add on the separator.
  $tbl_alias = empty($tbl_alias) ? $tbl_alias : ($tbl_alias .'.');

  $coslong = cos($long);
  $coslat = cos($lat);
  $sinlong = sin($long);
  $sinlat = sin($lat);
  return "(IFNULL(ACOS($coslat*COS(RADIANS({$tbl_alias}latitude))*($coslong*COS(RADIANS({$tbl_alias}longitude)) + $sinlong*SIN(RADIANS({$tbl_alias}longitude))) + $sinlat*SIN(RADIANS({$tbl_alias}latitude))), 0.00000)*$radius)";
}

/**
 * @todo This function uses earth_asin_safe so is not accurate for all possible
 *   parameter combinations. This means this function doesn't work properly
 *   for high distance values. This function needs to be re-written to work properly for
 *   larger distance values. See http://drupal.org/node/821628
 */
function earth_longitude_range($longitude, $latitude, $distance) {
  // Estimate the min and max longitudes within $distance of a given location.
  $long = deg2rad($longitude);
  $lat = deg2rad($latitude);
  $radius = earth_radius($latitude);

  $angle = $distance / $radius;
  $diff = earth_asin_safe(sin($angle)/cos($lat));
  $minlong = $long - $diff;
  $maxlong = $long + $diff;
  if ($minlong < -pi()) { $minlong = $minlong + pi()*2; }
  if ($maxlong > pi()) { $maxlong = $maxlong - pi()*2; }
  return array(rad2deg($minlong), rad2deg($maxlong));
}

function earth_latitude_range($longitude, $latitude, $distance) {
  // Estimate the min and max latitudes within $distance of a given location.
  $long = deg2rad($longitude);
  $lat = deg2rad($latitude);
  $radius = earth_radius($latitude);

  $angle = $distance / $radius;
  $minlat = $lat - $angle;
  $maxlat = $lat + $angle;
  $rightangle = pi()/2;
  if ($minlat < -$rightangle) { // wrapped around the south pole
    $overshoot = -$minlat - $rightangle;
    $minlat = -$rightangle + $overshoot;
    if ($minlat > $maxlat) { $maxlat = $minlat; }
    $minlat = -$rightangle;
  }
  if ($maxlat > $rightangle) { // wrapped around the north pole
    $overshoot = $maxlat - $rightangle;
    $maxlat = $rightangle - $overshoot;
    if ($maxlat < $minlat) { $minlat = $maxlat; }
    $maxlat = $rightangle;
  }
  return array(rad2deg($minlat), rad2deg($maxlat));
}

/**
 * This is a helper function to avoid errors when using the asin() PHP function.
 * asin is only real for values between -1 and 1.
 * If a value outside that range is given it returns NAN (not a number), which
 * we don't want to happen.
 * So this just rounds values outside this range to -1 or 1 first.
 *
 * This means that calculations done using this function with $x outside the range
 * will not be accurate.  The alternative though is getting NAN, which is an error
 * and won't give accurate results anyway.
 */
function earth_asin_safe($x) {
  return asin(max(-1, min($x, 1)));
}
