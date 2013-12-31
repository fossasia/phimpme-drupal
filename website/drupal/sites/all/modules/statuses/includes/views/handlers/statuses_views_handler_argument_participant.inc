<?php

/**
 * @file
 *   Allow only statuses where a given user is a participant.
 */

/**
 * Argument handler to select statuses where a given user is a participant.
 */
class statuses_views_handler_argument_participant extends views_handler_argument {
  function query() {
    $argument = $this->argument;
    $this->query->add_where(0, "($this->table.sender = %d OR $this->table.recipient = %d) AND $this->table.type = 'user'", $argument, $argument);
  }
}
