<?php

/**
 * @file
 *   Handles the conversation view.
 */

/**
 * Generates the title for the conversation page.
 */
function _statuses_conversation_title() {
  global $user;
  $args = explode(',', arg(2));
  $count = count($args);
  if (is_numeric($args[0]) && $count == 2 || ($count == 1 && $args[0] != $user->uid)) {
    if (empty($args[1])) {
      $recipient = user_load($args[0]);
      if (!empty($recipient->uid)) {
        return t('Conversation between you and @recipient', array('@recipient' => $recipient->name));
      }
    }
    elseif (is_numeric($args[1])) {
      $account1 = user_load($args[0]);
      $account2 = user_load($args[1]);
      if (!empty($account1->uid) && !empty($account2->uid)) {
        return t('Conversation between @sender and @recipient', array('@sender' => $account1->name, '@recipient' => $account2->name));
      }
    }
  }
  return t('Conversation');
}

/**
 * Builds a page with a conversation view. A form to post a message to the other
 * user's profile may be at the top.
 */
function _statuses_conversation() {
  drupal_set_title(_statuses_conversation_title());
  global $user;
  $args = explode(',', arg(2));
  $count = count($args);
  if ($count > 2 || $count < 1 || ($count == 1 && $args[0] == $user->uid) || !is_numeric($args[0]) || ($count == 2 && !is_numeric($args[1]))) {
    return MENU_NOT_FOUND;
  }
  $key = array_search($user->uid, $args);
  if ($key === FALSE) {
    if ($count === 1) {
      $args[1] = $user->uid;
    }
    $arg = implode(',', $args);
    return views_embed_view('facebook_status_conversation', 'default', $args[0], $args[1]);
  }
  $key = $key ? 0 : 1; //The recipient is not the current user.
  $recipient = user_load($args[$key]);
  if (statuses_user_access('converse') && !empty($recipient->uid)) {
    return theme('statuses_form_display', array('recipient' => $recipient, 'type' => 'user'));
  }
  return MENU_NOT_FOUND;
}
