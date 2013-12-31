/**
 *  Handle overlays via GGeoXml.
 */

Drupal.gmap.clientsidexml = {};

Drupal.gmap.addHandler('gmap', function(elem) {
  var obj = this;
  obj.bind('init', function() {
    jQuery.each(obj.vars.overlay, function(i,d) {
      switch (d.type) {
        case 'georss':
        case 'kml':
          // @@@ Add an overlay interface so all this can be managed?
          obj.map.addOverlay(new GGeoXml(d.url));
          break;
        case 'clientsidekml':
          var mapid = obj.map.getContainer().id;
          Drupal.gmap.clientsidexml[mapid] = new EGeoXml("Drupal.gmap.clientsidexml['"+mapid+"']", obj.map, d.url);
          Drupal.gmap.clientsidexml[mapid].parse();
      }
    });
  });
});
