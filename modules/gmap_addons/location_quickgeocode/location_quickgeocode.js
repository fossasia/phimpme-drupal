
Drupal.behaviors.location_quickgeocode = function (context) {
  $('div.location_quickgeocode:not(.location-quickgeocode-processed)', context).addClass('location-quickgeocode-processed').each(function() {
    $(this).html(Drupal.t('[ Quick Geocode ]'));
    var fieldset = $(this).parents('fieldset.location');

    // Get the map in a totally roundabout way.
    var obj = false;
    if ($(fieldset).find('div.gmap-map').length) {
      var mapid = $(fieldset).find('div.gmap-map')[0].id;
      setTimeout(function() {
        obj = Drupal.gmap.getMap(mapid);
      }, 0);
    }

    $(this).click(function() {
      if (obj) {
        var location = {}
        $(fieldset).find('input,select').each(function() {
          if (this.name) {
            location[this.name.match(/([^\[]*)\]$/).pop()] = $(this).val();
          }
        });
        $.ajax({
          url: Drupal.settings.basePath + 'location_quickgeocode',
          dataType: 'json',
          data: location,
          success: function(data) {
            if (data && data.lat && data.lon) {
              obj.locpick_coord = new GLatLng(data.lat, data.lon);
              obj.change("locpickchange", -1);
            }
            else {
              alert (Drupal.t('Sorry, not found.'));
            }
          }
        });
      }
    });
  });
};
