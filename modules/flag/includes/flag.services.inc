<?php

/**
 * @file
 * Services integration for the Flag module.
 */

/**
 * Access callback to check a user has access to a flag operation via Services.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID that should be flagged.
 * @param $uid
 *   Optional. The user ID on behalf to flag the content.
 * @param $action
 *   Optional. If the method is "flag", then pass the desired action which
 *   should be "flag" or "unflag".
 * @return
 *   TRUE if access is allowed.
 */
function flag_service_flag_access($flag_name, $content_id, $uid = NULL, $action = NULL) {
  $flag = flag_get_flag($flag_name);
  if (!$flag) {
    // Flag does not exist.
    return FALSE;
  }
  if (empty($action)) {
    return TRUE;
  }
  else {
    // Check action is valid.
    if (!in_array($action, array('flag', 'unflag'))) {
      return FALSE;
    }
    if ($uid) {
      $account = user_load($uid);
    }
    else {
      global $user;
      $account = $user;
    }

    if (empty($account) || !$flag->access($content_id, $action, $account)) {
      // User has no permission to use this flag.
      return FALSE;
    }
    return TRUE;
  }
}

/**
 * Service wrapper to flag a content.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID to check if it was flagged.
 * @param $uid
 *   The user ID to check if they flagged the content.
 * @param $action
 *   The action. Should be "flag" or "unflag".
 * @return
 *   TRUE if content was flagged.
 */
function flag_service_flag($flag_name, $content_id, $uid = NULL, $action = 'flag', $skip_permission_check = FALSE) {
  global $user;
  $account = empty($uid) ? $user : user_load($uid);
  $flag = flag_get_flag($flag_name);
  $skip_permission_check = (boolean) $skip_permission_check;
  return $flag->flag($action, $content_id, $account, $skip_permission_check);
}

/**
 * Service wrapper to check if a content is flagged by a user.
 *
 * @param $flag_name
 *   The flag name.
 * @param $content_id
 *   The content ID to check if it was flagged.
 * @param $uid
 *   The user ID to check if they flagged the content.
 * @return
 *   TRUE if content was flagged.
 */
function flag_service_is_flagged($flag_name, $content_id, $uid = NULL) {
  $flag = flag_get_flag($flag_name);
  return $flag->is_flagged($content_id, $uid);
}
