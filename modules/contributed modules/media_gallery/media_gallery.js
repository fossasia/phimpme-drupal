(function ($) {

Drupal.behaviors.media_gallery = {};

Drupal.behaviors.media_gallery.attach = function (context, settings) {
  $(window).bind('media_youtube_load', Drupal.media_gallery.handleMediaYoutubeLoad);
};

Drupal.media_gallery = {};

Drupal.media_gallery.handleMediaYoutubeLoad = function (event, videoSettings) {
  $('.media-gallery-detail').width(videoSettings.width + 'px');
};

})(jQuery);