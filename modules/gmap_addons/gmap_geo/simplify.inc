<?php

/**
 * @file
 */

/**
 * This function would probably work for lines as well as polygons, but you still have to
 * feed it a centroid.
 *
 * @param $vertices
 *   An array of the shape's vertices; each vertex is represented as array(x, y)
 * @param $centroid
 *   The centroid of the shape, represented as array(x, y)
 * @param $num_interesting_vertices
 *   Defaults to 20. How many 'interesting' vertices will be kept in the first round.
 * @param $restore_threshold
 *   Defaults to 1. Approximate allowed error, as miles between a point and its approximating line.
 * @param $restore_span
 *   Defaults to 10. Approximate maximum resolution of restored points in long spans, as miles.
 * @return
 *   An array of points which approximates the original shape. This set of 
 *   points will be a subset of the original array.
 *
 * A note on the use of "miles" in $restore_threshold and $restore_span: these
 * are approximate; by "miles" I mean "distance when lat/lon is treated as a
 * rectangular grid and 1 degree = 69 miles." Precision is not necessary here,
 * and "miles" are used for convenience.
 *
 * Within the body of this function:
 *   $vertices is the original set of vertices
 *   $vertices_rev1 is the first, "interesting" set of vertices
 *   $vertices_rev2 is the second, "restored" set of vertices, and the return value of the fn.
 */
function gmap_geo_simplify($vertices, $centroid, $num_interesting_vertices = 20, $restore_threshold = 1, $restore_span = 10) {
  if (count($vertices) <= $num_interesting_vertices + 10) {
    return $vertices;
  }
  
  //// first we'll pick some of the most interesting vertices
  //// this algorithm is from Advances in Information Systems, By Tatyana Yakhno
  //// KiMPA, p. 191, as seen on http://books.google.com/
  foreach ($vertices as $i => $v) {
    // previous vertex
    $prev = $vertices[$i - 1];

    // preserve ordering
    $v['order'] = $i;
    // distance to centroid
    $v['distance'] = _gmap_geo_simplify_dist($centroid, $v);
    // find angle of line btw centroid & point
    $dy = $centroid[1] - $v[1];
    $dx = $centroid[0] - $v[0];
    $v['angle'] = atan($dy/$dx);
    // find 'velocity'
    $v['velocity'] = abs($v['distance'] - $prev['distance']) / abs($v['angle'] - $prev['angle']);
    // find 'acceleration'
    $dv = $v['velocity'] - $prev['velocity'];
    $sign = $dv / abs($dv);
    $v['acceleration'] = $sign * $dv / abs($v['angle'] - $prev['angle']);
    
    // save $v
    $vertices[$i] = $v;
  }

  usort($vertices, '_gmap_geo_simplify_cmp_velocity'); // sort by velocity
  $vertices_tmp = array_slice($vertices, 0, $num_interesting_vertices * 2); // trim the less interesting
  usort($vertices_tmp, '_gmap_geo_simplify_cmp_acceleration'); // sort by acceleration
  $vertices_rev1 = array_slice($vertices_tmp, 0, $num_interesting_vertices); // again trim the less interesting
  usort($vertices_rev1, '_gmap_geo_simplify_cmp_order'); // put everything back in order
  
  // we always want the first and last points--this closes polygons and makes
  // sure that lines begin and end where they're supposed to.
  usort($vertices, '_gmap_geo_simplify_cmp_order');
  if (reset($vertices_rev1) != reset($vertices)) {
    array_unshift($vertices_rev1, reset($vertices));
  }
  if (end($vertices_rev1) != end($vertices)) {
    array_push($vertices_rev1, end($vertices));
  }

  //// now that we have a representation of some interesting parts of the poly,
  //// restore some less interesting corners
  $vertices_rev2 = array();

  $prev = end($vertices_rev1);
  $next = reset($vertices_rev1);
  $slope = ($next[1] - $prev[1])/(($next[0] - $prev[0]) ? ($next[0] - $prev[0]) : 0.0000000000000000001);
  $intercept = $prev[1] - $slope * $prev[0];

  foreach ($vertices as $v) {
    $i = array_search($v, $vertices_rev1);
    if ($i === FALSE) {
      // calculate how far astray the simplified shape is; we just use the
      // unprojected lat/lon here so it's more or less skewed depending on
      // where you are on the globe.
      $m = -1 / $slope; $b = $v[1] - $m * $v[0];
      $p[0] = ($b - $intercept)/($slope - $m);
      $p[1] = $m * $p[0] + $b;
      $v['error'] = _gmap_geo_simplify_dist($p, $v);
      
      // if the line is more than ~1 miles away from the excised point, we may
      // want that point back. this is a pretty arbitrary value, but it filters
      // out some noise
      if ($v['error']*69 > $restore_threshold) {
        $excised[] = $v;
      }
    }
    else {
      $vertices_rev2[] = $v;

      $prev = $v;
      $next = $vertices_rev1[$i + 1];
      $slope = ($next[1] - $prev[1])/($next[0] - $prev[0]);
      $intercept = $prev[1] - $slope * $prev[0];
    }

    // if there are any points we might want back between the current
    if (!empty($excised)) {
      $p0 = end($excised);
      $p1 = reset($excised);
      
      // if the most recent point is in $vertices_rev2 OR if the array of
      // interesting points is dispersed more than ~10 miles
      if ($i !== FALSE || $restore_span < _gmap_geo_simplify_dist($p0, $p1) * 69) {
        // take the point with the largest error
        usort($excised, '_gmap_geo_simplify_cmp_error');
        $vertices_rev2[] = $excised[0];

        $prev = $excised[0];
        $slope = ($next[1] - $prev[1])/($next[0] - $prev[0]);
        $intercept = $prev[1] - $slope * $prev[0];
        $excised = array();
      }
    }
  }
  usort($vertices_rev2, '_gmap_geo_simplify_cmp_order');

  // just return the points
  $result = array();
  foreach ($vertices_rev2 as $v) {
    $result[] = array($v[0], $v[1]);
  }
  return $result;
}

/**
 * Comparison functions for sorting by various keys. These REVERSE sort the arrays.
 */
function _gmap_geo_simplify_cmp_velocity($a, $b) {
  return ($a['velocity'] < $b['velocity'] ? 1 : ($a['velocity'] != $b['velocity'] ? -1 : 0));
}

function _gmap_geo_simplify_cmp_acceleration($a, $b) {
  return ($a['acceleration'] < $b['acceleration'] ? 1 : ($a['acceleration'] != $b['acceleration'] ? -1 : 0));
}

function _gmap_geo_simplify_cmp_order($a, $b) {
  return ($a['order'] < $b['order'] ? 1 : ($a['order'] != $b['order'] ? -1 : 0));
}

function _gmap_geo_simplify_cmp_error($a, $b) {
  return ($a['error'] < $b['error'] ? 1 : ($a['error'] != $b['error'] ? -1 : 0));
}

/**
 * Distance between two points.
 * @param $p0, @param $p1
 *   A point, as array(x, y)
 */
function _gmap_geo_simplify_dist($p0, $p1) {
  $dx = $p1[0] - $p0[0];
  $dy = $p1[1] - $p0[1];
  return sqrt($dx*$dx + $dy*$dy);
}
