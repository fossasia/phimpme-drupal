
/**
 *  @file
 *  Attach behaviors to formatter radio select when selecting a media's display
 *  formatter.
 */

(function ($) {
namespace('Drupal.media.formatForm');

Drupal.media.mediaFormatSelected = {};

Drupal.behaviors.mediaFormatForm = {
  attach: function (context, settings) {
    // Add "Submit" and "Cancel" buttons inside the IFRAME that trigger the
    // behavior of the hidden "OK" and "Cancel" buttons that are outside the
    // IFRAME. See Drupal.media.browser.validateButtons() for more details.
    $('<a class="button fake-ok">' + Drupal.t('Submit') + '</a>').appendTo($('#media-format-form')).bind('click', Drupal.media.formatForm.submit);
    $('<a class="button fake-cancel">' + Drupal.t('Cancel') + '</a>').appendTo($('#media-format-form')).bind('click', Drupal.media.formatForm.submit);

    if (Drupal.settings.media_format_form.autosubmit) {
      $('.button.fake-ok').click();
    }
  }
};

Drupal.media.formatForm.getOptions = function () {
  // Get all the values
  var ret = {}; $.each($('#media-format-form fieldset#edit-options *').serializeArray(), function (i, field) { ret[field.name] = field.value; });
  return ret;
};

Drupal.media.formatForm.getFormattedMedia = function () {
  var formatType = $("select#edit-format option:selected").val();
  return { type: formatType, options: Drupal.media.formatForm.getOptions(), html: Drupal.settings.media.formatFormFormats[formatType] };
};

Drupal.media.formatForm.submit = function () {
  // @see Drupal.behaviors.mediaFormatForm.attach().
  var buttons = $(parent.window.document.body).find('#mediaStyleSelector').parent('.ui-dialog').find('.ui-dialog-buttonpane button');
  if ($(this).hasClass('fake-cancel')) {
    buttons[1].click();
  } else {
    buttons[0].click();
  }
}

})(jQuery);
