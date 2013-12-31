
/**
 * Twiddle the province autocomplete whenever the user changes the country.
 */
(function ($) {
Drupal.behaviors.location = {
  attach: function(context) {
    $('select.location_auto_country:not(.location-processed)', context).change(function(e) {
      var obj = this;
      var input = null;
      var result = this.className.match(/(location_auto_join_[^ ]*)/);
      if (result) {
        input = $('.location_auto_province.' + result)
      }
      else {
        // No joining class found, fallback to searching the immediate area.
        input = $('.location_auto_province', $(this).parents('fieldset:first, .views-exposed-form:first'))
      }

      if (input && input.length) {
        //Unbind events on province field and empty its value
        input.unbind().val('');
        input.each(function(i) {
          //Get the (hidden) *-autocomplete input element
          var input_autocomplete = $('#' + this.id + '-autocomplete');
          // Update autocomplete url
          input_autocomplete.val(input_autocomplete.val().substr(0, input_autocomplete.val().lastIndexOf('/') + 1) + $(obj).val());
          // Mark as not processed.
          input_autocomplete.removeClass('autocomplete-processed');
        });
        // Reprocess.
        Drupal.behaviors.autocomplete.attach(document);
      }
    }).addClass('location-processed');
  }
};
})(jQuery);