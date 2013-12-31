<?php

/**
 * @file
 * GeoRSS support for Location.
 */

/**
 * @function
 * Return an array of RSS items for a location.
 */
function _location_rss_item($location, $mode = 'simple') {
  $ret = FALSE;
  if (location_has_coordinates($location, TRUE)) {
    switch ($mode) {
      // W3C Basic Geo Vocabulary
      case 'w3c':
        $ret = array(
          'key'       => 'geo:Point',
          'namespace' => array('xmlns:geo' => 'http://www.w3.org/2003/01/geo/wgs84_pos#'),
          'value'     => array(
            array('key' => 'geo:lat', 'value' => $location['latitude']),
            array('key' => 'geo:long', 'value' => $location['longitude']),
          ),
        );
        break;
      // Location 1.x-2.x bug compatible.
      // W3C Basic Geo Vocabulary with a misspelled longitude tag.
      case 'w3c_bugcompat':
        $ret = array(
          'key'       => 'geo:Point',
          'namespace' => array('xmlns:geo' => 'http://www.w3.org/2003/01/geo/wgs84_pos#'),
          'value'     => array(
            array('key' => 'geo:lat', 'value' => $location['latitude']),
            array('key' => 'geo:lon', 'value' => $location['longitude']),
          ),
        );
        break;
      // GeoRSS-Simple
      case 'simple':
        $ret = array(
          'key'       => 'georss:point',
          'namespace' => array('xmlns:georss' => 'http://www.georss.org/georss'),
          'value'     => "$location[latitude] $location[longitude]",
        );
        break;
      //
      case 'gml':
        $ret = array(
          'key' => 'georss:where',
          'namespace' => array(
            'xmlns:georss' => 'http://www.georss.org/georss',
            'xmlns:gml' => 'http://www.opengis.net/gml',
          ),
          'value' => array(
            'gml:Point' => array(
              'gml:pos' => "$location[latitude] $location[longitude]",
            ),
          ),
        );
        break;
    }
  }
  return $ret;
}
