<?php

/**
 * @file
 *   Allow only statuses from friends/followed + argument user.
 */

/**
 * Argument handler to select statuses from friends/followed + argument user.
 */
class statuses_views_handler_argument_flagged_user extends views_handler_argument {
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
      '#value' => t('Warning: this argument can be slow.'),
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
    $argument = $this->argument;
    $field = "$this->table.$this->real_field";
    $query = db_prefix_tables("$field IN (SELECT content_id FROM {flag_content} WHERE fid = %d AND uid = %d) OR $field = %d");
    $this->query->add_where(0, $query, $this->options['statuses_flag_type'], $argument, $argument);
  }
}
