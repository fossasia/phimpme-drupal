
(function ($) {
namespace('Drupal.media.browser.plugin');

Drupal.media.browser.plugin.fromurl = function (mediaBrowser, options) {
  return {
    init: function () {
      tabset = mediaBrowser.getTabset();
      tabset.tabs('add', '#fromurl', 'From URL');
      mediaBrowser.listen('tabs.show', function (e, id) {
        if (id == 'fromurl') {
          // We only need to set this once.
          // We probably could set it upon load.
          if (mediaBrowser.getActivePanel().html() == '') {
            mediaBrowser.getActivePanel().html(options.fromUrlForm);
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
Drupal.media.browser.register('fromurl', Drupal.media.browser.plugin.fromurl, {});

})(jQuery);
