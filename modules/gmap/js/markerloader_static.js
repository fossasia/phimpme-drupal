
/**
 * @file
 * GMap Marker Loader
 * Static markers.
 * This is a simple marker loader to read markers from the map settings array.
 * Commonly used with macros.
 */

/*global Drupal */

// Add a gmap handler
Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;
  var marker, i;
  if (obj.vars.markers) {
    // Inject markers as soon as the icon loader is ready.
    obj.bind('iconsready', function () {
      for (i = 0; i < obj.vars.markers.length; i++) {
        marker = obj.vars.markers[i];
        if (!marker.opts) {
          marker.opts = {};
        }
        // Pass around the object, bindings can change it if necessary.
        obj.change('preparemarker', -1, marker);
        // And add it.
        obj.change('addmarker', -1, marker);
      }
      obj.change('markersready', -1);
    });
  }
});
