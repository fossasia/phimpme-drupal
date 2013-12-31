
/**
 * @file
 * GMap Markers
 * Jef Poskanzer's Clusterer.js API version
 */

/*global Clusterer, Drupal, GMarker */

// Replace to override marker creation
Drupal.gmap.factory.marker = function (loc, opts) {
  return new GMarker(loc, opts);
};

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  obj.bind('init', function () {
    obj.clusterer = new Clusterer(obj.map);
    var s = Drupal.settings.gmap_markermanager;
    if (s) {
      obj.clusterer.SetMaxVisibleMarkers(+s.max_nocluster);
      obj.clusterer.SetMinMarkersPerCluster(+s.cluster_min);
      obj.clusterer.SetMaxLinesPerInfoBox(+s.max_lines);
    }
  });

  obj.bind('iconsready', function () {
    var s = Drupal.settings.gmap_markermanager;
    if (s.marker.length) {
      obj.clusterer.SetIcon(Drupal.gmap.getIcon(s.marker, 0));
    }
  });

  obj.bind('addmarker', function (marker) {
    var t = '';
    if (marker.opts.title) {
      t = marker.opts.title;
      if (marker.link) {
        t = '<a href="' + marker.link + '">' + t + '</a>';
      }
    }
    obj.clusterer.AddMarker(marker.marker, t);
  });

  obj.bind('delmarker', function (marker) {
    obj.clusterer.RemoveMarker(marker.marker);
  });

  obj.bind('clearmarkers', function () {
    // @@@ Maybe don't nuke ALL overlays?
    obj.map.clearOverlays();
  });
});

////////////////// Clusterer overrides section //////////////////

// Store original implementations of overridden functions
Clusterer.origFunctions = {};

// Alternate popup code from: http://drupal.org/node/155104#comment-574696
Clusterer.origFunctions.PopUp = Clusterer.PopUp;
Clusterer.PopUp = function (cluster) {
  var mode = Drupal.settings.gmap_markermanager.popup_mode;
  if (mode === 'orig') {
    return Clusterer.origFunctions.PopUp(cluster);
  }
  else if (mode === 'zoom') {
    var bounds = new GLatLngBounds();
    for (var k in cluster.markers)
      bounds.extend(cluster.markers[k].getPoint());

    var sw = bounds.getSouthWest();
    var ne = bounds.getNorthEast();
    var rect = [
      sw,
      new GLatLng(sw.lat(), ne.lng()),
      ne,
      new GLatLng(ne.lat(), sw.lng()),
      sw
    ];

    var center = bounds.getCenter();
    var zoom = cluster.clusterer.map.getBoundsZoomLevel(bounds);
    cluster.clusterer.map.setCenter(new GLatLng(+center.lat(), +center.lng()), +zoom);
  }
};
