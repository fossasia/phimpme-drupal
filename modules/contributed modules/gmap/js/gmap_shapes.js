
/**
 * @file
 * GMap Shapes
 * GMap API version / Base case
 */

/*global jQuery, Drupal, GEvent, GLatLng, GPolygon, GPolyline */

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;
/*
  obj.bind('init',function() {
    if (obj.vars.behavior.autozoom) {
      obj.bounds = new GLatLngBounds(new GLatLng(obj.vars.latitude,obj.vars.longitude),new GLatLng(obj.vars.latitude,obj.vars.longitude));
    }
  });
*/
  obj.bind('prepareshape', function (shape) {
    var pa, cargs, style;
    //var m = new GMarker(new GLatLng(marker.latitude,marker.longitude),marker.opts);
    pa = []; // point array (array of GLatLng-objects)
    var fillstyle = true;
    if (shape.type === 'circle') {
      pa = obj.poly.calcPolyPoints(new GLatLng(shape.center[0], shape.center[1]), shape.radius * 1000, shape.numpoints);
    }
    else if (shape.type === 'rpolygon') {
      shape.center = new GLatLng(shape.center[0], shape.center[1]);
      shape.point2 = new GLatLng(shape.point2[0], shape.point2[1]);
      var radius = shape.center.distanceFrom(shape.point2);
      pa = obj.poly.calcPolyPoints(shape.center, radius, shape.numpoints);
    }
    else if (shape.type === 'polygon') {
      jQuery.each(shape.points, function (i, n) {
        pa.push(new GLatLng(n[0], n[1]));
      });
    }
    else if (shape.type === 'line') {
      jQuery.each(shape.points, function (i, n) {
        pa.push(new GLatLng(n[0], n[1]));
      });
      fillstyle = false;
    }
    cargs = [pa];

    // Style normalization
    if (fillstyle) {
      style = obj.vars.styles.poly_default.slice();
    }
    else {
      style = obj.vars.styles.line_default.slice();
    }
    if (shape.style) {
      if (typeof shape.style === 'string') {
        if (obj.vars.styles[shape.style]) {
          style = obj.vars.styles[shape.style].slice();
        }
      }
      else {
        style = shape.style.slice();
      }
    }
    style[0] = '#' + style[0];
    style[1] = Number(style[1]);
    style[2] = style[2] / 100;
    if (fillstyle) {
      style[3] = '#' + style[3];
      style[4] = style[4] / 100;
    }
    
    if (shape.type == 'encoded_line') {
      shape.color = style[0];
      shape.weight = style[1];
      shape.opacity = style[2];
    }
    else if (shape.type == 'encoded_polygon') {
      jQuery.each(shape.polylines, function(i, polyline) {
        polyline.color = style[0];
        polyline.weight = style[1];
        polyline.opacity = style[2];
      });
      shape.fill = true;
      shape.color = style[3];
      shape.opacity = style[4];
      shape.outline = true;
    }

    jQuery.each(style, function (i, n) {
      cargs.push(n);
    });
    if (shape.opts) {
      cargs.push(shape.opts);
    }
    var Pg = function (args) {
      GPolygon.apply(this, args);
    };
    Pg.prototype = new GPolygon();
    var Pl = function (args) {
      GPolyline.apply(this, args);
    };
    Pl.prototype = new GPolyline();
    switch (shape.type) {
      case 'circle':
      case 'polygon':
      case 'rpolygon':
        shape.shape = new Pg(cargs);
        break;
      case 'line':
        shape.shape = new Pl(cargs);
        break;
      case 'encoded_line':
        shape.shape = GPolyline.fromEncoded(shape);        
        break;
      case 'encoded_polygon':
        shape.shape = GPolygon.fromEncoded(shape);
        break;
    }
  });

  obj.bind('addshape', function (shape) {
    if (!obj.vars.shapes) {
      obj.vars.shapes = [];
    }
    obj.vars.shapes.push(shape);
    obj.map.addOverlay(shape.shape);

    if (obj.vars.behavior.clickableshapes) {
      GEvent.addListener(shape.shape, 'click', function () {
        obj.change('clickshape', -1, shape);
      });
    }
  });

  obj.bind('delshape', function (shape) {
    obj.map.removeOverlay(shape.shape);
  });

  obj.bind('clearshapes', function () {
    if (obj.vars.shapes) {
      jQuery.each(obj.vars.shapes, function (i, n) {
        obj.change('delshape', -1, n);
      });
    }
  });
});
