Drupal.behaviors.fbss_comments = function (context) {
  var ctxt = $(context);
  if ($.fn.autogrow) {
    // jQuery Autogrow plugin integration.
    ctxt.find('.fbss-comments-textarea').autogrow({expandTolerance: 2});
    ctxt.find('.fbss-comments-textarea').css('resize', 'none');
  }
  // Mark the comments wrapper with no-comments class if no comments exist for this entry
  ctxt.find('.facebook-status-comments').each(function (index, item) {
    var $this = $(this);
    var $comments = $('.fbss-comments', $this);
    if ($comments.length === 0) {
      $this.addClass('no-comments');
    }
  });
  // The "Comment" link when there are no comments. Reveals the textarea and save button.
  ctxt.find('.fbss-comments-show-comment-form').one('click', function() {
    $(this).hide();
    var f = $('#'+ this.id +' + div');
    f.fadeIn(300);
    var sid = this.id.split('-').pop();
    f.find('.fbss-comments-replace-'+ sid +'-inner').fadeIn(300);
    f.find('.fbss-comments-textarea').focus();
    return false;
  });
  // The "Comment" link when there are comments. Reveals the textarea and save button.
  ctxt.find('.fbss-comments-show-comment-form-inner').one('click', function() {
    $(this).hide();
    var sid = this.id.split('-').pop();
    $(this).parents('form').find('.fbss-comments-replace-'+ sid +'-inner').fadeIn(300);
    $(this).parents('form').find('.fbss-comments-textarea').focus();
    return false;
  });
  // The "Show all X comments" link when there are fewer than 10 comments. Reveals the hidden comments.
  ctxt.find('a.fbss-comments-show-comments').one('click', function() {
    $('#'+ this.id +' ~ div.fbss-comments-hide').fadeIn(300);
    $(this).remove();
    return false;
  });
  // Hide things we're not ready to show yet.
  ctxt.find('.fbss-comments-hide').hide();
  // Show things we're not ready to hide yet.
  ctxt.find('.fbss-comments-show-comment-form, .fbss-comments-show-comment-form-inner, .fbss-comments-show-comments').fadeIn(300);
  ctxt.find('.fbss-comments-show-comments').css('display', 'block');
  // Disable the save button at first.
  ctxt.find('.fbss-comments-submit').attr('disabled', true);
  // Disable the save button after saving a comment.
  ctxt.find('.fbss-comments-comment-form').bind('ahah_success', function() {
    $(this).find('.fbss-comments-submit').attr('disabled', true);
  });
  // Hide the save button until the comment textarea is clicked.
  ctxt.find('.fbss-comments-submit').hide();
  ctxt.find('.fbss-comments-textarea').focus(function() {
    $(this).parents('form').find('.fbss-comments-submit').show();
  });
  // Enable the save button if there is text in the textarea.
  ctxt.find('.fbss-comments-textarea').keypress(function(key) {
    var th = $(this);
    setTimeout(function() {
      if (th.val().length > 0) {
        th.parents('form').find('input').attr('disabled', false);
      }
      else {
        th.parents('form').find('input').attr('disabled', true);
      }
    }, 10);
  });
  // Modal Frame integration.
  if (Drupal.modalFrame) {
    ctxt.find('.fbss-comments-edit-delete a').click(function(event) {
      event.preventDefault();
      var sid = $(this).parents('form').attr('id').split('-').pop();
      var th = $(this), p = th.parents('.fbss-comments').parent();
      var handle = function() {
        $.get('index.php?q=fbss_comments/js/modalframe/'+ sid +'&source='+ window.location.href, function(data) {
          th.parents('.fbss-comments').replaceWith(data);
          Drupal.attachBehaviors(p.find('.fbss-comments'));
        });
      };
      Drupal.modalFrame.open({url: $(this).attr('href'), onSubmit: handle});
    });
  }
}
