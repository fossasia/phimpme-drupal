(function ($) {

Drupal.behaviors.ViewsGalleriffic = { 
  attach: function(context) {
    var settings = Drupal.settings.views_galleriffic;
  

    // Initialize Advanced Galleriffic Gallery
    var gallery = $('#thumbs:not(.galleriffic-processed)',context).addClass("galleriffic-processed").galleriffic({ 
      delay:                  settings.delay,
      numThumbs:              settings.numbthumbs,
      preloadAhead:           settings.numbthumbs,
      enableTopPager:         settings.enableTopPager,
      enableBottomPager:      settings.enableBottomPager,
      imageContainerSel:      '#slideshow',
      controlsContainerSel:   '#controls',
      captionContainerSel:    '#caption',
      loadingContainerSel:    '#loading',
      renderSSControls:       settings.renderSSControls,
      renderNavControls:      settings.renderNavControls,
      playLinkText:           settings.playLinkText,
      pauseLinkText:          settings.pauseLinkText,
      prevLinkText:           settings.prevLinkText,
      nextLinkText:           settings.nextLinkText,
      nextPageLinkText:       settings.nextPageLinkText,
      prevPageLinkText:       settings.prevPageLinkText,
      enableHistory:          settings.enableHistory,
      preloadAhead:           settings.preloadAhead,
      autoStart:              settings.autoStart,
      syncTransitions:        settings.syncTransitions,
      defaultTransitionDuration: settings.transition,
      enableKeyboardNavigation: settings.enableKeyboardNavigation,
      onSlideChange:          function(prevIndex, nextIndex) {
        // 'this' refers to the gallery, which is an extension of $('#thumbs')
        this.find('ul.thumbs').children()
          .eq(prevIndex).fadeTo('fast', onMouseOutOpacity).end()
          .eq(nextIndex).fadeTo('fast', 1.0);
      },
      onPageTransitionOut:       function(callback) {
        this.fadeTo('fast', 0.0, callback);
      },
      onPageTransitionIn:        function() {
        var prevPageLink = this.find('a.prev').css('visibility', 'hidden');
        var nextPageLink = this.find('a.next').css('visibility', 'hidden');

        // Show appropriate next / prev page links
        if (this.displayedPage > 0)
          prevPageLink.css('visibility', 'visible');
              
        var lastPage = this.getNumPages() - 1;
        if (this.displayedPage < lastPage)
          nextPageLink.css('visibility', 'visible');

        this.fadeTo('fast', 1.0);
      } 
    });

    // Initially set opacity on thumbs and add
    // additional styling for hover effect on thumbs
    var onMouseOutOpacity = 0.67;
      $('#thumbs.galleriffic-processed ul.thumbs li').opacityrollover({
          mouseOutOpacity:   onMouseOutOpacity,
          mouseOverOpacity:  1.0,
          fadeSpeed:         'fast',
          exemptionSelector: '.selected'
    });
    /**************** Event handlers for custom next / prev page links **********************/
    gallery.find('a.prev').click(function(e) {
      gallery.previousPage();
      e.preventDefault();
    });

    gallery.find('a.next').click(function(e) {
      gallery.nextPage();
      e.preventDefault();
    });
    
    if (settings.enableHistory == 'true') { 
      /**** Functions to support integration of galleriffic with the jquery.history plugin ****/

      function pageload(hash) {
        // alert("pageload: " + hash);
        // hash doesn't contain the first # character.
        if(hash) {
          $.galleriffic.gotoImage(hash);
        } else {
          gallery.gotoIndex(0);
        }
      }

      // Initialize history plugin.
      // The callback is called at once by present location.hash. 
      $.historyInit(pageload, window.location.pathname);

      // set onlick event for buttons using the jQuery 1.3 live method
      $("a[rel='history']").live('click', function(e) {
        if (e.button != 0) return true;

        var hash = this.href;
        hash = hash.replace(/^.*#/, '');

        // moves to a new page. 
        // pageload is called at once. 
        // hash don't contain "#", "?"
        $.historyLoad(hash);

        return false;
      });
    }

  }
}
})(jQuery);
