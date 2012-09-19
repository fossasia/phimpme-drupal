/**
 * @file
 * Javascript for the interface at admin/content/media and also for interfaces
 * related to setting up media fields and for media type administration.
 *
 * Basically, if it's on the /admin path, it's probably here.
 */

(function ($) {

/**
 * Functionality for the thumbnail display
 */
Drupal.behaviors.mediaAdmin = {
  attach: function (context) {
    // Show a javascript confirmation dialog if a user has files selected and
    // they try to switch between the "Thumbnail" and "List" local tasks.
    $('.media-display-switch a').bind('click', function () {
      if ($(':checkbox:checked', $('form#media-admin')).length != 0) {
        return confirm(Drupal.t('If you switch views, you will lose your selection.'));
      }
    });

    // Configure the "Add file" link to fire the media browser popup.
    var $launcherLink = $('<a class="media-launcher" href="#"></a>').html(Drupal.t('Add file'));
    $launcherLink.bind('click', function () {
      // This option format needs *serious* work.
      // Not even bothering documenting it because it needs to be thrown.
      // See media.browser.js and media.browser.inc - media_browser()
      // For how it gets passed.
      var options = {
        disabledPlugins: ['library'],
        multiselect: true
      };
      Drupal.media.popups.mediaBrowser(function (mediaFiles) {
        // When the media browser succeeds, we refresh
        // @TODO: Should jump to the new media file and perhaps highlight it.
        parent.window.location.reload();
        return false;
      }, options);
    });

    $('ul.action-links', context).prepend($('<li></li>').append($launcherLink));

    if ($('.media-display-thumbnails').length) {
      // Implements 'select all/none' for thumbnail view.
      // @TODO: Support grabbing more than one page of thumbnails.
      var allLink = $('<a href="#">' + Drupal.t('all') + '</a>')
        .click(function () {
          $('.media-display-thumbnails', $(this).parents('form')).find(':checkbox').attr('checked', true).change();
          return false;
        });
      var noneLink = $('<a href="#">' + Drupal.t('none') + '</a>')
        .click(function () {
          $('.media-display-thumbnails', $(this).parents('form')).find(':checkbox').attr('checked', false).change();
          return false;
        });
      $('<div class="media-thumbnails-select" />')
        .append('<strong>' + Drupal.t('Select') + ':</strong> ')
        .append(allLink)
        .append(', ')
        .append(noneLink)
        .prependTo('#media-admin > div')
      // If the media item is clicked anywhere other than on the image itself
      // check the checkbox. For the record, JS thinks this is wonky.
      $('.media-item').bind('click', function (e) {
        if ($(e.target).is('img, a')) {
          return;
        }
        var checkbox = $(this).parent().find(':checkbox');
        if (checkbox.is(':checked')) {
          checkbox.attr('checked', false).change();
        } else {
          checkbox.attr('checked', true).change();
        }
      });

      // Add an extra class to selected thumbnails.
      $('.media-display-thumbnails :checkbox').each(function () {
        var checkbox = $(this);
        if (checkbox.is(':checked')) {
          $(checkbox.parents('li').find('.media-item')).addClass('selected');
        }

        checkbox.bind('change.media', function () {
          if (checkbox.is(':checked')) {
            $(checkbox.parents('li').find('.media-item')).addClass('selected');
          }
          else {
            $(checkbox.parents('li').find('.media-item')).removeClass('selected');
          }
        });
      });
    }

    // When any checkboxes are clicked on this form check to see if any are checked.
    // If any checkboxes are checked, show the edit options (@todo rename to edit-actions).
    $('#media-admin :checkbox').bind('change', function () {
      Drupal.behaviors.mediaAdmin.showOrHideEditOptions();
    });

    Drupal.behaviors.mediaAdmin.showOrHideEditOptions();
  },

  // Checks if any checkboxes on the form are checked, if so it will show the
  // edit-options panel.
  showOrHideEditOptions: function() {
    var fieldset = $('#edit-options');
    if (!$('#media-admin input[type=checkbox]:checked').size()) {
      fieldset.slideUp('fast');
    }
    else {
      fieldset.slideDown('fast');
    }
  }
};


/**
 * JavaScript for the Media types administrative form.
 */
Drupal.behaviors.mediaTypesAdmin = {
  attach: function (context) {
    if ($('.form-item-match-type', context).length == 0) {
      return;
    }
    // Toggle the 'other' text field on Match type.
    if ($('.form-item-match-type input:checked').val() != 'other') {
      $('.form-item-match-type-other').hide();
    }
    $('.form-item-match-type input').change(function () {
      if ($(this).val() == 'other') {
        $('.form-item-match-type-other').slideDown('fast');
      }
      else {
        $('.form-item-match-type-other').slideUp('fast');
      }
    });
  }
};



})(jQuery);
