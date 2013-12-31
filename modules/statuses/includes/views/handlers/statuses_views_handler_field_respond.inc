<?php

/**
 * @file
 *   Views handler for a "view conversation" link.
 */

/**
 * Formats the "view conversation" link.
 */
class statuses_views_handler_field_respond extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['recipient'] = 'recipient';
    $this->additional_fields['sid'] = 'sid';
    $this->additional_fields['type'] = 'type';
  }
  function render($values) {
    global $user;
    $recipient = user_load($values->{$this->aliases['recipient']});
    $sender = user_load($values->{$this->field_alias});
    $type = $values->{$this->aliases['type']};
    drupal_add_css(drupal_get_path('module', 'statuses') . '/resources/statuses.css');
    $options = array('attributes' => array('class' => array('statuses-respond')));
    if ($type == 'user' && $sender->uid != $recipient->uid && statuses_user_access('view_stream', $recipient) && statuses_user_access('view_stream', $sender)) {
      $title = t('View conversation');
      if ($recipient->uid == $user->uid) {
        $title = t('Reply');
      }
      return l($title, 'statuses/conversation/' . $sender->uid . ',' . $recipient->uid, $options);
    }
    elseif ($type != 'user' || ($sender->uid == $recipient->uid && $sender->uid != $user->uid)) {
      $s = '@' . $sender->name . ' ';
      // Properly reference tags with word-break characters in them.
      if (preg_match('/.+\b.+/', $sender->name)) {
        $s = '[@' . $sender->name . '] ';
      }
      if (variable_get('statuses_reply_type', 'at') == 'at') {
        // Evidently url() sanitizes query strings itself, so we don't have to use check_plain() here.
        $options['query'] = array(
          's' => $s,
          'rsid' => $values->{$this->aliases['sid']},
          'destination' => $_GET['q'],
        );
        return l(t('Respond'), 'statuses/share', $options);
      }
      return l(t('Respond'), 'statuses/conversation/' . $sender->uid . ',' . $user->uid, $options);
    }
  }
}
