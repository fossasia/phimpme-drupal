

Drupal.gmap.addHandler('gmap',function(elem) {
  var obj = this;

  obj.bind("bootstrap_options", function() {
    var opts = obj.opts;

    if (obj.vars.baselayers['G_MOON_ELEVATION_MAP']) {
      opts.mapTypes.push(G_MOON_ELEVATION_MAP);
      opts.mapTypeNames.push('G_MOON_ELEVATION_MAP');
    }
    if (obj.vars.baselayers['G_MOON_VISIBLE_MAP']) {
      opts.mapTypes.push(G_MOON_VISIBLE_MAP);
      opts.mapTypeNames.push('G_MOON_VISIBLE_MAP');
    }

    if (obj.vars.baselayers['G_MARS_ELEVATION_MAP']) {
      opts.mapTypes.push(G_MARS_ELEVATION_MAP);
      opts.mapTypeNames.push('G_MARS_ELEVATION_MAP');
    }
    if (obj.vars.baselayers['G_MARS_VISIBLE_MAP']) {
      opts.mapTypes.push(G_MARS_VISIBLE_MAP);
      opts.mapTypeNames.push('G_MARS_VISIBLE_MAP');
    }
    if (obj.vars.baselayers['G_MARS_INFRARED_MAP']) {
      opts.mapTypes.push(G_MARS_INFRARED_MAP);
      opts.mapTypeNames.push('G_MARS_INFRARED_MAP');
    }

    if (obj.vars.baselayers['G_SKY_VISIBLE_MAP']) {
      opts.mapTypes.push(G_SKY_VISIBLE_MAP);
      opts.mapTypeNames.push('G_SKY_VISIBLE_MAP');
    }

  });
});
