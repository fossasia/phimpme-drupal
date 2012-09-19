<?php

/**
 * Implements hook_rules_event_info().
 */
function fbss_comments_rules_rules_event_info() {
  return array(
    'fbss_comments_save' => array(
      'label' => t('User saves a new comment on a status'),
      'module' => 'Statuses Comments',
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
    ),
    'fbss_comments_edit' => array(
      'label' => t('User edits a comment on a status'),
      'module' => 'Statuses Comments',
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
    ),
    'fbss_comments_delete' => array(
      'label' => t('User deletes a comment on a status'),
      'module' => 'Statuses Comments',
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function fbss_comments_rules_rules_condition_info() {
  return array(
    'fbss_comments_rules_on_own' => array(
      'label' => t('Comment was posted on own status'),
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
      'module' => 'Statuses Comments',
    ),
    'fbss_comments_rules_can_post' => array(
      'label' => t('User has permission to send a status comment'),
      'arguments' => array(),
      'module' => 'Statuses Comments',
    ),
    'fbss_comments_rules_can_edit' => array(
      'label' => t('User has permission to edit a status comment'),
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
      'module' => 'Statuses Comments',
    ),
    'fbss_comments_rules_can_delete' => array(
      'label' => t('User has permission to delete a status comment'),
      'arguments' => array(
        'comment' => array(
          'type' => 'fbss_comment',
          'label' => t('The status comment.'),
        ),
      ),
      'module' => 'Statuses Comments',
    ),
  );
}

/**
 * Check if a comment was posted on a user's own status.
 */
function fbss_comments_rules_on_own($comment) {
  $status = statuses_load($comment->sid);
  return $comment->uid == $status->sender;
}

/**
 * Check if a user can comment on a status.
 */
function fbss_comments_rules_can_post() {
  return fbss_comments_can('post', NULL);
}

/**
 * Check if a user can edit a status comment.
 */
function fbss_comments_rules_can_edit($comment) {
  return fbss_comments_can('edit', $comment);
}

/**
 * Check if a user can delete a status comment.
 */
function fbss_comments_rules_can_delete($comment) {
  return fbss_comments_can('delete', $comment);
}

/**
 * Implements hook_rules_action_info().
 */
function fbss_comments_rules_rules_action_info() {
  return array(
    'fbss_comments_rules_load_action' => array(
      'label' => t('Load a status comment'),
      'new variables' => array(
        'comment_loaded' => array(
          'type' => 'fbss_comment',
          'label' => t('Loaded status comment'),
        ),
      ),
      'help' => t('Enter the Comment ID of a status comment to load.'),
      'module' => 'Statuses Comments',
      'eval input' => array('cid'),
    ),
    'fbss_comments_rules_edit_action' => array(
      'label' => t('Edit a status comment'),
      'help' => t('Enter the Status Comment ID of the status comment to edit and the text you wish to replace the comment.'),
      'module' => 'Statuses Comments',
      'eval input' => array('cid', 'message'),
    ),
    'fbss_comments_rules_delete_action' => array(
      'label' => t('Delete a status comment'),
      'help' => t('Enter the Status Comment ID of the status comment to delete.'),
      'module' => 'Statuses Comments',
      'eval input' => array('cid'),
    ),
    'fbss_comments_rules_add_action' => array(
      'label' => t('Add a status comment'),
      'help' => t('Enter the status comment text, the ID of the relevant status, and the user ID of the creator of the comment.'),
      'module' => 'Statuses Comments',
      'eval input' => array('sid', 'message', 'uid'),
    ),
  );
}

/**
 * Builds the form for loading a status comment.
 */
function fbss_comments_rules_load_action_form($settings, &$form) {
  $settings += array('cid' => '');
  $form['settings']['cid'] = array(
    '#type' => 'textfield',
    '#title' => t('Status Comment ID'),
    '#default_value' => $settings['cid'],
    '#required' => TRUE,
  );
}

/**
 * Loads a status.
 */
function fbss_comments_rules_load_action($settings) {
  return array('comment_loaded' => fbss_comments_load($settings['cid']));
}

/**
 * Builds the form for editing a status comment.
 */
function fbss_comments_rules_edit_action_form($settings, &$form) {
  $settings += array(
    'cid' => '',
    'message' => '',
  );
  $form['settings']['cid'] = array(
    '#type' => 'textfield',
    '#title' => t('Status Comment ID'),
    '#default_value' => $settings['cid'],
    '#required' => TRUE,
  );
  $form['settings']['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Comment message'),
    '#default_value' => $settings['message'],
    '#rows' => 3,
  );
}

/**
 * Edits a status.
 */
function fbss_comments_rules_edit_action($settings) {
  // TODO Please review the conversion of this statement to the D7 database API syntax.
  /* db_query("UPDATE {fbss_comments} SET comment = '%s' WHERE cid = %d", $settings['message'], $settings['cid']) */
  db_update('fbss_comments')
  ->fields(array(
    'comment' => $settings['message'],
  ))
  ->condition('cid', $settings['cid'])
  ->execute();
  $c = fbss_comments_load($settings['cid']);
  module_invoke_all('fbss_comments_after_save', $c, TRUE);
  if (module_exists('trigger')) {
    module_invoke_all('fbss_comments', 'fbss_comments_edited', $c);
  }
}

/**
 * Builds the form for deleting a status.
 */
function fbss_comments_rules_delete_action_form($settings, &$form) {
  $settings += array('cid' => '');
  $form['settings']['cid'] = array(
    '#type' => 'textfield',
    '#title' => t('Status Comment ID'),
    '#default_value' => $settings['cid'],
    '#required' => TRUE,
  );
}

/**
 * Deletes a status.
 */
function fbss_comments_rules_delete_action($settings) {
  fbss_comments_delete_comment($settings['cid']);
}

/**
 * Builds the form for adding a status.
 */
function fbss_comments_rules_add_action_form($settings, &$form) {
  $settings += array(
    'sid' => '',
    'message' => '',
    'uid' => '',
  );
  $form['settings']['sid'] = array(
    '#type' => 'textfield',
    '#title' => t('Status ID'),
    '#description' => t('Enter the ID of the status on which this comment will be posted.'),
    '#default_value' => $settings['sid'],
    '#required' => TRUE,
  );
  $form['settings']['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Comment message'),
    '#default_value' => $settings['message'],
    '#rows' => 3,
    '#required' => TRUE,
  );
  $form['settings']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#description' => t('Enter the ID of the user who created this comment.'),
    '#default_value' => $settings['uid'],
  );
}

/**
 * Adds a status.
 */
function fbss_comments_rules_add_action($settings) {
  fbss_comments_save_comment($settings['sid'], $settings['message'], empty($settings['uid']) ? $GLOBALS['user']->uid : $settings['uid']);
}

/**
 * Implements hook_rules_data_type_info().
 */
function fbss_comments_rules_rules_data_type_info() {
  return array(
    'fbss_comment' => array(
      'label' => t('Facebook-style Status Comment'),
      'class' => 'rules_data_type_fbss_comment',
      'savable' => FALSE,
      'identifiable' => TRUE,
      'use_input_form' => FALSE,
      'module' => 'Statuses Comments',
    ),
  );
}

/**
 * Defines the rules node data type.
 */
class rules_data_type_fbss_comment extends rules_data_type {
  function save() {
    $comment = &$this->get();
    fbss_comments_save_comment($comment->sid, $comment->comment, $comment->uid);
    return TRUE;
  }
  function load($cid) {
    return fbss_comments_load($cid);
  }
  function get_identifier() {
    $status = &$this->get();
    return $status->cid;
  }
}
