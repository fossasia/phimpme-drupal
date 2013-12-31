<?php

/**
 * @file
 * An extended subclass for component filtering.
 */
class rules_scheduler_views_filter extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Component');
      $result = db_select('rules_scheduler', 'r')
        ->fields('r', array('config'))
        ->distinct()
        ->execute();
      $config_names = array();
      foreach ($result as $record) {
        $config_names[$record->config] = $record->config;
      }
      $this->value_options = $config_names;
    }
  }
}