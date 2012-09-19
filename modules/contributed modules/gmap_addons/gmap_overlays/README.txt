This module implements various add-on overlay support for GMap.

It extends both the macro and the api interfaces.

-----------
   MACRO
-----------

Add 'overlay' sections into your macro.

[gmap |overlay=kml:http://gmaps-samples.googlecode.com/svn/trunk/ggeoxml/cta.kml]
[gmap |overlay=clientsidekml:/path/to/file.kml]
[gmap |overlay=georss:http://slashgeo.org/index.rss]
[gmap |overlay=traffic:]
[gmap |overlay=tile/0.2:http://example.com:8080/geoserver/gwc/service/gmaps?layers=topp:states&zoom={Z}&x={X}&y={Y}]

The part before the first : is the overlay type and overlay type options (slash delimited), which determines the behavior of the overlay.
The part after the : depends on the overlay type:

kml:
  The remainder is the url of the KML file.
clientsidekml:
  The remainder is the path to the KML file.
georss:
  The remainder is the url of the GeoRSS file.
traffic:
  The remainder is ignored. You still need to type the :, though.
tile:
  The remainder is the url scheme for requesting map tiles. {X}, {Y}, and {Z} will be replaced with the x, y, and zoom level.

Overlay type options (specified from left to right):
kml:
  None at the moment.
clientsidekml:
  None at the moment.
georss:
  None at the moment.
traffic:
  None at the moment.
tile:
  Opacity/minResolution/maxResolution
  Opacity:
    0.0 - 1.0 or 'gif'. Default is 1.0. The layer is assumed to be a PNG layer unless 'gif' is stated.
  minResolution: Minimum zoom level for this layer.
  maxResolution: Maximum zoom level for this layer.

----------------------
Notes on clientsidekml
----------------------

You will need to fetch a third party script and put it in thirdparty/.

See thirdparty/README.txt for more details.

--------------------
Notes on 'tile' type
--------------------

'tile' type works with any service which knows how to serve a tiled map using google's tiling setup.
This is similar to TMS, but has a different numbering scheme, documented at
http://code.google.com/apis/maps/documentation/overlays.html#Google_Maps_Coordinates

TileCache (http://www.tilecache.org/) and GeoWebCache (http://geowebcache.org/)
are programs capable of providing tiles in this way.

Of the two, GeoWebCache is the easiest to set up from scratch.

Interesting links for TileCache:
http://crschmidt.net/blog/311/using-tilecache-with-google-maps-and-virtual-earth/

Interesting links for GeoWebCache:
http://oegeo.wordpress.com/2008/05/20/the-5-minute-guide-to-setting-up-geoserver-and-geowebcache-on-windows/

Example urls:

GeoWebCache -- GeoServer extension:
http://example.com:8080/geoserver/gwc/service/gmaps?layers=topp:states&zoom={Z}&x={X}&y={Y}

GeoWebCache -- Standalone:
http://example.com:8080/geowebcache/gmaps?layers=topp:states&zoom={Z}&x={X}&y={Y}

TileCache:
http://example.com/tilecache/1.0.0/google-tiles/{Z}/{X}/{Y}.png?type=google

----------
   API
----------

$settings['overlay'] = array( // All overlays are in one numerically indexed array.
  array(
    // Overlay type, determines behavior of overlay.
    'type' => 'kml',
    // For KML and GeoRSS, the 'url' key is the URL of the file.
    'url' => 'http://mapgadgets.googlepages.com/cta.kml',
  ),
  array(
    // Overlay type, determines behavior of overlay.
    'type' => 'georss',
    // For KML and GeoRSS, the 'url' key is the URL of the file.
    'url' => 'http://slashgeo.org/index.rss',
  ),
  array(
    // Overlay type, determines behavior of overlay.
    'type' => 'traffic',
    // No options.
  ),
  array(
    // Overlay type, determines behavior of overlay.
    'type' => 'tile',
    'options' => array(
      'tileUrlTemplate' => 'http://localhost:8080/geoserver/gwc/service/gmaps?layers=topp:states&zoom={Z}&x={X}&y={Y}',
      'isPng' => TRUE,
      'opacity' => 0.2,
    ),
  ),
);

-----------
   NOTES
-----------

GeoRSS and KML both use Google's GGeoXml interface, which autodetects a feed's
format, so it doesn't matter which you use.


------
 TODO
------
minZoom and maxZoom don't actually do anything -- zoom limiting is designed for *base*
layers and needs additional show/hide code to work with overlays.
http://groups.google.com/group/Google-Maps-API/browse_thread/thread/a7aeb33781d9c573
http://groups.google.com/group/Google-Maps-API/browse_thread/thread/555d0ccefad25ec6
