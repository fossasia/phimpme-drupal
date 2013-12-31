<?php

/*
 * Created on 11.04.2007
 * parse gpx files so we can show tracks on the map
 */

function gmap_gpx_parse_file($gpxfile) {

  if (!file_exists($gpxfile)) return false;

  $fp = @fopen($gpxfile, "r");
  if (!$fp) return false;

  $data = fgets($fp, 8192);
  while (!feof($fp)) {
    $data .= fgets($fp, 8192);
  }
  fclose($fp);

  return gmap_gpx_parse($data);
}

function gmap_gpx_parse(&$data) {
  global $gmap_gpx;

  $parser = drupal_xml_parser_create($data);

  xml_set_element_handler($parser, '_gmap_gpx_start_element', '_gmap_gpx_stop_element');
  xml_set_character_data_handler($parser, '_gmap_gpx_char_data');

  $gmap_gpx = array(
    'char_data' => NULL,
    'stack' => array(),
  );
  xml_parse($parser, $data, true); //$buf, feof($fp));

  xml_parser_free($parser);

  //_gmap_gpx_write_csv_file();
  unset($gmap_gpx['stack']);
}

function gmap_gpx_cleanup() {
  global $gmap_gpx;
  $gmap_gpx = null;
}

function gmap_gpx_data2map(&$map) {
  global $gmap_gpx;

  $c = is_array($gmap_gpx['wpt']) ? count($gmap_gpx['wpt']) : 0;
  for ($i = 0; $i <= $c; $i++) {
    $wp =& $gmap_gpx['wpt'][$i];
    $m = array('latitude' => $wp['lat'], 'longitude' => $wp['lon']);
    if ($wp['name']) $m['opts']['title'] = $wp['name'];
    $map['markers'][] = $m;
    // TODO: select icon from $wp['sym'] and $wp['type']
  }

  // 0..n routes with 0..x points each
  if (isset($gmap_gpx['rte']) && is_array($gmap_gpx['rte'])) {
    $c = count($gmap_gpx['rte']); // number of routes
    for ($i = 0; $i < $c; $i++) {
      _gmap_gpx_add_wp_poly($map, $gmap_gpx['rte'][$i]['points']);
    }
  }

  // 0..n tracks with 1..m segments
  if (isset($gmap_gpx['trk']) && is_array($gmap_gpx['trk'])) {
    $c = count($gmap_gpx['trk']); // number of tracks
    for ($i = 0; $i < $c; $i++) {
      $segs =& $gmap_gpx['trk'][$i]['segs'];
      $c2 = count($segs); // number of segments in track
      for ($j = 0; $j < $c2; $j++) {
        _gmap_gpx_add_wp_poly($map, $segs[$j]['points']);
      }
    }
  }

  // TODO: make sure we don't screw up around 180 degrees
  if ($gmap_gpx['bounds']) {
    $lat = ($gmap_gpx['bounds']['minlat'] + $gmap_gpx['bounds']['maxlat']) / 2;
    $lon = ($gmap_gpx['bounds']['minlon'] + $gmap_gpx['bounds']['maxlon']) / 2;
    $map['latlong'] = $lat .','. $lon;
  }
}

function _gmap_gpx_add_wp_poly(&$map, &$wpa) {
  $c = count($wpa);
  $s = array('type' => 'polygon', 'points' => array());
  for ($i = 0; $i < $c; $i++) {
    $wp = $wpa[$i];
    $s['points'][] = array('latitude' => $wp['lat'], 'longitude' => $wp['lon']);
  }
  //$s['style'] = array('#0000ff', 3, 1);
  $map['shapes'][] = $s;
}

