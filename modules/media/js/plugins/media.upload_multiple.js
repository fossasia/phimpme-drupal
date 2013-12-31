Drupal.behaviors.mediaUploadMultiple = {};

Drupal.behaviors.mediaUploadMultiple.attach = function (context, settings) {
  // When the plupload element initializes, it expands the size of the elements
  // it has created, so we need to resize the browser iframe after it's done.
  var uploader = jQuery('#edit-upload').pluploadQueue();
  if (uploader) {
    // Handle the case in which the uploader has already finished initializing.
    Drupal.media.browser.resizeIframe();
    // Handle the case in which the uploader has not yet initialized.
    uploader.bind("PostInit", Drupal.media.browser.resizeIframe);

    uploader.bind('StateChanged', Drupal.behaviors.mediaUploadMultiple.submit);
  }
};

Drupal.behaviors.mediaUploadMultiple.submit = function (uploader, file) {
  if (uploader.state == 2) {
    jQuery('#media-add-upload-multiple .form-submit').val(Drupal.t('Loading...'));
  }
};
