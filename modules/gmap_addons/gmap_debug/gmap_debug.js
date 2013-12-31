
Drupal.behaviors.gmap_debug = function (context) {
  $('#gmap-debug-forcesizecheck:not(.gmap-debug-processed)', context).addClass('gmap-debug-processed').each(function () {
    $(this).click(function(e) {
      e.preventDefault();
      if (Drupal.gmap) {
        Drupal.gmap.globalChange('widthchange');
        alert(Drupal.t('Dimensions recalculated.'));
      }
      else {
        alert(Drupal.t('No maps loaded.'));
      }
      return false;
    });
  });
  $('#gmap-debug-startup:not(.gmap-debug-processed)', context).addClass('gmap-debug-processed').each(function () {
    $(this).click(function(e) {
      e.preventDefault();
      Drupal.attachBehaviors(document);
      return false;
    });
  });
  $('#gmap-debug-shutdown:not(.gmap-debug-processed)', context).addClass('gmap-debug-processed').each(function () {
    $(this).click(function(e) {
      e.preventDefault();
      var reply = prompt('Map ID to shut down?');
      if (Drupal.gmap && Drupal.gmap.getMap(reply)) {
        Drupal.gmap.unloadMap(reply);
      }
      else {
        alert(Drupal.t('Unable to locate requested map.'));
      }
      return false;
    });
  });
  $('#gmap-debug-reboot:not(.gmap-debug-processed)', context).addClass('gmap-debug-processed').each(function () {
    $(this).click(function(e) {
      e.preventDefault();
      var reply = prompt('Map ID to reboot?');
      if (Drupal.gmap && Drupal.gmap.getMap(reply)) {
        Drupal.gmap.unloadMap(reply);
        Drupal.attachBehaviors(document);
      }
      else {
        alert(Drupal.t('Unable to locate requested map.'));
      }
      return false;
    });
  });
};
