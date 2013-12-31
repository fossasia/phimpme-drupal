<?php

/**
 * @file
 *   Shows the User ID of the current user.
 */

/**
 * Field handler to show the User ID of the current user.
 */
class statuses_views_handler_field_current_uid extends views_handler_field {
  function render($values) {
    return $GLOBALS['user']->uid;
  }
}
