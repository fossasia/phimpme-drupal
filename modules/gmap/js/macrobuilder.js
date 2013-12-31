
/**
 * @file
 * Map ID widget for macro form.
 */

/*global jQuery, Drupal */

Drupal.gmap.addHandler('mapid', function (elem) {
  var obj = this;
  // Respond to incoming map id changes.
  var binding = obj.bind("idchange", function () {
    elem.value = obj.vars.macro_mapid;
  });
  // Send out outgoing map id changes.
  jQuery(elem).change(function () {
    obj.vars.macro_mapid = elem.value;
    obj.change("idchange", binding);
  });
});
