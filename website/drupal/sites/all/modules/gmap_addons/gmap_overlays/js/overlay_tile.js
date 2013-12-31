/**
 *  Handle tile overlays in google format.
 */

Drupal.gmap.addHandler('gmap', function(elem) {
  var obj = this;
  obj.overlay_tile = [];
  obj.bind('init', function() {
    jQuery.each(obj.vars.overlay, function(i,d) {
      switch (d.type) {
        case 'tile':
          var minzoom = null;
          var maxzoom = null;
          // Note: We end up having to manually manage this anyway, because
          // these numbers are there to adjust the bounds of the zoom control
          // when being used as a baselayer. Since we are only supporting
          // overlays, we need to manage our own hiding / showing.
          if (d.minZoom) { minzoom = d.minZoom; }
          if (d.maxZoom) { maxzoom = d.maxZoom; }
          var tilelayer =  new GTileLayer(new GCopyrightCollection(), minzoom, maxzoom, d.options);
          var myTileLayer = new GTileLayerOverlay(tilelayer);
          obj.map.addOverlay(myTileLayer);
          obj.overlay_tile.push({
            layer: tilelayer,
            overlay: myTileLayer,
            minZoom: minzoom,
            maxZoom: maxzoom
          })
          break;
      }
    });
  });
  // Manage hiding and showing layer overlays as needed.
  obj.bind('zoom', function () {
    jQuery.each(obj.overlay_tile, function (i, n) {
      if (n.minZoom != null) {
        if (obj.vars.zoom >= n.minZoom) {
          if (n.overlay.isHidden()) {
            n.overlay.show();
          }
        }
        else {
          if (!n.overlay.isHidden()) {
            n.overlay.hide();
          }
        }
      }
      if (n.maxZoom != null) {
        if (obj.vars.zoom <= n.maxZoom) {
          if (n.overlay.isHidden()) {
            n.overlay.show();
          }
        }
        else {
          if (!n.overlay.isHidden()) {
            n.overlay.hide();
          }
        }
      }
    });
  });
});
