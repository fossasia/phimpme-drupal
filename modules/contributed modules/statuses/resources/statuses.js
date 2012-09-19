var fbss_allowClickRefresh = true;
var fbss_refreshIDs;

(function($) {
Drupal.behaviors.statuses = {
attach: function (context) {
  var initialLoad = false;
  // Drupal passes document as context on page load.
  if (context == document) {
    initialLoad = true;
  }
  // Make sure we can run context.find().
  var ctxt = $(context);
  statuses_submit_disabled = false;
  var $statuses_field = ctxt.find('.statuses-text:first');
  var statuses_original_value = $statuses_field.val();
  var fbss_maxlen = Drupal.settings.statuses.maxlength;
  var fbss_hidelen = parseInt(Drupal.settings.statuses.hideLength);
  if (fbss_refreshIDs == undefined) {
    fbss_refreshIDs = Drupal.settings.statuses.refreshIDs;
  }
  if ($.fn.autogrow && $statuses_field) {
    // jQuery Autogrow plugin integration.
    $statuses_field.autogrow({expandTolerance: 2});
    $statuses_field.css('resize', 'none');
  }
  if (Drupal.settings.statuses.autofocus) {
    $statuses_field.focus();
  }
  if (Drupal.settings.statuses.noautoclear || Drupal.settings.statuses.autofocus) {
    if ($statuses_field.val() && $statuses_field.val().length != 0 && fbss_maxlen != 0) {
      fbss_print_remaining(fbss_maxlen - statuses_original_value.length, $statuses_field.parents('.statuses-update').find('.statuses-chars'));
    }
  }
  else {
    $.each(ctxt.find('.statuses-text-main'), function(i, val) {
      $(this).addClass('statuses-faded');
    });
    // Clear the status field the first time it's in focus if it hasn't been changed.
    ctxt.find('.statuses-text-main').one('focus', function() {
      var th = $(this);
      if (th.val() == statuses_original_value) {
        th.val('');
        if (fbss_maxlen != 0) {
          fbss_print_remaining(fbss_maxlen, th.parents('.statuses-update').find('.statuses-chars'));
        }
      }
      th.removeClass('statuses-faded');
    });
  }
  // Truncate long status messages.
  function fbss_truncate(i, val) {
    var th = $(val);
    var oldMsgText = th.html();
    var oldMsgLen = oldMsgText.length;
    if (oldMsgLen > fbss_hidelen) {
      var newMsgText =
        oldMsgText.substring(0, fbss_hidelen - 1) +
        '<span class="statuses-hellip">&hellip;&nbsp;</span><a class="statuses-readmore-toggle active">' +
        Drupal.t('Read more') +
        '</a><span class="statuses-readmore">' +
        oldMsgText.substring(fbss_hidelen - 1) +
        '</span>';
      th.html(newMsgText);
      th.find('.statuses-readmore').hide();
      th.find('.statuses-readmore-toggle').click(function(e) {
        e.preventDefault();
        th.html(oldMsgText);
      });
    }
  }
  if (fbss_hidelen > 0) {
    ctxt.find('.statuses-content').each(fbss_truncate);
  }
  // Modal Frame integration.
  if (Drupal.modalFrame) {
    ctxt.find('.statuses-edit a, .statuses-delete a').click(function(event) {
      event.preventDefault();
      Drupal.modalFrame.open({url: $(this).attr('href'), onSubmit: fbss_refresh});
    });
  }
  // React when a status is submitted.
  ctxt.find('#statuses-box').bind('ajax_complete', function(context) {
    if ($(context.target).html() != $(this).html()) {
      return;
    }
    fbss_refresh();
  });
  // On document load, add a refresh link where applicable.
  if (initialLoad && fbss_refreshIDs && Drupal.settings.statuses.refreshLink) {
    var loaded = {};
    $.each(fbss_refreshIDs, function(i, val) {
      if (val && val != undefined) {
        if ($.trim(val) && loaded[val] !== true) {
          loaded[val] = true;
          var element = $(val);
          element.wrap('<div></div>');
          var rlink = '<a href="'+ window.location.href +'">'+ Drupal.t('Refresh') +'</a>';
          element.parent().after('<div class="statuses-refresh-link">'+ rlink +'</div>');
        }
      }
    });
  }
  // Refresh views appropriately.
  ctxt.find('.statuses-refresh-link a').click(function() {
    if (fbss_allowClickRefresh) {
      fbss_allowClickRefresh = false;
      setTimeout('fbss_allowRefresh()', 2000);
      $(this).after('<div class="fbss-remove-me ahah-progress ahah-progress-throbber"><div class="throbber">&nbsp;</div></div>');
      fbss_refresh();
    }
    return false;
  });
  // Restore original status text if the field is blank and the slider is clicked.
  ctxt.find('.statuses-intro').click(function() {
    var th = $(this);
    var te = th.parents('.statuses-update').find('.statuses-text');
    if (te.val() == '') {
      te.val(statuses_original_value);
      if (fbss_maxlen != 0) {
        fbss_print_remaining(fbss_maxlen - statuses_original_value.length, th.parents('.statuses-update').find('.statuses-chars'));
      }
    }
  });
  if (fbss_maxlen != 0) {
    // Count remaining characters.
    ctxt.find('.statuses-text').bind('keydown keyup', function(fbss_key) {
      var th = $(this);
      var thCC = th.parents('.statuses-update').find('.statuses-chars');
      var fbss_remaining = fbss_maxlen - th.val().length;
      fbss_print_remaining(fbss_remaining, thCC);
    });
  }
}
}
// Change remaining character count.
function fbss_print_remaining(fbss_remaining, where) {
  if (fbss_remaining >= 0) {
    where.html(Drupal.formatPlural(fbss_remaining, '1 character left', '@count characters left', {}));
    if (statuses_submit_disabled) {
      $('.statuses-submit').attr('disabled', false);
      statuses_submit_disabled = false;
    }
  }
  else if (Drupal.settings.statuses.maxlength != 0) {
    where.html('<span class="statuses-negative">'+ Drupal.formatPlural(Math.abs(fbss_remaining), '-1 character left', '-@count characters left', {}) +'</span>');
    if (!statuses_submit_disabled) {
      $('.statuses-submit').attr('disabled', true);
      statuses_submit_disabled = true;
    }
  }
}
// Disallow refreshing too often or double-clicking the Refresh link.
function fbss_allowRefresh() {
  fbss_allowClickRefresh = !fbss_allowClickRefresh;
}
// Refresh parts of the page.
function fbss_refresh() {
  if (Drupal.heartbeat) {
    Drupal.heartbeat.pollMessages();
  }
  // Refresh elements by re-loading the current page and replacing the old version with the updated version.
  var loaded = {};
  if (fbss_refreshIDs && fbss_refreshIDs != undefined) {
    var loaded2 = {};
    $.each(fbss_refreshIDs, function(i, val) {
      if (val && val != undefined) {
        if ($.trim(val) && loaded2[val] !== true) {
          loaded2[val] = true;
          var element = $(val);
          element.before('<div class="fbss-remove-me ahah-progress ahah-progress-throbber" style="display: block; clear: both; float: none;"><div class="throbber">&nbsp;</div></div>');
        }
      }
    });
    var location = window.location.pathname +'?';
    // Build the relative URL with query parameters.
    var query = window.location.search.substring(1);
    if ($.trim(query) != "") {
      location += query +'&';
    }
    // IE will cache the result unless we add an identifier (in this case, the time).
    $.get(location +"ts="+ (new Date()).getTime(), function(data, textStatus) {
      // From load() in jQuery source. We already have the scripts we need.
      var new_data = data.replace(/<script(.|\s)*?\/script>/g, "");
      // Apparently Safari crashes with just $().
      var new_content = $('<div></div>').html(new_data);
      if (textStatus != 'error' && new_content) {
        // Replace relevant content in the viewport with the updated version.
        $.each(fbss_refreshIDs, function(i, val) {
          if (val && val != undefined) {
            if ($.trim(val) != '' && loaded[val] !== true) {
              var element = $(val);
              var insert = new_content.find(val);
              // If a refreshID is found multiple times on the same page, replace each one sequentially.
              if (insert.length && insert.length > 0 && element.length && element.length >= insert.length) {
                $.each(insert, function(j, v) {
                  v = $(v);
                  var el = $(element[j]);
                  // Don't bother replacing anything if the replacement region hasn't changed.
                  if (v.get() != el.get()) {
                    Drupal.detachBehaviors(element[j]);
                    el.replaceWith(v);
                    Drupal.attachBehaviors(v);
                  }
                });
              }
              loaded[val] = true;
            }
          }
        });
      }
      $('.fbss-remove-me').remove();
    });
  }
  else {
    $('.fbss-remove-me').remove();
  }
}
})(jQuery);
