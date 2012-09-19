/**
 * @file
 * Implements history using the BBQ plugin.
 * See http://benalman.com/code/projects/jquery-bbq/examples/fragment-jquery-ui-tabs
 */
(function($) {

Drupal.quicktabsBbq = function($tabset, clickSelector, changeSelector) {

  changeSelector = changeSelector || clickSelector;

  // Define our own click handler for the tabs, overriding the default.
  $(clickSelector, $tabset).each(function(i, el){
    this.tabIndex = i;
    $(this).click(function(e){
      e.preventDefault();
      var state = {},
        id = $tabset.attr('id'), // qt container id
        idx = this.tabIndex; // tab index

      state[id] = idx;
      $.bbq.pushState(state);
    });
  });

  $(window).bind('hashchange', function(e) {
    $tabset.each(function() {
      var idx = $.bbq.getState(this.id, true);
      var $active_link = $(this).find(changeSelector).eq(idx);
      $active_link.triggerHandler('change');
    });
  });

  $(window).trigger('hashchange');
}

})(jQuery);