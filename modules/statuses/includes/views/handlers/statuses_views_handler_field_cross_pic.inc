<?php

/**
 * @file
 *   Handling for cross-posting.
 */

/**
 * Shows either the status owner (if the status was posted to one's own profile)
 * or the status owner and status poster (if the status was posted elsewhere).
 * Also shows the user picture.
 */
class statuses_views_handler_field_cross_pic extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    if (module_exists('imagecache_profiles')) {
      $options['imagecache_preset'] = array(
        'default' => variable_get('user_picture_imagecache_profiles_default', ''),
        'translatable' => FALSE,
      );
    }
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    if (module_exists('imagecache_profiles')) {
      $options = $this->options;
      $form['imagecache_preset'] = array(
        '#title' => t('Image style'),
        '#type' => 'select',
        '#options' => image_style_options(TRUE),
        '#default_value' => $options['imagecache_preset'],
      );
    }
  }
  function construct() {
    parent::construct();
    $this->additional_fields['recipient'] = 'recipient';
    $this->additional_fields['type'] = 'type';
  }
  function render($values) {
    $recipient_id = $values->{$this->aliases['recipient']};
    $sender_id = $values->{$this->field_alias};
    $sender = user_load($sender_id);
    $type = $values->{$this->aliases['type']};
    $options = $this->options;
    $preset = NULL;
    if (isset($options['imagecache_preset']) && $options['imagecache_preset']) {
      $preset = $options['imagecache_preset'];
    }
    if ($sender_id == $recipient_id && $type == 'user') {
      return t('!picture !user', array(
        '!picture' => '<span class="statuses-sender-picture">' . statuses_display_user_picture($sender, $preset) . '</span>',
        '!user' => '<span class="statuses-sender">' . theme('username', array('account' => $sender)) . '</span>',
      ));
    }
    elseif ($type == 'user') {
      $recipient = user_load($recipient_id);
      $args = array(
        '!sender' => '<span class="statuses-sender">' . theme('username', array('account' => $sender)) . '</span>',
        '!recipient' => '<span class="statuses-recipient">' . theme('username', array('account' => $recipient)) . '</span>',
        '!sender-picture' => '<span class="statuses-sender-picture">' . statuses_display_user_picture($sender, $preset) . '</span>',
        '!recipient-picture' => '<span class="statuses-recipient-picture">' . statuses_display_user_picture($recipient, $preset) . '</span>',
      );
      return t('!sender-picture !sender &raquo; !recipient-picture !recipient', $args);
    }
    else {
      $context = statuses_determine_context($type);
      $recipient = $context['handler']->load_recipient($recipient_id);
      $args = array(
        '!sender' => '<span class="statuses-sender">' . theme('username', array('account' => $sender)) . '</span>',
        '!recipient' => '<span class="statuses-recipient">' . $context['handler']->recipient_link($recipient) . '</span>',
        '!sender-picture' => '<span class="statuses-sender-picture">' . statuses_display_user_picture($sender, $preset) . '</span>',
      );
      return t('!sender-picture !sender &raquo; !recipient', $args);
    }
  }
}
