<?php

/**
 * @file
 *   Formats the status comment update form in Views.
 */

/**
 * Field handler to add the status comment update form.
 */
class fbss_comments_views_handler_field_comment_box extends views_handler_field {
  function render($values) {
    return '<div class="statuses-comments">' .
      theme('fbss_comments_form_display', array(
        'sid' => $values->{$this->field_alias},
        'delay_load_form' => TRUE,
        'delay_load_comments' => TRUE
      )) . '</div>';
  }
}
