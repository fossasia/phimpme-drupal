
var images_to_show = []; // Store the path of images
var gallery;

function mycarousel_itemLoadCallback(carousel, state)
{
    // Check if the requested items already exist
    if (carousel.has(carousel.first, carousel.last)) {
        return;
    }

    var nid = jQuery('#node_id')[0].value
    jQuery.get(
        '?q=list/' + nid,
        {
            first: carousel.first,
            last: carousel.last
        },
        function(xml) {
            mycarousel_itemAddCallback(carousel, carousel.first, carousel.last, xml);
        },
        'xml'
    );
};

function get_images_incat()
{
    var nid = jQuery('#node_id')[0].value
    jQuery.get(
        '?q=list/' + nid,
        {
            first: 1,
            last: 1
        },
        function(xml) {
            jQuery('image', xml).each(function(i) {
                url = jQuery(this).text();
                images_to_show.push(url);
            });
            // For Galleriffic
            populate_thumbs_list();
        },
        'xml'
    );
}

function mycarousel_itemAddCallback(carousel, first, last, xml)
{
    // Set the size of the carousel
    carousel.size(parseInt(jQuery('total', xml).text()));

    jQuery('image', xml).each(function(i) {
        url = jQuery(this).text();
        carousel.add(first + i, mycarousel_getItemHTML(url));
        images_to_show.push(url);
    });

    // For Galleriffic
    populate_thumbs_list();
};

/**
 * Item html creation helper.
 */
function mycarousel_getItemHTML(url)
{
    return '<img src=\"' + url + '\" width=\"\" height=\"\" alt=\"\" />';
};

function populate_thumbs_list()
{
    images_to_show.forEach(append_thumbnail);
    images_to_show = [];
}

function append_thumbnail(path, index, array)
{
    var ulist = jQuery('#thumbs > ul')[0];
    var last_item = jQuery(ulist.lastElementChild); // A <li> element
    var new_item = last_item.clone();

    // Remove the first thumbnail and recreate
    if (last_item.next().length == 0 && index == 0)
    {
        gallery.removeImageByIndex(0);
    }
    // From the second one, we create and append new element
    new_item.find('a')[0].href = path;
    var img = new_item.find('img')[0];
    img.src = get_thumbnail_url(path);
    // new_item.appendTo(ulist);
    gallery.appendImage(new_item);
}

function get_thumbnail_url(origin_url)
{
    var a = origin_url.split('/');
    return '?q=sites/default/files/styles/square_thumbnail/public/' + a[a.length - 1];
}

/*
 * Initialize Galleriffic
 */
function init_galleriffic()
{
    gallery = jQuery('#thumbs').galleriffic({
        delay:                     3000, // in milliseconds
        numThumbs:                 10, // The number of thumbnails to show page
        preloadAhead:              10, // Set to -1 to preload all images
        enableTopPager:            false,
        enableBottomPager:         true,
        maxPagesToShow:            7,  // The maximum number of pages to display in either the top or bottom pager
        imageContainerSel:         '#slideshow', // The CSS selector for the element within which the main slideshow image should be rendered
        controlsContainerSel:      '#controls', // The CSS selector for the element within which the slideshow controls should be rendered
        captionContainerSel:       '#caption', // The CSS selector for the element within which the captions should be rendered
        loadingContainerSel:       '#loading', // The CSS selector for the element within which should be shown when an image is loading
        renderSSControls:          true, // Specifies whether the slideshow's Play and Pause links should be rendered
        renderNavControls:         true, // Specifies whether the slideshow's Next and Previous links should be rendered
        playLinkText:              'Play',
        pauseLinkText:             'Pause',
        prevLinkText:              'Previous',
        nextLinkText:              'Next',
        nextPageLinkText:          'Next &rsaquo;',
        prevPageLinkText:          '&lsaquo; Prev',
        enableHistory:             false, // Specifies whether the url's hash and the browser's history cache should update when the current slideshow image changes
        enableKeyboardNavigation:  true, // Specifies whether keyboard navigation is enabled
        autoStart:                 false, // Specifies whether the slideshow should be playing or paused when the page first loads
        syncTransitions:           false, // Specifies whether the out and in transitions occur simultaneously or distinctly
        defaultTransitionDuration: 1000, // If using the default transitions, specifies the duration of the transitions
        onSlideChange:             undefined, // accepts a delegate like such: function(prevIndex, nextIndex) { ... }
        onTransitionOut:           undefined, // accepts a delegate like such: function(slide, caption, isSync, callback) { ... }
        onTransitionIn:            undefined, // accepts a delegate like such: function(slide, caption, isSync) { ... }
        onPageTransitionOut:       undefined, // accepts a delegate like such: function(callback) { ... }
        onPageTransitionIn:        undefined, // accepts a delegate like such: function() { ... }
        onImageAdded:              undefined, // accepts a delegate like such: function(imageData, $li) { ... }
        onImageRemoved:            undefined  // accepts a delegate like such: function(imageData, $li) { ... }
    });
}

(function($) {
    $(document).ready(function() {
        $('#mycarousel').jcarousel({
            // Uncomment the following option if you want items
            // which are outside the visible range to be removed
            // from the DOM.
            // Useful for carousels with MANY items.

            // itemVisibleOutCallback: {onAfterAnimation: function(carousel, item, i, state, evt) { carousel.remove(i); }},
            itemLoadCallback: mycarousel_itemLoadCallback,
            scroll:1,
            visible:1
        });

        $('div.navigation').css({'width': '300px', 'float': 'left'});
        $('#largeshow').css('display', 'block');

        init_galleriffic();
    });
})(jQuery);
