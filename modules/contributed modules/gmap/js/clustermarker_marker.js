
/**
 * @file
 * GMap Markers
 * Martin Pearman's ClusterMarker version
 */

/*global ClusterMarker, Drupal, GMarker */

// Replace to override marker creation
Drupal.gmap.factory.marker = function (loc, opts) {
  return new GMarker(loc, opts);
};

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  obj.bind('init', function () {
    obj.clusterMarker = 0;
  });

  obj.bind('iconsready', function () {
    if (!obj.clusterMarker) {
      // Force copying the settings so we don't overwrite them.
      var options = jQuery.extend(true, {}, Drupal.settings.gmap_markermanager);
      if (options.clusterMarkerIcon.length) {
        options.clusterMarkerIcon = Drupal.gmap.getIcon(options.clusterMarkerIcon, 0);
      }
      else {
        delete options.clusterMarkerIcon;
      }
      options.borderPadding = +options.borderPadding;
      options.fitMapMaxZoom = +options.fitMapMaxZoom;
      options.intersectPadding = +options.intersectPadding;
      obj.clusterMarker = new ClusterMarker(obj.map, options);
    }
  });

  obj.bind('addmarker', function (marker) {
    obj.clusterMarker.addMarkers([marker.marker]);
  });

  obj.bind('delmarker', function (marker) {
    // @@@TODO: Implement this!
  });

  obj.bind('clearmarkers', function () {
    obj.clusterMarker.removeMarkers();
  });

  obj.bind('markersready', function () {
    obj.clusterMarker.refresh();
  });
});
