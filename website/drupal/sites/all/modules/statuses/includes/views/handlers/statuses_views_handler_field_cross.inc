<?php

/**
 * @file
 *   Handling for cross-posting.
 */

/**
 * Shows either the status owner (if the status was posted to one's own profile)
 * or the sender and recipient (if the status was posted elsewhere).
 */
class statuses_views_handler_field_cross extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['recipient'] = 'recipient';
    $this->additional_fields['type'] = 'type';
  }
  function render($values) {
    $type = $values->{$this->aliases['type']};
    $recipient_id = $values->{$this->aliases['recipient']};
    $sender_id = $values->{$this->field_alias};
    if ($sender_id == $recipient_id && $type == 'user') {
      return '<span class="statuses-sender">' . theme('username', array('account' => user_load($sender_id))) . '</span>';
    }
    else {
      $context = statuses_determine_context($type);
      $recipient = $context['handler']->load_recipient($recipient_id);
      $args = array(
        '!sender' => '<span class="statuses-sender">' . theme('username', array('account' => user_load($sender_id))) . '</span>',
        '!recipient' => '<span class="statuses-recipient">' . $context['handler']->recipient_link($recipient) . '</span>',
      );
      return t('!sender &raquo; !recipient', $args);
    }
  }
}
