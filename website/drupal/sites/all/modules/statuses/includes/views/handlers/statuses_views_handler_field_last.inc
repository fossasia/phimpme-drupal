<?php

/**
 * @file
 *   Shows the last status each user sent to the current user, if any.
 */

/**
 * Field handler to show the last status sent to the current user.
 */
class statuses_views_handler_field_last extends views_handler_field {
  function render($values) {
    global $user;
    $uid = $values->{$this->field_alias};
    $message = db_query_range("
      SELECT message
      FROM {statuses}
      WHERE
        type = 'user' AND
        message <> '' AND (
          (sender = :sender AND recipient = :recipient) OR
          (sender = :recipient AND recipient = :sender)
        )
      ORDER BY sid DESC
    ", 0, 1, array(':sender' => $user->uid, ':recipient' => $uid))->fetchField();
    $message = _statuses_run_filter($message);
    if (variable_get('statuses_nl2br', 0)) {
      $message = nl2br($message);
    }
    return '<span class="statuses-content">' . $message . '</span>';
  }
}
