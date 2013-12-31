<?php

/**
 * @file
 *   Converts tags in status message to links.
 */

/**
 * Field handler to display message with tags converted to links.
 */
class statuses_tags_views_handler_field_message extends views_handler_field {
  function render($values) {
    $message = _statuses_run_filter($values->{$this->field_alias});
    if (variable_get('statuses_nl2br', 0)) {
      $message = nl2br($message);
    }
    return '<span class="statuses-content">' . statuses_tags_process_message($message) . '</span>';
  }
}
