<?php

/**
 * @file
 *   Views handler for the HTML classes for a status.
 */

/**
 * Handles the HTML classes for a status.
 */
class statuses_views_handler_field_classes extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['recipient'] = 'recipient';
    $this->additional_fields['type'] = 'type';
    if (module_exists('fbss_privacy')) {
      $this->additional_fields['private'] = 'private';
    }
  }
  function render($values) {
    $recipient_id = $values->{$this->aliases['recipient']};
    $sender_uid = $values->{$this->field_alias};
    $type = $values->{$this->aliases['type']};
    $self = $recipient_id == $sender_uid && $type == 'user';
    $private = isset($this->aliases['private']) ? $values->{$this->aliases['private']} : 0;
    return 'facebook-status-item facebook-status-media facebook-status-type-'. $type . ($self ? ' facebook-status-self-update' : '') . ($private ? ' facebook-status-private' : '');
  }
}
