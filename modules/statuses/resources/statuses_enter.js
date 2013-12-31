(function ($) {

Drupal.behaviors.statuses_enter = {
  attach: function (context) {
    var ctxt = $(context);
    var shift = false;
    ctxt.find('.statuses-text').keydown(function(e) {
      if (e.which == 16) {
        shift = true;
      }
    });
    ctxt.find('.statuses-text').keyup(function(e) {
      if (e.which == 16) {
        shift = false;
      }
    });
    ctxt.find('.statuses-text').keypress(function(e) {
      // Submit the form (via AHAH if possible) when the user hits Shift+Enter.
      if (e.which == 13 && shift && $(this).val().length) {
        e.preventDefault();
        var $form = $(this).parents('form');
        var $element = $form.find('.statuses-submit');
        if (Drupal.settings.ahah && Drupal.settings.ahah[$element[0].id]) {
          $element.trigger(Drupal.settings.ahah[$element[0].id].event);
        }
        else {
          $form.submit();
        }
      }
    });
  }
}

})(jQuery);
