<?php

/**
 * @file
 *   Defines API hooks for the Statuses Comments module.
 */

/**
 * React to a comment being saved.
 *
 * @param $comment
 *   The newly saved comment object.
 * @param $edit
 *   TRUE if the comment was just edited; FALSE if it was just created.
 * @see fbss_comments_save_comment()
 * @see fbss_comments_edit_submit()
 */
function hook_fbss_comments_after_save($comment, $edit) {
  if ($edit) {
    drupal_set_message(t('The comment has been saved.'));
  }
  else {
    drupal_set_message(t('The comment has been updated.'));
  }
}

/**
 * React to a comment being deleted.
 *
 * @param $cid
 *   The ID of the comment that was just deleted.
 * @see fbss_comments_delete_comment()
 */
function hook_fbss_comments_delete($cid) {
  drupal_set_message(t('The comment has been deleted.'));
}

/**
 * Alter the permissions to take action on a comment.
 *
 * @param $allow
 *   Whether the user will be allowed to take action on the comment. Only set
 *   this to FALSE if you want to explicitly deny access. Setting this to TRUE
 *   defaults to the built-in access controls.
 * @param $op
 *   The action being taken on the comment. One of view, post, edit, delete.
 * @param $comment
 *   The comment object on which the action is being taken.
 * @param $account
 *   The user object of the person taking the action.
 * @see fbss_comments_can()
 */
function hook_fbss_comments_has_permission_alter(&$allow, $op, $comment, $account) {
}
