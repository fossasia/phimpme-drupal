<?php

/**
 * @file
 *   Filters to statuses where a given user is a participant.
 */

/**
 * Filter handler to select statuses where a given user is a participant.
 */
class statuses_views_handler_filter_participant extends views_handler_filter {
  function value_form(&$form, &$form_state) {
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Participant user ID'),
      '#default_value' => isset($this->value) ? $this->value : '',
      '#required' => TRUE,
    );
  }
  function value_validate($form, &$form_state) {
    if (!is_numeric($form_state['values']['options']['value'])) {
      form_set_error('options][value', t('The specified user ID must be numeric.'));
    }
  }
  function query() {
    $this->query->add_where(
      $this->options['group'],
      "$this->table.sender = %d OR ($this->table.recipient = %d AND $this->table.type = 'user')",
      $this->value,
      $this->value
    );
  }
}
