
(function ($) {
namespace('Drupal.media.browser.plugin');

Drupal.media.browser.plugin.upload = function (mediaBrowser, options) {
  return {
    /* Abstract */
    init: function () {
      tabset = mediaBrowser.getTabset();
      tabset.tabs('add', '#upload', Drupal.t('Upload'));
      mediaBrowser.listen('tabs.show', function (e, id) {
        if (id == 'upload') {
          // We only need to set this once.
          // We probably could set it upon load.
          if (mediaBrowser.getActivePanel().html() == '') {
            mediaBrowser.getActivePanel().html(options.uploadForm);
          }
        }
      });
    }
  };
};

// For now, I guess self registration makes sense.
// Really though, we should be doing it via drupal_add_js and some settings
// from the drupal variable.
// @todo: needs a review.
Drupal.media.browser.register('upload', Drupal.media.browser.plugin.upload, {});

})(jQuery);
