<?php

/**
 * @file
 *   Filters to statuses matching the current context.
 */

/**
 * Filter handler to select statuses from matching the current context.
 */
class statuses_views_handler_filter_autotype extends views_handler_filter {
  function query() {
    $context = statuses_determine_context();
    $type = $context['handler']->type();
    $recipient = $context['handler']->find_recipient();
    $recipient_id = $context['handler']->recipient_id($recipient);
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], db_and()
      ->condition("{$this->table_alias}.recipient", $recipient_id)
      ->condition("{$this->table_alias}.type", $type)
    );
  }
}
