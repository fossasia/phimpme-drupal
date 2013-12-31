<?php

/**
 * @file
 * Encoded polyline utilities.
 */

/**
 * References:
 * [1] http://code.google.com/apis/maps/documentation/polylinealgorithm.html
 * [2] http://facstaff.unca.edu/mcmcclur/GoogleMaps/EncodePolyline/
 * [3] http://mathworld.wolfram.com/
 */

DEFINE('GMAP_DP_EPSILON', 0.00001);
DEFINE('GMAP_ZOOM_LEVELS', 18);
DEFINE('GMAP_ZOOM_FACTOR', 2);


/**
 * The following three functions will encode numbers so that they may be used
 * in "Encoded Polylines" on Google Maps. The encoding is described here:
 *   http://code.google.com/apis/maps/documentation/polylinealgorithm.html
 *
 * Numbers for latitudes/longitudes and levels are encoded slightly
 * differently--when generating Encoded Polylines, latitudes and longitudes are
 * encoded with gmap_polyutil_encode_signed(), and "levels" are encoded using
 * gmap_polyutil_encode_unsigned().
 */
function gmap_polyutil_encode_latlon($x) {
  $x = round($x * 1e5) << 1;
  if ($x < 0) {
    $x = ~ $x;
  }
  return _gmap_polyutil_encode($x);
}

function gmap_polyutil_encode_levels($x) {
  return _gmap_polyutil_encode(abs($x));
}

function _gmap_polyutil_encode($x) {
  $result = '';
  while ($x >= 32) {
    $result .= chr((32 | ($x & 31)) + 63);
    $x >>= 5;
  }
  $result .= chr(($x & 31) + 63);
  return $result;
}

/**
 * Distance in two dimensions.
 * √((x1-x0)^2 + (y1-y0)^2)
 */
function gmap_polyutil_dist($p1, $p2) {
  return sqrt(pow($p2[0] - $p1[0], 2) + pow($p2[1] - $p1[1], 2));
}

/**
 * Distance between a point and a line segment.
 * @param $q
 *   Point to measure.
 * @param $p1
 *   Start point of line segment.
 * @param $p2
 *   End point of line segment.
 */
function gmap_polyutil_point_line_dist($q, $p1, $p2) {
  if ($p1[0] == $p2[0] && $p1[1] == $p2[1]) {
    // lp1 and lp2 are the same point--they don't define a line--so we return
    // the distance between two points.
    return gmap_polyutil_dist($q, $p1);
  }

  // Use the dot product to find where q lies with respect to the line segment
  // p1p2. For more information, see:
  //   http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
  //   http://www.codeguru.com/forum/printthread.php?t=194400
  $u = (($p2[1]-$p1[1])*($q[1]-$p1[1]) + ($p2[0]-$p1[0])*($q[0]-$p1[0])) / (pow($p2[1]-$p1[1], 2) + pow($p2[0]-$p1[0], 2));

  if ($u <= 0) { // point is not alongside segment, it is further off in $p1's direction
    return gmap_polyutil_dist($q, $p1);
  }
  elseif ($u >= 1) { // point is not alongside segment, it is further off in $p2's direction
    return gmap_polyutil_dist($q, $p2);
  }
  else { // point is alongside segment
    // calculate distance between q and the nearest point on the line segment
    // use $u to calculate the nearest point on the line segment:
    //   p1 + u*(p2 - p1) => [p1x + u*(p2x - p1x), p1y + u*(p2y - p1y)]
    return gmap_polyutil_dist($q, array( $p1[0] + $u*($p2[0] - $p1[0]), $p1[1] + $u*($p2[1] - $p1[1])));
  }
}

/**
 * Implementation of the Douglas-Peucker polyline simplification algorithm. See:
 * http://facstaff.unca.edu/mcmcclur/GoogleMaps/EncodePolyline/algorithm.html
 *
 * @param $points
 *   An array of coordinate pairs.
 * @return
 *   An array of keys => weights; the keys correspond with indices of points in
 *   the $points array. Some points may be insignificant according to the
 *   algorithm--they will not have entries in the return array. The "weights"
 *   are actually the point's distance from the line segment that it subdivides.
 */
function gmap_polyutil_dp_encode($points) {
  $weights = array();

  if (count($points) > 2) {
    // the 'stack' holds line segments to be simplified
    $stack[] = array(0, count($points) - 1);
    
    while (count($stack) > 0) {
      // take a line segment to look at
      $segment = array_pop($stack);
      
      // figure out which subdividing point is the furthest off the line segment
      $max_dist = 0;
      for ($i = $segment[0] + 1; $i < $segment[1]; $i++) {
        $dist = gmap_polyutil_point_line_dist($points[$i], $points[$segment[0]], $points[$segment[1]]);
        if ($dist > $max_dist) {
          $max_dist = $dist;
          $max_i = $i;
        }
      }
      
      // if the subdividing point found above is significantly off the line
      // segment then we want to simplify further. Add sub-segments to the stack.
      if ($max_dist > GMAP_DP_EPSILON) {
        $weights[$max_i] = $max_dist;
        array_push($stack, array($segment[0], $max_i));
        array_push($stack, array($max_i, $segment[1]));
      }
    }
  }

  // The first and last points of the line should always be visible.
  $levels = _gmap_polyutil_zoom_levels();
  $weights[0] = $levels[0];
  $weights[count($points) -1] = $levels[0];

  return $weights;
}

/**
 * Simplify a set of points and generate an "Encoded Polyline" for Google Maps.
 * @param $points
 *   An array of coordinate pairs as latitude, longitude.
 * @return
 *   An array containing the point and zoom information necessary to display
 *   encoded polylines on Google Maps: 'points', 'levels', 'numLevels', and 'zoomFactor'.
 */
function gmap_polyutil_polyline($points) {
  $points_encoded = '';
  $levels_encoded = '';

  // simplify the line
  $weights = gmap_polyutil_dp_encode($points);

  $previous = array(0, 0);
  foreach ($points as $i => $p) {
    if (isset($weights[$i])) {
      // encode each simplified point
      // the deltas between points are encoded, rather than the point values themselves
      $points_encoded .= gmap_polyutil_encode_latlon($p[0] - $previous[0]) . gmap_polyutil_encode_latlon($p[1] - $previous[1]);
      $levels_encoded .= gmap_polyutil_encode_levels(_gmap_polyutil_get_zoom_level($weights[$i]));
      $previous = $p;
    }
  }

  return array(
    'points' => $points_encoded,
    'levels' => $levels_encoded,
    'numLevels' => GMAP_ZOOM_LEVELS,
    'zoomFactor' => GMAP_ZOOM_FACTOR,
  );
}

/**
 * Build a logarithmic scale of zoom levels.
 */
function _gmap_polyutil_zoom_levels() {
  static $levels;
  if (!isset($levels)) {
    for ($i = 0; $i < GMAP_ZOOM_LEVELS; $i++) {
      $levels[$i] = GMAP_DP_EPSILON * pow(GMAP_ZOOM_FACTOR, GMAP_ZOOM_LEVELS - $i - 1);
    }
  }
  return $levels;
}

/**
 * Place points in levels based on their "weight" -- a value derived from
 * distance calculations in the simplification algorithm, gmap_polyutil_dp_encode().
 */
function _gmap_polyutil_get_zoom_level($weight) {
  $levels = _gmap_polyutil_zoom_levels();
  $i = 0;
  while ($levels[$i] > $weight) {
    $i++;
  }
  return GMAP_ZOOM_LEVELS - $i - 1;
}
