<?php

/**
 * @file
 *   Simplifies the Tag Type filter in Views.
 */

/**
 * Changes the form for the Tag Type filter options.
 */
class statuses_views_handler_filter_string_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Tag type');
      $options = array('user' => t('Users'));
      if (module_exists('taxonomy')) {
        $options['term'] = t('Terms');
      }
      $this->value_options = $options;
    }
  }
}
