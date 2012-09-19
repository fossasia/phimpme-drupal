
/**
 * @file
 * Alignment widget.
 * Applies CSS classes to a macro.
 */

/*global jQuery, Drupal */
(function ($) {
Drupal.gmap.addHandler('align', function (elem) {
  var obj = this;
  // Respond to incoming alignment changes.
  var binding = obj.bind("alignchange", function () {
    elem.value = obj.vars.align;
  });
  // Send out outgoing alignment changes.
  $(elem).change(function () {
    obj.vars.align = elem.value;
    obj.change("alignchange", binding);
  });
});

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;
  // Respond to incoming alignment changes.
  obj.bind("alignchange", function () {
    var cont = obj.map.getContainer();
    $(cont)
      .removeClass('gmap-left')
      .removeClass('gmap-center')
      .removeClass('gmap-right');
    if (obj.vars.align === 'Left') {
      $(cont).addClass('gmap-left');
    }
    if (obj.vars.align === 'Center') {
      $(cont).addClass('gmap-center');
    }
    if (obj.vars.align === 'Right') {
      $(cont).addClass('gmap-right');
    }
  });
  // Send out outgoing alignment changes.
  // N/A

  obj.bind('buildmacro', function (add) {
    if (obj.vars.align && obj.vars.align !== 'None') {
      add.push('align=' + obj.vars.align);
    }
  });
});
})(jQuery);