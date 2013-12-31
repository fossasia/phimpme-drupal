/**
 *  The Google Maps Traffic Overlay.
 */

Drupal.gmap.addHandler('gmap', function(elem) {
  var obj = this;
  obj.bind('init', function() {
    jQuery.each(obj.vars.overlay, function(i,d) {
      switch (d.type) {
        case 'traffic':
          // @@@ Add an overlay interface so all this can be managed?
          obj.map.addOverlay(new GTrafficOverlay());
          break;
      }
    });
  });
});
