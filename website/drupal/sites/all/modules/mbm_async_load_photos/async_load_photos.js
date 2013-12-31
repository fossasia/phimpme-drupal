/**
 * Script to get the photos ansynchrnously, then append to Galleriffic.
 * Credit: quannguyen@mbm.vn
 **/
(function ($) {

// We're about to manipulate the Galleriffic, so we turn effect off
// to avoid mismatch in time.
$.fx.off = true;

Drupal.async_load_photos = {
  currentpage: 0,
  exifdata: Array(),
  markerExisted: Array()
  };

Drupal.behaviors.async_load_photos = {
  attach: function(context, settings) {
    var st = settings.AsyncLoadPhotos;
    /* We don't trust st.page and st.total, because they are only available when
     * the View has pager */
    // Start to work when the Galleriffic has done transforming
    var gallery = $('#thumbs.galleriffic-processed', context);
    if (gallery) {

      /**
       * Callback for AJAX.
       **/
      var listGotten = function(returnedData, textStatus, XMLHttpRequest) {
        // Retrieve the next photos
        if (returnedData.length) {
          // Collect the exif data
          Drupal.async_load_photos.exifdata =
            $.merge(Drupal.async_load_photos.exifdata, $.map(returnedData, collectExif));
          // The first time of retrieving, we will replace the existing photos,
          // in order to make Exif showing available
          if (Drupal.async_load_photos.currentpage == 0) {
            removeFirstPhotos();
            Drupal.galleriffic.gotoImage(Drupal.galleriffic.data[0]);
          }
          // Append new photo
          $.each(returnedData, appendToGalleriffic);
          // Update the slide (big photo)
          if (Drupal.async_load_photos.currentpage == 0) {
            Drupal.galleriffic.gotoImage(Drupal.galleriffic.data[0]);
          }
          Drupal.async_load_photos.currentpage++;
          getListPhotos({
            page: Drupal.async_load_photos.currentpage,
            pagesize: st.pagesize,
            slide: st.style_slide,
            thumbnail: st.style_thumbnail});
        } else {
          // Stop update thumnail list. Turn effect on again
          $.fx.off = false;
        }
      };


      /**
       * Get the list of next photos via AJAX.
       **/
      var getListPhotos = function(pagedata) {
        $.ajax({
        url: st.ajax_url,
        type: 'GET',
        dataType: 'json',
        data: pagedata,
        error: errorCallback,
        success: listGotten
        });
      }


      // Get the list in background
      var i = parseInt(st.page);
      if (!isNaN(i)) Drupal.async_load_photos.currentpage = i;
      var pagedata = {
        page: Drupal.async_load_photos.currentpage,
        pagesize: st.pagesize,
        slide: st.style_slide,
        thumbnail: st.style_thumbnail
      }

      if ($('ul.thumbs').children().length == st.pagesize) {
        getListPhotos(pagedata);
      }

      // Change callback for SlideChanged
      Drupal.galleriffic.onSlideChange = function(prevIndex, nextIndex) {
        this.find('ul.thumbs').children()
          .eq(prevIndex).fadeTo('fast', 0.67).end()
          .eq(nextIndex).fadeTo('fast', 1.0);

        /* Locate the photo on map */
        var mapId = Object.keys(Drupal.settings.gmap)[0];
        if (!mapId) return;
        var map = Drupal.gmap.getMap(mapId).map;
        var exif = Drupal.async_load_photos.exifdata[nextIndex];
        var lo = glocFromExif(exif);
        if (!isNaN(lo.lat) && !isNaN(lo.lon)) {
          var p = new GLatLng(lo.lat, lo.lon);
          map.setCenter(p, 8);
          // If there exist marker for this photo, return
          if ($.inArray(nextIndex, Drupal.async_load_photos.markerExisted) > -1)
            return;
          // Marker not exist
          var baseIcon = new GIcon(G_DEFAULT_ICON);
          baseIcon.image = "../sites/all/modules/mbm_async_load_photos/mbm_marker.png";
          var pmaker = new GMarker(p, {icon: baseIcon});
          map.addOverlay(pmaker);
          Drupal.async_load_photos.markerExisted.push(nextIndex);
        }
        else
          map.setCenter();
      }
    }
  }
}

})(jQuery);

