<?php

/**
 * @file
 *   Formats statuses in Views.
 */

/**
 * Field handler to provide the most recent status update.
 */
class statuses_views_handler_field_message extends views_handler_field {
  function render($values) {
    $message = _statuses_run_filter($values->{$this->field_alias});
    if (variable_get('statuses_nl2br', 0)) {
      $message = nl2br($message);
    }
    return '<span class="statuses-content">' . $message . '</span>';
  }
}
