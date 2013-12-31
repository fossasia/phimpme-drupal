<?php

/**
 * @file
 * GMap macro parsing routines.
 */


/**
 * Parse a macro style definition.
 * Example: #111111/1/100/#111111/1
 * @internal
 */
function _gmap_parse_style($style) {
  if (strpos($style, '/') === FALSE) {
    // Style ref.
    return $style;
  }
  $styles = explode('/', $style);

  // @@@ Todo: Fix up old xmaps stuff. Possibly detect by looking for array length 7?

  // Strip off # signs, they get added by code.
  if (isset($styles[0]) && substr($styles[0], 0, 1) == '#') {
    $styles[0] = substr($styles[0], 1);
  }
  if (isset($styles[3]) && substr($styles[3], 0, 1) == '#') {
    $styles[3] = substr($styles[3], 1);
  }

  // Assume anything > 0 and < 1.1 was an old representation.
  if ($styles[2] > 0 && $styles[2] < 1.1) {
    $styles[2] = $styles[2] * 100;
  }
  if (isset($styles[4])) {
    if ($styles[4] > 0 && $styles[4] < 1.1) {
      $styles[4] = $styles[4] * 100;
    }
  }

  return $styles;
}

/**
 * Parse "x.xxxxx , y.yyyyyy (+ x.xxxxx, y.yyyyy ...)" into an array of points.
 * @internal
 */
function _gmap_str2coord($str) {
  // Explode along + axis
  $arr = explode('+', $str);
  // Explode along , axis
  $points = array();
  foreach ($arr as $pt) {
    list($lat, $lon) = explode(',', $pt);
    $points[] = array((float)trim($lat), (float)trim($lon));
  }
  return $points;
}

