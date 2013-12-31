<?php

/**
 * @file
 *   Shows only users who have sent status messages to or received status messages from the current user.
 */

/**
 * Shows only users who have sent status messages to or received status messages from the current user.
 */
class statuses_views_handler_filter_communicated extends views_handler_filter {
  function query() {
    global $user;
    $this->ensure_my_table();
    $this->query->add_where(0, db_prefix_tables("
        ({users}.uid IN (SELECT sender FROM {statuses} WHERE type = 'user' AND recipient = %d) OR
        {users}.uid IN (SELECT recipient FROM {statuses} WHERE type = 'user' AND sender = %d))
        AND {users}.uid <> %d
    "), $user->uid, $user->uid, $user->uid);
  }
}
