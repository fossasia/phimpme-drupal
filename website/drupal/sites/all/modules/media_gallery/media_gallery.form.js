(function ($) {

Drupal.behaviors.menuFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.block-form', context).drupalSetSummary(function (context) {
      if ($('#edit-media-gallery-expose-block-und', context).attr('checked')) {
        return Drupal.t('Enabled');
      }
      else {
        return Drupal.t('Not enabled');
      }
    });
  }
};

Drupal.behaviors.media_gallery_form = {};
Drupal.behaviors.media_gallery_form.attach = function (context, settings) {
  // Change the "Presentation settings" image to match the radio buttons / checkbox.
  var inputs = $('.presentation-settings input', context);
  if (inputs.length) {
    inputs.bind('change', Drupal.behaviors.media_gallery_form.format_select);
    Drupal.behaviors.media_gallery_form.format_select();
  }
};

Drupal.behaviors.media_gallery_form.format_select = function (event) {
  var radioValue = $('.presentation-settings input:radio:checked').val();
  var icon = $('.presentation-settings .setting-icon');
  var checkbox = $('.presentation-settings .field-name-media-gallery-lightbox-extras input');

  // Depending on the radio button chosen add a class
  if (radioValue == 'node') {
    icon.attr('class', 'setting-icon display-page');
    // Disable the checkbox
    checkbox.attr('disabled', true);
  } else {
    icon.attr('class', 'setting-icon display-lightbox');
    // Turn on the checkbox
    checkbox.attr('disabled', false);
    // Add a class if the checkbox is checked
    if (checkbox.is(':checked')) {
      icon.attr('class', 'setting-icon display-extras');
    }
  }
};

})(jQuery);
