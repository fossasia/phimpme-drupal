<?php

/**
 * @file
 *   Provides access functions for the Statuses API.
 */

/**
 * Checks permission to create a new status message.
 * Note that this is not called when sending a message to all users at once.
 *
 * @param $recipient
 *   An object representing the recipient of the status message. Defaults to
 *   the current user.
 * @param $type
 *   The stream type. Defaults to "user."
 * @param $sender
 *   An object representing the sender of the status message. Defaults to the
 *   current user.
 * @return
 *   TRUE if $sender has permission to post a status message to $recipient;
 *   FALSE otherwise.
 */
function statuses_user_access_add($recipient = NULL, $type = 'user', $sender = NULL) {
  global $user;
  $recipient = (empty($recipient) ? $user : $recipient);
  $sender = (empty($sender) ? $user : $sender);
  $context = statuses_determine_context($type);
  return $context['handler']->access_add($recipient, $sender);
}

/**
 * Checks permission to view a conversation.
 * Note that this is only called on conversation pages.
 *
 * @return
 *   TRUE if the current user has permission to view the current conversation;
 *   FALSE otherwise.
 */
function statuses_user_access_converse() {
  global $user;
  $uids = explode(',', arg(2), 2);
  if (empty($uids)) {
    return FALSE;
  }
  elseif (count($uids) === 1 && $uids[0] != $user->uid) {
    $uids[1] = $user->uid;
  }
  elseif (count($uids) === 1) {
    return FALSE; //Can't have a conversation with yourself
  }
  elseif ($uids[0] == $uids[1]) {
    return FALSE; //Can't have a conversation with yourself
  }
  return user_access('view all statuses');
}

/**
 * Checks permission to delete a status.
 *
 * @param $status
 *   The status being considered for deletion.
 * @param $account
 *   The user account object of the user taking the action.
 * @return
 *   TRUE if the given user has permission to delete the given status; FALSE
 *   otherwise.
 */
function statuses_user_access_delete($status, $account = NULL) {
  $account = (empty($account) ? $GLOBALS['user'] : $account);
  $context = statuses_determine_context($status->type);
  return $context['handler']->access_delete($status, $account);
}

/**
 * Checks permission to edit a status.
 *
 * @param $status
 *   The status being considered for editing.
 * @param $account
 *   The user account object of the user taking the action.
 * @return
 *   TRUE if the given user has permission to edit the given status; FALSE
 *   otherwise.
 */
function statuses_user_access_edit($status, $account = NULL) {
  $account = (empty($account) ? $GLOBALS['user'] : $account);
  $context = statuses_determine_context($status->type);
  return $context['handler']->access_edit($status, $account);
}

/**
 * Checks permission to view a status.
 *
 * @param $status
 *   The status being considered for viewing.
 * @param $account
 *   The user account object of the user taking the action.
 * @return
 *   TRUE if the given user has permission to view the given status; FALSE
 *   otherwise.
 */
function statuses_user_access_view($status, $account = NULL) {
  $account = (empty($account) ? $GLOBALS['user'] : $account);
  $context = statuses_determine_context($status->type);
  return $context['handler']->access_view($status, $account);
}

/**
 * Checks permission to view a status stream.
 *
 * @param $recipient
 *   The owner of the status stream being considered for viewing.
 * @param $account
 *   The user account object of the user taking the action.
 * @return
 *   TRUE if the given user has permission to view the relevant stream; FALSE
 *   otherwise.
 */
function statuses_user_access_view_stream($recipient = NULL, $type = 'user', $account = NULL) {
  global $user;
  $account = (empty($account) ? $user : $account);
  $recipient = (empty($recipient) ? $user : $recipient);
  $context = statuses_determine_context($type);
  return $context['handler']->access_stream($recipient, $account);
}

/**
 * Checks permission to generate status updates via Devel Generate.
 */
function statuses_user_access_generate() {
  return user_access('delete all statuses') &&
    user_access('post status messages to other streams') &&
    user_access('send messages to all users at once') &&
    user_access('administer Statuses settings');
}
