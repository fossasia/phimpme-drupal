(function ($) {

Drupal.quicktabsShowHide = function() {
  $(this).parents('tr').find('div.qt-tab-' + this.value + '-options-form').show().siblings('div.qt-tab-options-form').hide();
};

Drupal.behaviors.quicktabsform = {
  attach: function (context, settings) {
    $('#quicktabs-form tr').once(function(){
      var currentRow = $(this);
      currentRow.find('div.form-item :input[name*="type"]').bind('click', Drupal.quicktabsShowHide);
      $(':input[name*="type"]:checked', this).trigger('click');
    })
  }
};

})(jQuery);
