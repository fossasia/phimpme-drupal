
/**
 * @file
 * GMap macro widget and macro compiler.
 */

/*global Drupal */

/**
 * Widget handler.
 */
Drupal.gmap.addHandler('macrotext', function (elem) {
  var obj = this;
  obj.macrostorage = {};

  obj.bind("widthchange", function (w) {
    obj.macrostorage.width = w;
  });
  obj.bind("heightchange", function (h) {
    obj.macrostorage.height = h;
  });

  // Basic macros.
  obj.bind('buildmacro', function (add) {
    add.push('zoom=' + obj.vars.zoom);
    add.push('center=' + obj.vars.latitude + ',' + obj.vars.longitude);
    add.push('width=' + obj.macrostorage.width);
    add.push('height=' + obj.macrostorage.height);
    if (obj.vars.macro_mapid && obj.vars.macro_mapid !== '') {
      add.push('id=' + obj.vars.macro_mapid);
    }
    add.push('control=' + obj.vars.controltype);
    // @@@ Use maptype instead, accept old and new.
    add.push('type=' + obj.vars.maptype);
  });


  // Update macro every time something happens.
  obj.bind('all', function (name) {
    if (name !== 'buildmacro') {
      var add = [];
      // Collect macro pieces.
      obj.change('buildmacro', -1, add);
      elem.value = '[gmap ' + add.join(' |') + ']';
    }
  });
});

/**
 * Extensible macros @@@
 */
Drupal.gmap.map.prototype.parse = function (m) {
  // Trim off outside tag
  if (m.substr(0, 5) === '[gmap') {
    m = m.slice(6, -1);
  }
};
