
/**
 * @file
 * GMap Markers
 * Google GMarkerManager API version
 */

/*global Drupal, GMarker, GMarkerManager */

// Replace to override marker creation
Drupal.gmap.factory.marker = function (loc, opts) {
  return new GMarker(loc, opts);
};

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  obj.bind('init', function () {
    // Set up the markermanager.
    obj.mm = new GMarkerManager(obj.map, Drupal.settings.gmap_markermanager);
  });

  obj.bind('addmarker', function (marker) {
    var minzoom = Drupal.settings.gmap_markermanager.markerMinZoom;
    var maxzoom = Drupal.settings.gmap_markermanager.markerMaxZoom;
    if (marker.minzoom) {
      minzoom = marker.minzoom;
    }
    if (marker.maxzoom) {
      maxzoom = marker.maxzoom;
    }
    if (maxzoom > 0) {
      obj.mm.addMarker(marker.marker, minzoom, maxzoom);
    }
    else {
      obj.mm.addMarker(marker.marker, minzoom);
    }
    obj.mm.refresh();
  });

  obj.bind('delmarker', function (marker) {
    // @@@ This is NOT AVAILABLE in this version.
  });

  obj.bind('clearmarkers', function () {
    // @@@ This is NOT AVAILABLE in this version.
  });
});
