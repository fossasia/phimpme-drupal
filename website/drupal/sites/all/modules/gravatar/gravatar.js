// $Id: gravatar.js,v 1.3 2010/05/29 20:25:55 davereid Exp $

(function ($) {

Drupal.behaviors.gravatarPreview = {
  attach: function (context) {
    $('input[name=gravatar_default]', context).once('gravatarPreview', function () {
      $(this).bind('change', function() {
        var selected_image = $('img#gravatar-imagepreview-' + $(this).val());
        $('img#gravatar-imagepreview').attr('src', selected_image.attr('src'));
      });
    });

    $(document).ready(function () {
      var selected_index = $('input[name=gravatar_default][checked]').val();
      var selected_image = $('img#gravatar-imagepreview-' + selected_index);
      $('img#gravatar-imagepreview').attr('src', selected_image.attr('src'));
      $('.js-show').show();
    });
  }
};

})(jQuery);
