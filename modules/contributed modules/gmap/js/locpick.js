
/**
 * @file
 * Location chooser interface.
 */

/*global $, Drupal, GEvent, GLatLng, GMarker */

(function ($) {
Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  var binding = obj.bind("locpickchange", function () {
    if (obj.locpick_coord) {
      GEvent.trigger(obj.map, "click", null, obj.locpick_coord);
    }
  });

  obj.bind("locpickremove", function () {
    obj.map.removeOverlay(obj.locpick_point);
    obj.locpick_point = null;
    obj.locpick_coord = null;
    obj.change('locpickchange', -1);
  });

  obj.bind("init", function () {
    if (obj.vars.behavior.locpick) {
      obj.locpick_coord = new GLatLng(obj.vars.latitude, obj.vars.longitude);

      GEvent.addListener(obj.map, "click", function (overlay, point) {
        obj.map.checkResize();
        if (!overlay) {
          if (!obj.locpick_point) {
            obj.map.addOverlay(obj.locpick_point = new GMarker(point, {draggable: true}));
          }
          obj.locpick_point.setLatLng(point);
          GEvent.addListener(obj.locpick_point, 'drag', function () {
            obj.locpick_coord = obj.locpick_point.getLatLng();
            obj.change('locpickchange', binding);
          });
          GEvent.addListener(obj.locpick_point, 'dragend', function () {
            obj.locpick_coord = obj.locpick_point.getLatLng();
            obj.change('locpickchange', binding);
          });
          obj.locpick_coord = point;
          obj.map.panTo(point);
          obj.change('locpickchange', binding);
        }
        else {
          // Unsetting the location
          obj.change('locpickremove', -1);
        }
      });
    }
  });

  obj.bind("ready", function () {
    // Fake a click to set the initial point, if one was set.
    if (obj.vars.behavior.locpick) {
      if (!obj.locpick_invalid) {
        obj.change('locpickchange', -1);
      }
    }
  });

});

Drupal.gmap.addHandler('locpick_latitude', function (elem) {
  var obj = this;

  obj.bind("init", function () {
    if (elem.value !== '') {
      obj.vars.latitude = Number(elem.value);
      obj.locpick_coord = new GLatLng(obj.vars.latitude, obj.vars.longitude);
    }
    else {
      obj.locpick_coord = null;
      obj.locpick_invalid = true;
    }
  });

  var binding = obj.bind("locpickchange", function () {
    if (obj.locpick_coord) {
      elem.value = obj.locpick_coord.lat();
    }
    else {
      elem.value = '';
    }
  });

  $(elem).change(function () {
    if (elem.value !== '') {
      if (obj.locpick_coord) {
        obj.locpick_coord = new GLatLng(Number(elem.value), obj.locpick_coord.lng());
        obj.change('locpickchange', binding);
      }
      else {
        obj.locpick_coord = new GLatLng(Number(elem.value), 0.0);
      }
    }
    else {
      obj.change('locpickremove', -1);
    }
  });
});

Drupal.gmap.addHandler('locpick_longitude', function (elem) {
  var obj = this;

  obj.bind("init", function () {
    if (elem.value !== '') {
      obj.vars.longitude = Number(elem.value);
      obj.locpick_coord = new GLatLng(obj.vars.latitude, obj.vars.longitude);
    }
    else {
      obj.locpick_invalid = true;
    }
  });

  var binding = obj.bind("locpickchange", function () {
    if (obj.locpick_coord) {
      elem.value = obj.locpick_coord.lng();
    }
    else {
      elem.value = '';
    }
  });

  $(elem).change(function () {
    if (elem.value !== '') {
      if (obj.locpick_coord) {
        obj.locpick_coord = new GLatLng(obj.locpick_coord.lat(), Number(elem.value));
        obj.change('locpickchange', binding);
      }
      else {
        obj.locpick_coord = new GLatLng(0.0, Number(elem.value));
      }
    }
    else {
      obj.change('locpickremove', -1);
    }
  });
});
})(jQuery);
