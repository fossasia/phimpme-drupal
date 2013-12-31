
/**
 * @file
 * Gmap Overlay Editor
 */

/*global jQuery, Drupal, GEvent, GMarker, GPolygon, GPolyline */
Drupal.gmap.addHandler('overlayedit_linestyle', function (elem) {
  var obj = this;
  obj.vars.styles.overlayline = [];
  var f = function () {
    var o = Number(jQuery(this).attr('id').match(/\d+$/));
    obj.vars.styles.overlayline[o] = this.value;
  };
  jQuery(elem).find('input.gmap_style').change(f).each(f);
});

Drupal.gmap.addHandler('overlayedit_linestyle_apply', function (elem) {
  var obj = this;
  obj.vars.overlay_linestyle_apply = Boolean(elem.checked);
  jQuery(elem).change(function () {
    obj.vars.overlay_linestyle_apply = Boolean(this.checked);
  });
});

Drupal.gmap.addHandler('overlayedit_polystyle', function (elem) {
  var obj = this;
  obj.vars.styles.overlaypoly = [];
  var f = function () {
    var o = Number(jQuery(this).attr('id').match(/\d+$/));
    obj.vars.styles.overlaypoly[o] = this.value;
  };
  jQuery(elem).find('input.gmap_style').change(f).each(f);
});

Drupal.gmap.addHandler('overlayedit_polystyle_apply', function (elem) {
  var obj = this;
  obj.vars.overlay_polystyle_apply = Boolean(elem.checked);
  jQuery(elem).change(function () {
    obj.vars.overlay_polystyle_apply = Boolean(this.checked);
  });
});

Drupal.gmap.addHandler('overlayedit_fillstroke_default', function (elem) {
  var obj = this;
  obj.vars._usedefaultfillstroke = Boolean(elem.checked);
  jQuery(elem).change(function () {
    obj.vars._usedefaultfillstroke = Boolean(this.checked);
    alert(obj.vars._usedefaultfillstroke);
  });
});

Drupal.gmap.addHandler('overlayedit_mapclicktype', function (elem) {
  var obj = this;
  obj.vars.overlay_add_mode = elem.value;
  jQuery(elem).change(function () {
    obj.vars.overlay_add_mode = elem.value;
    if (obj.temp_point) {
      delete obj.temp_point;
    }
  });
});
Drupal.gmap.addHandler('overlayedit_markerclicktype', function (elem) {
  var obj = this;
  obj.vars.overlay_del_mode = elem.value;
  jQuery(elem).change(function () {
    obj.vars.overlay_del_mode = elem.value;
  });
});

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  // Add status bar
  var status = jQuery(elem).after('<div class="gmap-statusbar">Status</div>').next();
  obj.statusdiv = status[0];

  obj.bind('buildmacro', function (add) {
    var temp, i, q, tm, ct;

    var style_line = function (n) {
      if (!n.style.length) {
        return '';
      }
      var style = n.style.slice(0, 3);
      style[0] = '#' + style[0];
      return style.join('/') + ':';
    };
    var style_poly = function (n) {
      if (!n.style.length) {
        return '';
      }
      var style = n.style.slice();
      style[0] = '#' + style[0];
      style[3] = '#' + style[3];
      return style.join('/') + ':';
    };

    var feature_dump = function (n) {
      var f = n.overlay;
      var tmp = [];
      var i, ct, vtx;
      ct = f.getVertexCount();
      for (i = 0; i < ct; i++) {
        vtx = f.getVertex(i);
        tmp.push('' + vtx.lat() + ',' + vtx.lng());
      }
      return tmp.join(' + ');
    };

    if (obj._oe && obj._oe.features) {
      var polygons = [];
      var polylines = [];
      var circles = [];
      var markers = {};
      jQuery.each(obj._oe.features, function (i, n) {
        if (n.type) {
          switch (n.type) {
            case 'polyline':
              add.push('line=' + style_line(n) + feature_dump(n));
              break;
            case 'polygon':
              add.push('polygon=' + style_poly(n) + feature_dump(n));
              break;
            case 'point':
              if (!markers[n.marker]) {
                markers[n.marker] = [];
              }
              var pt = n.overlay.getLatLng();
              var ptxt = '';
              if (n.html) {
                ptxt = ':' + n.html;
              }
              markers[n.marker].push('' + pt.lat() + ',' + pt.lng() + ptxt);
              break;
            case 'circle':
              add.push('circle=' + style_poly(n) + n.center.lat() + ' , ' + n.center.lng() + ' + ' + n.radius / 1000);
              break;
          }
        }
      });
      jQuery.each(markers, function (i, n) {
        add.push('markers=' + i + '::' + n.join(' + '));
      });
    }
  });
});

