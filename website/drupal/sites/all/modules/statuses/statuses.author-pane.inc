<?php

/**
 * @file
 *   This file provides a preprocess function for the author pane used by
 *   Advanced Forum and Advanced Profile Kit.
 */

/**
 * Implements hook_preprocess_author_pane().
 */
function statuses_preprocess_author_pane(&$variables) {
  $variables += array(
    'statuses' => '',
    'statuses_status' => '',
    'statuses_time' => '',
  );
  $status = _statuses_get_status_fast($variables['account']->uid);
  if (empty($status) || !statuses_user_access('view', $status)) {
    return;
  }
  // Show nothing if the status is blank.
  if (!empty($status->message)) {
    // The fully themed status, including the themed username (if settings permit) and the time.
    $variables['statuses'] = statuses_show($status);
    // Just the status, safe but with no filters run over it.
    $variables['statuses_status'] = check_plain($status->message);
    // The formatted time the status was submitted.
    $variables['statuses_time'] = theme('statuses_time', array('time' => $status->created));
  }
}
