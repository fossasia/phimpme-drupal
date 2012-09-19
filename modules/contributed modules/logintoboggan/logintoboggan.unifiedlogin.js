
(function ($) {

Drupal.behaviors.unifiedLogin = {

  attach: function (context) {
    // Attach behaviors to the links so that they show/hide forms appropriately.
    $('.toboggan-unified #register-link').click(function() {
      $(this).addClass('lt-active').blur();
      $('.toboggan-unified #login-link').removeClass('lt-active');
      $('.toboggan-unified #register-form').show();
      $('.toboggan-unified #login-form').hide();
      return false;
    });
    $('.toboggan-unified #login-link').click(function() {
      $(this).addClass('lt-active').blur();
      $('.toboggan-unified #register-link').removeClass('lt-active');
      $('.toboggan-unified #login-form').show();
      $('.toboggan-unified #register-form').hide();
      return false;
    });

    switch(Drupal.settings.LoginToboggan.unifiedLoginActiveForm) {
      case 'register':
        $('.toboggan-unified #register-link').click();
        break;
      case 'login':
      default:
        $('.toboggan-unified #login-link').click();
        break;
    }
  }
};

})(jQuery);

