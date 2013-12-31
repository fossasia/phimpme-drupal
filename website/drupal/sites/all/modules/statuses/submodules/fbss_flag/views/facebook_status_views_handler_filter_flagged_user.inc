<?php

/**
 * @file
 *   Filters to statuses posted by "followed" users plus the current user.
 */

/**
 * Filter handler to select statuses from friends/followed + current user.
 */
class statuses_views_handler_filter_flagged_user extends views_handler_filter {
  function option_definition() {
    $options = parent::option_definition();
    $flag = array_shift(flag_get_flags($content_type));
    $default = $flag ? $flag->fid : NULL;
    $options['statuses_flag_type'] = array(
      'default' => $default,
      'translatable' => FALSE,
    );
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $flags = flag_get_flags('user');
    $options = array();
    foreach ($flags as $flag) {
      $options[$flag->fid] = $flag->get_title();
    }
    $form['warning'] = array(
      '#value' => t('Warning: this filter can be slow.'),
      '#weight' => -100,
    );
    $form['statuses_flag_type'] = array(
      '#type' => 'radios',
      '#title' => t('Flag'),
      '#options' => $options,
      '#default_value' => $this->options['statuses_flag_type'],
      '#required' => TRUE,
    );
  }
  function query() {
    $query = "({$this->table}.sender IN (SELECT content_id FROM {flag_content} WHERE fid = %d AND uid = %d) OR {$this->table}.sender = %d)";
    $query = db_prefix_tables($query);
    $this->query->add_where($this->options['group'], $query, $this->options['statuses_flag_type'], $GLOBALS['user']->uid, $GLOBALS['user']->uid);
  }
}