Drupal.gmap.map.prototype.statusdiv = undefined;

Drupal.gmap.map.prototype.status = function (text) {
  var obj = this;
  if (obj.statusdiv) {
    jQuery(obj.statusdiv).html(text);
  }
};

// Extend markers to store type info.
GMarker.prototype.gmapMarkerData = function (data) {
  if (data) {
    this._gmapdata = data;
  }
  return this._gmapdata;
};

/************* Overlay edit widget ******************/
Drupal.gmap.addHandler('overlayedit', function (elem) {
  var obj = this;

  var binding = obj.bind('overlay_edit_mode', function () {
    // @@@
  });

  jQuery(elem).change(function () {
    obj.vars.overlay_next_icon = elem.value;
//    obj.vars.overlay_edit_mode = elem.value;
//    obj.change('overlay_edit_mode',binding);
  });

  obj.bind('init', function () {
    obj._oe = {};
    obj.vars.overlay_add_mode = 'Points'; //elem.value;
    obj.vars.overlay_del_mode = 'Remove';
    var edit_text_elem;

    if (obj.map) {
      obj._oe.features = [];
      obj._oe.featuresRef = {};
      obj._oe.editing = false;
      obj._oe.markerseq = {};
      GEvent.addListener(obj.map, 'click', function (overlay, point) {
        var ctx, s, p;
        if (overlay) {
          if (obj._oe.editing) {
            // Work around problem where double clicking to finish a poly fires a click event.
            obj._oe.editing = false;
          }
          else {
          }
        }
        else if (point && !obj._oe.editing) {
          obj._oe.editing = true;
          switch (obj.vars.overlay_add_mode) {
            case 'Points':
              var m = elem.value; // @@@ It's kinda silly to be binding the whole shebang to this dropdown..
              if (!obj._oe.markerseq.hasOwnProperty(m)) {
                obj._oe.markerseq[m] = -1;
              }
              obj._oe.markerseq[m] = obj._oe.markerseq[m] + 1;
              p = new GMarker(point, {icon: Drupal.gmap.getIcon(m, obj._oe.markerseq[m])});
              obj.map.addOverlay(p);
              ctx = {
                'type' : 'point',
                'marker' : m,
                'overlay' : p
              };
              var offset = obj._oe.features.push(ctx) - 1;
              obj._oe.editing = false;
              GEvent.addListener(p, "click", function () {
                switch (obj.vars.overlay_del_mode) {
                  case 'Remove':
                    obj._oe.markerseq[m] = obj._oe.markerseq[m] - 1;
                    ctx.type = 'deleted';
                    obj.map.removeOverlay(p);
                    ctx.overlay = null;
                    var tmpcnt = 0;
                    // Renumber markers in set.
                    jQuery.each(obj._oe.features, function (i, n) {
                      if (n.type && n.type === 'point' && n.marker === m) {
                        var pt = n.overlay.getLatLng();
                        n.overlay.setImage(Drupal.gmap.getIcon(n.marker, tmpcnt).image);
                        tmpcnt = tmpcnt + 1;
                      }
                    });
                    break;
                  case 'Edit info':
                    // @@@
                    break;
                }
                obj.change('mapedited', -1);
              });
              obj.change('mapedited', -1);
              break;

            case 'Lines':
              ctx = {
                'type' : 'polyline',
                'style' : [],
                'overlay' : null
              };
              s = obj.vars.styles.line_default;
              if (obj.vars.overlay_linestyle_apply) {
                ctx.style = obj.vars.styles.overlayline.slice();
                s = ctx.style;
              }
              p = new GPolyline([point], '#' + s[0], Number(s[1]), s[2] / 100);
              obj.map.addOverlay(p);
              ctx.overlay = p;
              obj._oe.featuresRef[p] = obj._oe.features.push(ctx) - 1;

              p.enableDrawing();
              p.enableEditing({onEvent: "mouseover"});
              p.disableEditing({onEvent: "mouseout"});
              GEvent.addListener(p, "endline", function () {
                //obj._oe.editing = false;
                GEvent.addListener(p, "lineupdated", function () {
                  obj.change('mapedited', -1);
                });
                GEvent.addListener(p, "click", function (latlng, index) {
                  if (typeof index === "number") {
                    // Delete vertex on click.
                    p.deleteVertex(index);
                  }
                  else {
                    var feature = obj._oe.features[obj._oe.featuresRef[p]];
                    feature.stroke = obj.vars.stroke; // @@@
                    p.setStrokeStyle(feature.stroke);
                  }
                });
                obj.change('mapedited', -1);
              });
              break;

            case 'GPolygon':
              ctx = {
                'type' : 'polygon',
                'style' : [],
                'overlay' : null
              };
              s = obj.vars.styles.poly_default;
              if (obj.vars.overlay_polystyle_apply) {
                ctx.style = obj.vars.styles.overlaypoly.slice();
                s = ctx.style;
              }
              p = new GPolygon([point], '#' + s[0], Number(s[1]), s[2] / 100, '#' + s[3], s[4] / 100);
              obj.map.addOverlay(p);
              ctx.overlay = p;
              obj._oe.featuresRef[p] = obj._oe.features.push(ctx) - 1;

              p.enableDrawing();
              p.enableEditing({onEvent: "mouseover"});
              p.disableEditing({onEvent: "mouseout"});
              GEvent.addListener(p, "endline", function () {
                //obj._oe.editing = false;
                GEvent.addListener(p, "lineupdated", function () {
                  obj.change('mapedited', -1);
                });
                GEvent.addListener(p, "click", function (latlng, index) {
                  if (typeof index === "number") {
                    p.deleteVertex(index);
                  }
                  else {
                    var feature = obj._oe.features[obj._oe.featuresRef[p]];
                    feature.stroke = obj.vars.stroke;
                    feature.fill = obj.vars.fill;
                    p.setStrokeStyle(feature.stroke);
                    p.setFillStyle(feature.fill); // @@@
                  }
                });
                obj.change('mapedited', -1);
              });
              break;

            case 'Circles':
              var temppoint = point;
              // @@@ Translate
              obj.status("Drawing circle. Click a point on the rim to place.");

              var handle = GEvent.addListener(obj.map, 'click', function (overlay, point) {
                if (point) {
                  var ctx = {
                    'type' : 'circle',
                    'center' : temppoint,
                    'radius' : null,
                    'style' : [],
                    'overlay' : null
                  };
                  var s = obj.vars.styles.poly_default;
                  if (obj.vars.overlay_polystyle_apply) {
                    ctx.style = obj.vars.styles.overlaypoly.slice();
                    s = ctx.style;
                  }
                  obj.status("Placed circle. Radius was " + temppoint.distanceFrom(point) / 1000 + " km.");
                  ctx.radius = temppoint.distanceFrom(point);
                  var p = new GPolygon(obj.poly.calcPolyPoints(ctx.center, ctx.radius, 32), '#' + s[0], Number(s[1]), s[2] / 100, '#' + s[3], s[4] / 100);
                  obj.map.addOverlay(p);
                  ctx.overlay = p;
                  obj._oe.featuresRef[p] = obj._oe.features.push(ctx) - 1;
                  GEvent.addListener(p, "click", function () {
                    switch (obj.vars.overlay_del_mode) {
                      case 'Remove':
                        ctx.type = 'deleted';
                        obj.map.removeOverlay(p);
                        ctx.overlay = null;
                        break;
                      case 'Edit info':
                        // @@@
                        break;
                    }
                    obj.change('mapedited', -1);
                  });
                }
                else {
                  // @@@ Uh, do cleanup I suppose..
                }
                obj._oe.editing = false;
                GEvent.removeListener(handle);
                obj.change('mapedited', -1);
              });
              break;
          }
        }
      });
    }
  });
});