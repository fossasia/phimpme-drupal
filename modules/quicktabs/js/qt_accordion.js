(function ($) {

Drupal.behaviors.qt_accordion = {
  attach: function (context, settings) {
    $('.quick-accordion', context).once(function(){
      var id = $(this).attr('id');
      var qtKey = 'qt_' + this.id.substring(this.id.indexOf('-') +1);
      var options = settings.quicktabs[qtKey].options;

      options.active = parseInt(settings.quicktabs[qtKey].active_tab);
      if (settings.quicktabs[qtKey].history) {
        options.event = 'change';
        $(this).accordion(options);
        Drupal.quicktabsBbq($(this), 'h3 a', 'h3');
      }
      else {
        $(this).accordion(options);
      }
    });
  }
}

})(jQuery);
