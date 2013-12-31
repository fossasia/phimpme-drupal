<?php

/**
 * @file
 *   Handling for the recipient's name.
 */

/**
 * Allows linking to the recipient of the status.
 */
class statuses_views_handler_field_recipient extends views_handler_field_user {
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to'] = array(
      'default' => TRUE,
      'translatable' => FALSE,
    );
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to'] = array(
      '#type' => 'checkbox',
      '#title' => t('Link to recipient'),
      '#default_value' => $this->options['link_to'],
    );
  }
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }
  function render($values) {
    $type = $values->{$this->aliases['type']};
    $context = statuses_determine_context($type);
    $recipient = $context['handler']->load_recipient($recipient_id);
    if (!empty($this->options['link_to'])) {
      return '<span class="statuses-recipient">' . $context['handler']->recipient_link($recipient) . '</span>';
    }
    return '<span class="statuses-recipient">' . check_plain($context['handler']->recipient_name($recipient)) . '</span>';
  }
}
