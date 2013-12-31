<?php

/**
 * @file
 *   Filters to allow only the latest status in Views.
 */

/**
 * Filter handler which requires the latest status.
 */
class statuses_views_handler_filter_latest_only extends views_handler_filter_boolean_operator {
  function construct() {
    $this->definition['type'] = 'yes-no';
    $this->definition['label'] = t('Show only latest status update (per user)');
    $this->value_value = $this->definition['label'];
    parent::construct();
  }

  function query() {
    if ($this->value) {
      $this->ensure_my_table();
      $subquery = "(SELECT MAX(sid) FROM {statuses} WHERE sender = recipient AND type = 'user' GROUP BY recipient)";
      $this->query->add_where(0, db_prefix_tables("$this->table_alias.sid IN $subquery"));
    }
  }
}
