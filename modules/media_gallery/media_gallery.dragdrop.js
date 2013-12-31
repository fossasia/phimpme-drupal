Drupal.mediaGallerySort = {};
Drupal.behaviors.mediaGallerySort = {};

Drupal.behaviors.mediaGallerySort.attach = function (context, settings) {
  var $ = jQuery;
  // Create a drag-and-drop editor for gallery collections.
  var $collection = $('.media-gallery-collection', context).once('media-gallery-sortable');
  $('.node-media-gallery.node-teaser .float-overflow', $gallery).once('media-gallery-draggable', Drupal.mediaGallerySort.addDraggableIcon);
  if ($collection.length && settings.mediaGallerySortCollectionUrl) {
    $collection.sortable();
    //Drupal.mediaGallerySort.setHeight($collection);
    var callback = settings.mediaGallerySortCollectionUrl;
    $collection.bind('sortupdate', {'callback': callback}, Drupal.mediaGallerySort.handle_update);
    $collection.bind('sortstart', Drupal.mediaGallerySort.setHeight);
    $collection.bind('sortstop', Drupal.mediaGallerySort.setHeight);
  }
  // Create a drag-and-drop editor for individual gallery grid pages.
  var $gallery = $('body.page-node #block-system-main .node-media-gallery .media-gallery-media > .field-items').once('media-gallery-sortable');
  $('.media-gallery-item-wrapper', $gallery).once('media-gallery-draggable', Drupal.mediaGallerySort.addDraggableIcon);
  if ($gallery.length && settings.mediaGallerySortGalleryUrl) {
    $gallery.sortable();
    //Drupal.mediaGallerySort.setHeight($gallery);
    callback = settings.mediaGallerySortGalleryUrl;
    $gallery.bind('sortupdate', {'callback': callback, 'reorder': 'true'}, Drupal.mediaGallerySort.handle_update);
    $gallery.bind('sortstart', Drupal.mediaGallerySort.setHeight);
    $gallery.bind('sortstop', Drupal.mediaGallerySort.setHeight);
  }
};

/**
 * Set an equal explicit, rounded height for all the items in the gallery to
 * avoid float breaking.
 */
Drupal.mediaGallerySort.setHeight = function (event) {
  var $ = jQuery;
  var placeholder = $(this).children('.ui-sortable-placeholder');
  if (placeholder.length) {
    var height = placeholder.height();
    $(this).children().height(height);
  } else {
    $(this).children().attr('style', '');
  }
};

/**
 * Adds the draggable icon to to each image.
 */
Drupal.mediaGallerySort.addDraggableIcon = function () {
  var $ = jQuery;
  $(this).addClass('draggable');
  $(this).prepend($('<div class="draggable-wrapper"><a class="draggable-handle">Drag</a></div>'));
}

/**
 * Event handler for the 'sortupdate' event. Sends the new order to the server.
 *
 * TODO: Refactor inline closures using jQuery.proxy or, if that's not enough,
 * an equivalent to ThemeBuilder.bind().
 */
Drupal.mediaGallerySort.handle_update = function (event, ui) {
  var $ = jQuery;
  var sortable = $(this);
  var reorder = event.data.reorder;
  var post = {
    order: sortable.sortable('toArray'),
    page: $.deparam.querystring().page
  };

  /**
   * Change ID attributes of sorted items to reflect the new order.
   *
   * Individual media items in the media_gallery_media field don't have a primary
   * key, so we're sorting by their delta. When their delta changes on the server
   * side, we need to reflect it on the client side as well.
   */
  var success_callback = function (data) {
    if (reorder) {
      var $ = jQuery;
      var i, newId, $item;
      var $toReorder = $('#block-system-main [id^=' + data.idPrefix + ']').addClass('media-gallery-to-reorder');
      if ($toReorder.length !== data.order.length) {
        // We seem to have the wrong set of objects to reorder, so just
        // refresh the page; the server has the right order already.
        window.location.reload();
        return;
      }
      var newOrder = Drupal.mediaGallerySort._getNewOrder(data.order, data.idPrefix);
      // Replacing IDs is tricky; we don't want to end up with two elements
      // having the same ID. For a first pass, replace all elements in order
      // with their new ID plus a placeholder.
      for (i = 0; i < newOrder.length; i++) {
        newId = data.idPrefix + newOrder[i] + '---new';
        $toReorder.first().attr('id', newId);
        $toReorder = $toReorder.not('#' + newId);
      }
      // Now remove the placeholder from each item's ID.
      $toReorder = $('.media-gallery-to-reorder');
      for (i = 0; i < newOrder.length; i++) {
        $item = $($toReorder[i]);
        newId = $item.attr('id').replace(/---new/, '');
        $item.removeClass('media-gallery-to-reorder');
        $item.attr('id', newId);
      }
    }
    sortable.sortable('enable');
  }

  $.post(event.data.callback, post, success_callback);
  sortable.sortable('disable');
};

/**
 * Helper function that returns a sorted array of element IDs minus a prefix.
 *
 * @param {Array} ids
 *   The IDs to be reordered. Example: ['element-5', 'element-4', 'element-6']
 * @param {String} prefix
 *   The prefix to be removed from each ID. Example: 'element-'
 *
 * @return {Array}
 *   The IDs, minus their prefix, sorted numerically. Example: [4, 5, 6]
 */
Drupal.mediaGallerySort._getNewOrder = function (ids, prefix) {
  var i;
  var newOrder = [];
  for (i = 0; i < ids.length; i++) {
    var re = new RegExp(prefix);
    newOrder.push(parseInt(ids[i].replace(re, ''), 10));
  }
  newOrder.sort(Drupal.mediaGallerySort.numericAscending);
  return newOrder;
}

/**
 * Sort callback. Returns the lesser of two numeric inputs.
 */
Drupal.mediaGallerySort.numericAscending = function (a, b) {
  return a - b;
}
