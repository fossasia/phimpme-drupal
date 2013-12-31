<?php

/**
 * @file
 *   Form/page callbacks for the Statuses Comments module.
 */

/**
 * The administrative settings form.
 */
function fbss_comments_admin($form, &$form_state) {
  $form = array();
  $form['fbss_comments_hide_small'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum number of comments at which some will be hidden'),
    '#description' => t('When there are at least this many comments on a status, some will be hidden behind a "Show all comments" link.'),
    '#default_value' => variable_get('fbss_comments_hide_small', 3),
    '#size' => 4,
    '#maxlength' => 4,
  );
  $form['fbss_comments_show_all'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum number of comments at which the "Show all comments" link will redirect the user to the individual status page'),
    '#description' => t('When there are at least this many comments on a status, the user will be directed to the status page if they try to view them all.'),
    '#default_value' => variable_get('fbss_comments_show_all', 10),
    '#size' => 4,
    '#maxlength' => 4,
  );
  if (module_exists('imagecache_profiles')) {
    $form['fbss_comments_user_pictures'] = array(
      '#title' => t('Imagecache preset for user pictures in comments'),
      '#type' => 'select',
      '#options' => image_style_options(TRUE),
      '#default_value' => variable_get('fbss_comments_user_pictures', 0),
    );
  }
  else {
    $form['fbss_comments_user_pictures'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show user pictures on comments'),
      '#default_value' => variable_get('fbss_comments_user_pictures', 0),
    );
  }
  $form['fbss_comments_enter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Submit the comment form when pressing Enter'),
    '#description' => t('If enabled, the comment form will be submitted when the user presses Enter.') . ' ' .
      t('Users can still create line breaks by pressing Shift+Enter.'),
    '#default_value' => variable_get('fbss_comments_enter', 0),
  );
  $form['fbss_comments_ahah'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use AHAH to refresh status comment forms without refreshing the page'),
    '#description' => '<strong>' . t('Disable this if comments are being loaded onto the page via AJAX by a module other than FBSS or Views.') . '</strong>',
    '#default_value' => variable_get('fbss_comments_ahah', 1),
  );
  return system_settings_form($form);
}

/**
 * Validate the administrative settings form.
 */
function fbss_comments_admin_validate($form, $form_state) {
  $v = $form_state['values'];
  if (!is_numeric($v['fbss_comments_hide_small']) || $v['fbss_comments_hide_small'] < 1) {
    form_set_error('fbss_comments_hide_small', t('The minimum number of comments at which some will be hidden must be a positive integer.'));
  }
  if (!is_numeric($v['fbss_comments_show_all']) || $v['fbss_comments_show_all'] < 1) {
    form_set_error('fbss_comments_show_all', t('The minimum number of comments at which the "Show all comments" link will redirect the user to the individual status page must be a positive integer.'));
  }
  if ($v['fbss_comments_show_all'] < $v['fbss_comments_hide_small']) {
    form_set_error('fbss_comments_show_all', t('The minimum number of comments at which the "Show all comments" link will redirect the user to the individual status page must be less than the minimum number of comments at which some will be hidden.'));
  }
}

/**
 * The edit form for status comments.
 *
 * @param $comment
 *   The comment object for the status comment being edited.
 */
function fbss_comments_edit($form, $form_state, $comment) {
  $form = array();
  $form['status-comment-edit'] = array(
    '#type' => 'textarea',
    '#rows' => 3,
    '#required' => TRUE,
    '#default_value' => $comment->comment,
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  $form['#cid'] = $comment->cid;
  if (module_exists('modalframe')) {
    modalframe_child_js();
  }
  return $form;
}

/**
 * The submit handler for the edit form for status comments.
 */
function fbss_comments_edit_submit($form, &$form_state) {
  // TODO Please review the conversion of this statement to the D7 database API syntax.
  /* db_query("UPDATE {fbss_comments} SET comment = '%s' WHERE cid = %d", $form_state['values']['status-comment-edit'], $form['#cid']) */
  db_update('fbss_comments')
  ->fields(array(
    'comment' => $form_state['values']['status-comment-edit'],
  ))
  ->condition('cid', $form['#cid'])
  ->execute();
  $c = fbss_comments_load($form['#cid']);
  module_invoke_all('fbss_comments_after_save', $c, TRUE);
  if (module_exists('trigger')) {
    module_invoke_all('fbss_comments', 'fbss_comments_edited', $c);
  }
  if ($_GET['destination'] && $_GET['destination'] != 'fbss_comments/js/refresh') {
    $form_state['redirect'] = $_GET['destination'];
  }
  else {
    $form_state['redirect'] = '<front>';
  }
  drupal_set_message(t('Status comment has been successfully edited.'));
  if (module_exists('modalframe')) {
    modalframe_close_dialog();
  }
}

/**
 * The delete form for status comments.
 *
 * @param $comment
 *   The comment object for the status comment being deleted.
 */
function fbss_comments_delete($form, $form_state, $comment) {
  $form = array();
  $form['infotext'] = array(
    '#value' => '<p>' . t('Are you sure you want to permanently delete the status comment %comment?',
      array('%comment' => $comment->comment)) . '</p>',
  );
  $form['confirm'] = array(
    '#type' => 'submit',
    '#value' => t('Confirm'),
    '#submit' => array('fbss_comments_delete_confirm'),
  );
  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('fbss_comments_delete_cancel'),
  );
  $form['#cid'] = $comment->cid;
  if (module_exists('modalframe')) {
    modalframe_child_js();
  }
  return $form;
}

/**
 * The confirmation submit handler for the delete form for status comments.
 */
function fbss_comments_delete_confirm($form, &$form_state) {
  fbss_comments_delete_comment($form['#cid']);
  drupal_set_message(t('Status comment deleted.'));
  if (!empty($_GET['destination']) && $_GET['destination'] != 'fbss_comments/js/refresh') {
    $form_state['redirect'] = $_GET['destination'];
  }
  else {
    $form_state['redirect'] = '<front>';
  }
  if (module_exists('modalframe')) {
    modalframe_close_dialog();
  }
}

/**
 * The cancellation submit handler for the delete form for status comments.
 */
function fbss_comments_delete_cancel($form, &$form_state) {
  drupal_set_message(t('Status comment was not deleted.'));
  if ($_GET['destination'] && $_GET['destinaton'] != 'fbss_comments/js/refresh') {
    $form_state['redirect'] = $_GET['destination'];
  }
  else {
    $form_state['redirect'] = '<front>';
  }
  if (module_exists('modalframe')) {
    modalframe_close_dialog();
  }
}