function _gmap_gpx_start_element(&$parser, $name, $attrs) {
  global $gmap_gpx;
  static $wpa = array('GPX WPT' => 1, 'GPX RTE RTEPT' => 1,
                       'GPX TRK TRKSEG TRKPT' => 1);
  $path = join(' ', $gmap_gpx['stack']) .' '. $name;

  //if (($path == 'GPX WPT') || ($path == 'GPX RTE RTEPT') || ($path == 'GPX TRK TRKSEG TRKPT')) {
  if (isset ($wpa[$path])) {
    $gmap_gpx['waypoint'] = array(
      'lat' => $attrs['LAT'],
      'lon' => $attrs['LON'],
      //'lat_pretty' => _gmap_gpx_latlon_pretty($attrs['LAT'], 'N', 'S'),
      //'lon_pretty' => _gmap_gpx_latlon_pretty($attrs['LON'], 'E', 'W', true),
    );
    $gmap_gpx['point_type'] = strtolower($name); // 'wpt' || 'rtept' || 'trkpt'
  }
  elseif (isset($gmap_gpx['waypoint']) && ($name == 'EXTENSIONS')) {
    $gmap_gpx['in_extension'] = true;
  }
  elseif ($name == 'BOUNDS') {
    foreach ($attrs as $key => $val) {
      $gmap_gpx['bounds'][strtolower($key)] = $val;
    }
  }

  $gmap_gpx['char_data'] = '';
  $gmap_gpx['stack'][] = $name;
}

function _gmap_gpx_stop_element(&$parser, $name) {
  global $gmap_gpx;
  static $info_tags = array('NAME' => 1 , 'DESC' => 1, 'CMT' => 1,
    'URL' => 1, 'URLNAME' => 1, 'TIME' => 1);
  static $wpa = array('GPX WPT' => 1, 'GPX RTE RTEPT' => 1,
                       'GPX TRK TRKSEG TRKPT' => 1);

  $path = join(' ', $gmap_gpx['stack']);
  array_pop($gmap_gpx['stack']); // remove $name from stack

  $point_type = $gmap_gpx['point_type'];
  if (isset($wpa[$path])) { // closing tag of one point
    $gmap_gpx[$point_type]['points'][] = $gmap_gpx['waypoint'];
    unset($gmap_gpx['waypoint']);
    unset($gmap_gpx['point_type']);
  }
  elseif (isset($gmap_gpx['waypoint'])) { // more info for a point
    if ($gmap_gpx['in_extension']) {
      if ($name == 'EXTENSIONS') unset($gmap_gpx['in_extension']);
      return;
    }
    if ($name == 'TIME') {
      $gmap_gpx['waypoint']['time'] = strtotime($gmap_gpx['char_data']);
    }
    else {
      $gmap_gpx['waypoint'][strtolower($name)] = $gmap_gpx['char_data'];
    }
  }
  elseif ($path == 'GPX RTE') { // end of a route
    $gmap_gpx['rte'][] = $gmap_gpx['rtept'];
    unset($gmap_gpx['rtept']);
  }
  elseif ($path == 'GPX TRK TRKSEG') { // end of a track segment
    $gmap_gpx['cur_trk']['segs'][] = $gmap_gpx['trkpt'];
    unset($gmap_gpx['trkpt']);
  }
  elseif ($path == 'GPX TRK') { // end of a track
    $gmap_gpx['trk'][] = $gmap_gpx['cur_trk'];
    unset($gmap_gpx['cur_trk']);
  }
  elseif (isset($info_tags[$name])) { // more info for track, route or whole file
    $nl = strtolower($name);
    $v = ($name == 'time') ? strtotime($gmap_gpx['char_data']) : $gmap_gpx['char_data'];
    switch ($gmap_gpx['stack'][1]) {
      case 'RTE':
        $gmap_gpx['rtept']['info'][$nl] = $v;
        break;
      case 'TRK':
        $gmap_gpx['cur_trk']['info'][$nl] = $v;
        break;
      default:
        $gmap_gpx['info'][$nl] = $v;
    }
  }

  unset($gmap_gpx['char_data']);
}

function _gmap_gpx_char_data(&$parser, $data) {
  global $gmap_gpx;
  $gmap_gpx['char_data'] .= $data;
}

function _gmap_gpx_latlon_pretty($deg, $pos, $neg, $three = false) {
  if ($deg > 0) {
    $str = $pos;
  }
  else {
    $str = $neg;
    $deg = - $deg;
  }

  $d = $deg;
  settype($d, 'integer');
  $deg -= $d;
  if ($three && $d < 100)
    $str .= '0';
  if ($d < 10)
    $str .= '0';
  $str .= $d .'°';

  $deg *= 60;
  $deg *= 1000;
  settype($deg, 'integer');
  $deg /= 1000;
  $str .= $deg ."'";

  return $str;
}
