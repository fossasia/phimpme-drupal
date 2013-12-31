<?php

/**
 * @file
 *   Filter to allow only statuses posted on one's own profile.
 */

/**
 * Allows only statuses posted on one's own profile.
 */
class statuses_views_handler_filter_own extends views_handler_filter_boolean_operator {
  function construct() {
    $this->definition['type'] = 'yes-no';
    $this->definition['label'] = t('Show only own statuses');
    $this->value_value = $this->definition['label'];
    parent::construct();
  }

  function query() {
    if ($this->value) {
      $this->ensure_my_table();
      $this->query->add_where(0, db_prefix_tables("($this->table_alias.type = 'user' AND $this->table_alias.sender = $this->table_alias.recipient)"));
    }
  }
}
