Drupal.behaviors.fbss_comments_enter = function(context) {
  var ctxt = $(context);
  var shift = false;
  ctxt.find('.fbss-comments-textarea').keydown(function(e) {
    if (e.which == 16) {
      shift = true;
    }
  });
  ctxt.find('.fbss-comments-textarea').keyup(function(e) {
    if (e.which == 16) {
      shift = false;
    }
  });
  ctxt.find('.fbss-comments-textarea').keypress(function(e) {
    // Submit the form (via AHAH if possible) when the user hits Enter (but not Shift+Enter).
    if (e.which == 13 && !shift && $(this).val().length) {
      var $form = $(this).parents('form');
      var $element = $form.find('.fbss-comments-submit');
      if (Drupal.settings.ahah && Drupal.settings.ahah[$element[0].id]) {
        $element.trigger(Drupal.settings.ahah[$element[0].id].event);
      }
      else {
        $form.submit();
      }
    }
  });
}