function _gmap_parse_macro($instring, $ver = 2) {
  // Get a list of keys that are "multiple."
  $m = array();
  $multiple = gmap_module_invoke('macro_multiple', $m);
  $def = gmap_defaults();

  // Remove leading and trailing tags
  if (substr(trim($instring), -1)==']') {
    $instring = substr(trim($instring), 0, -1);
  }
  if (substr($instring, 0, 5)=='[gmap') {
    $instring = substr($instring, 6);
  }

  // Chop the macro into an array
  $temp = explode('|', $instring);
  $m = array();
  foreach ($temp as $row) {
    $offset = strpos($row, '=');
    if ($offset !== FALSE) {
      $k = trim(substr($row, 0, $offset));
      $r = trim(substr($row, $offset+1));
      if (in_array($k, $multiple)) {
        // Things that can appear multiple times
        if (!isset($m[$k])) {
          $m[$k] = array();
        }
        $m[$k][] = $r;
      }
      else {
        $m[$k] = $r;
      }
    }
  }

  // Synonyms
  if (isset($m['type'])) {
    $m['maptype'] = $m['type'];
    unset($m['type']);
  }
  if (isset($m['control'])) {
    $m['controltype'] = $m['control'];
    unset($m['control']);
  }

  if (isset($m['feed']) && is_array($m['feed'])) {
    foreach ($m['feed'] as $k => $v) {
      $temp = explode('::', $v);
      // Normalize url
      if (substr($temp[1], 0, 1) == '/') {
        $temp[1] = substr($temp[1], 1);
      }
      $temp[1] = url($temp[1]);
      $m['feed'][$k] = array(
        'markername' => $temp[0],
        'url' => $temp[1],
      );
    }
  }

  // Add custom styles.
  if (isset($m['style']) && is_array($m['style'])) {
    foreach ($m['style'] as $k => $v) {
      $temp = explode(':', $v);
      $m['styles'][$temp[0]] = _gmap_parse_style($temp[1]);
    }
    unset($m['style']);
  }

  // Merge points and markers
  if (!isset($m['points']) || !is_array($m['points'])) $m['points'] = array();
  if (!isset($m['markers']) || !is_array($m['markers'])) $m['markers'] = array();
  $m['markers-temp'] = array_merge($m['points'], $m['markers']);
  unset($m['points']);
  unset($m['markers']);

  // all shapes in 1 array
  if (isset($m['circle']) && is_array($m['circle'])) {
    foreach ($m['circle'] as $shape) {
      $s = array('type' => 'circle');
      $cp = strpos($shape, ':');
      if ($cp !== FALSE) {
        $stylestr = substr($shape, 0, $cp);
        $s['style'] = _gmap_parse_style($stylestr);
        $shape = substr($shape, $cp+1);
      }
      $tmp = explode('+', $shape);
      $s['radius'] = $tmp[1] ? $tmp[1] : 100;
      if (isset($tmp[2]) && $tmp[2]) $s['numpoints'] = trim($tmp[2]);
      $tmp = _gmap_str2coord($tmp[0]);
      $s['center'] = $tmp[0];
      $m['shapes'][] = $s;
    }
    unset($m['circle']);
  }
  // Fixup legacy lines.
  if (isset($m['line1'])) {
    if (!isset($m['line'])) $m['line'] = array();
    $m['line'][] = $def['line_colors'][0] . ':' . $m['line1'];
    unset($m['line1']);
  }
  if (isset($m['line2'])) {
    if (!isset($m['line'])) $m['line'] = array();
    $m['line'][] = $def['line_colors'][1] . ':' . $m['line3'];
    unset($m['line2']);
  }
  if (isset($m['line3'])) {
    if (!isset($m['line'])) $m['line'] = array();
    $m['line'][] = $def['line_colors'][2] . ':' . $m['line3'];
    unset($m['line3']);
  }

  if (isset($m['line']) && is_array($m['line'])) {
    foreach ($m['line'] as $shape) {
      $s = array('type' => 'line');
      $cp = strpos($shape, ':');
      if ($cp != FALSE) {
        $stylestr = substr($shape, 0, $cp);
        $s['style'] = _gmap_parse_style($stylestr);
        $shape = substr($shape, $cp+1);
      }
      $s['points'] = _gmap_str2coord($shape);
      $m['shapes'][] = $s;
    }
    unset($m['line']);
  }
  if (isset($m['rpolygon']) && is_array($m['rpolygon'])) {
    foreach ($m['rpolygon'] as $shape) {
      $s = array('type' => 'rpolygon');
      $cp = strpos($shape, ':');
      if ($cp !== FALSE) {
        $stylestr = substr($shape, 0, $cp);
        $s['style'] = _gmap_parse_style($stylestr);
        $shape = substr($shape, $cp+1);
      }
      $tmp = explode('+', $shape);
      if ($tmp[2]) {
        $s['numpoints'] = (int)trim($tmp[2]);
        $tmp = array_slice($tmp, 0, 2);
      }
      $shape = implode('+', $tmp);
      $tmp = _gmap_str2coord($shape);
      $s['center'] = $tmp[0];
      $s['point2'] = $tmp[1];
      $m['shapes'][] = $s;
    }
    unset($m['rpolygon']);
  }
  if (isset($m['polygon']) && is_array($m['polygon'])) {
    foreach ($m['polygon'] as $shape) {
      $s = array('type' => 'polygon');
      $cp = strpos($shape, ':');
      if ($cp !== FALSE) {
        $stylestr = substr($shape, 0, $cp);
        $s['style'] = _gmap_parse_style($stylestr);
        $shape = substr($shape, $cp+1);
      }
      $s['points'] = _gmap_str2coord($shape);
      $m['shapes'][] = $s;
    }
    unset($m['polygon']);
  }

  // Version 1 -> 2 conversion
  if ($ver == 1) {
    // Zoom is flipped
    if (isset($m['zoom'])) {
      $m['zoom'] = 18 - $m['zoom'];
      if ($m['zoom'] < 1) {
        $m['zoom'] = 1;
      }
    }
  }

  // Center -> latitude and longitude
  if (isset($m['center']) && $m['center']) {
    list($m['latitude'], $m['longitude']) = explode(',', $m['center']);
    unset($m['center']);
  }

  // Behavior
  if (isset($m['behaviour'])) {
    $m['behavior'] = $m['behaviour'];
    unset($m['behaviour']);
  }
  if (isset($m['behavior'])) {
    $sep = ' ';
    if (strpos($m['behavior'], ',') !== FALSE) {
      // In some places, commas were used to seperate behaviors.
      // This was originally an accident, but it's easy enough to support.
      $sep = ',';
    }
    $m['behavior-temp'] = explode($sep, $m['behavior']);
    // 2010 Nov 30 change:
    // Fix a very old bug regarding behavior flags:
    // It was always supposed to defer to the site default behaviors for every
    // flag not defined by the macro, but this was just plain not happening.
    // This is a backwards-incompatible change
    $m['behavior'] = $def['behavior'];
    foreach ($m['behavior-temp'] as $v) {
      $m['behavior'][substr($v, 1)] = (substr($v, 0, 1) == '+') ? TRUE : FALSE;
    }
    unset($m['behavior-temp']);
  }

  // tcontrol now is mtc.
  if (isset($m['tcontrol'])) {
    if (strtolower(trim($m['tcontrol'])) == 'on') {
      $m['mtc'] = 'standard';
    }
    else {
      $m['mtc'] = 'none';
    }
    unset($m['tcontrol']);
  }

  // notype also controls mtc.
  if (isset($m['behavior']['notype'])) {
    if ($m['behavior']['notype']) {
      $m['mtc']= 'none';
    }
    unset($m['behavior']['notype']);
  }

  // Stuff that was converted to behavior flags

  // Scale control.
  if (isset($m['scontrol'])) {
    if (strtolower(trim($m['scontrol'])) == 'on') {
      $m['behavior']['scale'] = TRUE;
    }
    else {
      $m['behavior']['scale'] = FALSE;
    }
    unset($m['scontrol']);
  }

  // Draggability.
  if (isset($m['drag'])) {
    if (strtolower(trim($m['drag'])) == 'yes') {
      $m['behavior']['nodrag'] = FALSE;
    }
    else {
      $m['behavior']['nodrag'] = TRUE;
    }
    unset($m['drag']);
  }

  // Markers fixup
  foreach ($m['markers-temp'] as $t) {
    unset($markername);
    // Named?
    if (strpos($t, '::')) { // Single : gets handled below.
      list($markername, $t) = explode('::', $t, 2);
    }
    // Break down into points
    $points = explode('+', $t);
    $offset = -1;
    foreach ($points as $point) {
      $marker = array();
      $offset++;
      $marker['options'] = array();
      // Labelled?
      // @@@ Gmap allows both a tooltip and a popup, how to represent?
      if (strpos($point, ':')) {
        list($point, $marker['text']) = explode(':', $point, 2);
        $marker['text'] = theme('gmap_marker_popup', array('label' => $marker['text']));
      }
      if (strpos($point, '%')) {
        list($point, $addons) = explode('%', $point, 2);
        $motemp = explode('%', $addons);
        foreach ($motemp as $option) {
          $marker['options'][trim($option)] = TRUE;
        }
      }
      list($marker['latitude'], $marker['longitude']) = explode(',', $point, 2);
      // Named markers get an offset too.
      if (isset($markername)) {
        $marker['markername'] = $markername;
        $marker['offset'] = $offset;
      }
      $m['markers'][] = $marker;
    }
  }
  unset($m['markers-temp']);

  // Assign an id if one wasn't specified.
  if (!isset($m['id'])) {
    $m['id'] = gmap_get_auto_mapid();
  }

  // The macro can now be manipulated by reference.
  // Note: We do NOT use gmap_module_invoke here,
  // as this $op has weird semantics for backwards
  // compatibility / convenience reasons.
  // (Specifically, modules are allowed to do arbitrary
  // manipulations on $m OR return the changes
  // they want to apply to $m.)
  foreach (module_implements('gmap') as $module) {
    $additions = call_user_func_array($module . '_gmap', array('parse_macro', &$m));
    if (!empty($additions)) {
      foreach ($additions as $k => $v) {
        $m[$k] = $v;
      }
    }
  }
  return $m;
}
