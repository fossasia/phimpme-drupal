(function ($) {

  Drupal.behaviors.mediaGalleryEdit = Drupal.behaviors.mediaGalleryEdit || {};

  Drupal.behaviors.mediaGalleryEdit.attach = function (context, settings) {
    // Get the set of remove checkboxes
    $('.form-type-checkbox[class *= "remove"]').bind('change', function (event) {
      // Get the value of the checkbox
      var isChecked = event.target.checked;
      // Get the containing media item
      var mediaItem = $(this).closest('.media-edit-form');
      // Get the inputs and wrapping form items in the media item
      var mediaItemFields = mediaItem.find('.sidebar').nextAll().not('.form-actions');
      var inputs = mediaItemFields.find(':input');
      // If the checkbox is checked, disabled the form elements in the media item;
      if (isChecked) {
        mediaItemFields.addClass('disabled');
      }
      // else remove the disabled attribute and styling.
      else {
        mediaItemFields.removeClass('disabled');
      }
    });
  };
})(jQuery);