function errorCallback() {
  if (console) {
    console.warn('Error: Cannot get the list.');
  }
  jQuery.fx.off = false;
};

/**
 * Remove all existing photos in Galleriffic
 **/
function removeFirstPhotos() {
  while (Drupal.galleriffic.removeImageByIndex(Drupal.galleriffic.data.length-1)) {}
}

/**
 * Add photos to Galleriffic list
 **/
function appendToGalleriffic(index, Elm) {
  /* The Drupal.galleriffic's existence requires a modification in
   * Views Galleriffic's views_galleriffic.js */
  if (Drupal.galleriffic) {
    // Is about to update thumbnail list. Turn effect off
    // to reduce flicker.
    if (!jQuery.fx.off) jQuery.fx.off = true;
    Drupal.galleriffic.appendImage(buildlistItem(Elm));
    return true;
  }
  return false;
}


/**
* Build list item, to insert to Galleriffic
**/
function buildlistItem(Elm) {
  var newLi = jQuery('<li/>')
  var newAnc = jQuery('<a/>', {
    href: Elm.slide,
    title: Elm.filename,
    name: Elm.filename,
    'class': 'thumb'}).appendTo(newLi);
  var newImg = jQuery('<img/>', {
    src: Elm.thumbnail,
    alt: Elm.filename}).appendTo(newAnc);
  var captionDiv = jQuery('<div/>', {
    'class': 'caption'}).appendTo(newLi);
  captionDiv.append(jQuery('<div/>', {'class': 'image-title'})
                    .append(jQuery('<div/>', {text: Elm.title})));

  // Edit link
  if (Elm.edit_link) {
    var edDiv = jQuery('<div/>', {'class': 'image-edit'}).appendTo(captionDiv);
    jQuery('<a/>', {
      href: Elm.edit_link,
      text: 'Edit'}).wrap('<div/>').appendTo(edDiv);
  }

  // Galleriffic Description
  var deDiv = jQuery('<div/>', {'class': 'image-desc'}).appendTo(captionDiv);
  // Add tags
  if (Elm.tags.length) {
    var taDiv = jQuery('<div/>', {'class': 'phototag', text: 'Tags: '});
    taDiv.appendTo(deDiv);
  }
  jQuery.each(Elm.tags, function(idx, tg) {
    jQuery('<a/>', {
      href: tg.link,
      text: tg.name
    }).appendTo(taDiv);
    // Add comma
    if (idx < Elm.tags.length - 1) {
      taDiv.append(document.createTextNode(', '));
    }
  })
  // Add Photo description
  if (Elm.description) {
    var pdesDiv = jQuery('<div/>', {'class': 'photodesc'}).appendTo(deDiv);
    pdesDiv.append(Elm.description);
  }

  // Add Exif data
  var exifDiv = jQuery('<div/>', {'class': 'exif-data'}).appendTo(captionDiv);
  jQuery('<div/>').append(jQuery('<span/>', {'class': 'label', text: 'Camera:'}))
             .append(jQuery('<span/>', {text: Elm.exif_make}))
             .appendTo(exifDiv);
  jQuery('<div/>').append(jQuery('<span/>', {'class': 'label', text: 'Model:'}))
             .append(jQuery('<span/>', {text: Elm.exif_model}))
             .appendTo(exifDiv);
  return newLi;
}

// Implement Python-like string.startswith
if (typeof String.prototype.startsWith != 'function') {
  String.prototype.startsWith = function (str) {
    return (this.lastIndexOf(str, 0) == 0);
  };
}

// Collect Exif data from AJAX
function collectExif(Elm, idx) {
  var collect = {};
  jQuery.each(Elm, function(key, value) {
    if (key.startsWith('exif_') || key.startsWith('gps_')) {
      collect[key] = Elm[key];
    }
  });
  return collect;
}

/**
 * Gmap LatLon from Exif
 **/
function glocFromExif(exif) {
  var lon = parseFloat(exif.gps_gpslongitude);
  var lat = parseFloat(exif.gps_gpslatitude);
  var lonref = exif.gps_gpslongituderef;
  var latref = exif.gps_gpslatituderef;
  if (lonref == 'W') lon = -lon;
  if (latref == 'S') lat = -lat;
  return {lat: lat, lon: lon};
}
