<?php

/**
 * @file
 *   Provides Devel Generate integration.
 */

/**
 * The Generate status comments form.
 */
function fbss_comments_generate_form($form, $form_state) {
  $form['killswitch'] = array(
    '#type' => 'checkbox',
    '#title' => t('<strong>Delete all status comments</strong> before generating new content'),
    '#default_value' => FALSE,
  );
  $form['num'] = array(
    '#type' => 'textfield',
    '#title' => t('How many status comments would you like to generate?'),
    '#default_value' => 50,
    '#size' => 10,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate status comments'),
  );
  return $form;
}

/**
 * The validation handler for the Generate status comments form.
 */
function fbss_comments_generate_form_validate($form, &$form_state) {
  $n = $form_state['values']['num'];
  //A user might want to "generate" 0 status comments if they really just want to delete everything.
  if (!is_numeric($n) || $n < 0 || $n != round($n)) {
    form_set_error('num', t('You must generate at least 0 status comments.'));
  }
}

/**
 * The submit handler for the Generate status comments form.
 */
function fbss_comments_generate_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $operations = array();
  if ($v['killswitch']) {
    $operations[] = array('fbss_comments_generate_killswitch', array());
  }
  if ($v['num'] > 50 || $v['killswitch']) {
    for ($i = 0; $i < $v['num']; $i += 50) {
      $count = ($v['num'] - $i < 50 ? $v['num'] - $i : 50);
      $operations[] = array('fbss_comments_generate_status_comment_bulk', array($count));
    }
    batch_set(array(
      'title' => t('Generating status comments'),
      'operations' => $operations,
      'finished' => 'fbss_comments_generate_finished',
      'file' => drupal_get_path('module', 'fbss_comments') . '/fbss_comments.generate.inc',
    ));
  }
  else {
    $context = array('results' => array('num' => 0));
    for ($i = 0; $i < $v['num']; $i++) {
      fbss_comments_generate_status_comment($context);
    }
    fbss_comments_generate_finished(TRUE, $context['results'], array());
  }
}

/**
 * Creates a lot of status updates at once.
 *
 * @param $count
 *   The number of status comments to create.
 * @param $time
 *   The number of seconds ago that corresponds to the created time of the
 *   oldest status we could create.
 */
function fbss_comments_generate_status_comment_bulk($count, &$context) {
  for ($i = 0; $i < $count; $i++) {
    fbss_comments_generate_status_comment($context);
  }
}

/**
 * Generates a status.
 *
 * @param $time
 *   The number of seconds ago that corresponds to the created time of the
 *   oldest status we could create.
 */
function fbss_comments_generate_status_comment(&$context) {
  module_load_include('inc', 'devel_generate', 'devel_generate');

  $sid = db_select('statuses')
    ->fields('statuses',  array('sid'))
    ->orderBy('RAND()')
    ->execute()
    ->fetchField();

  $mtext = devel_create_greeking(mt_rand(2, variable_get('statuses_length', 140) / 7));
  $text = drupal_substr($mtext, 0, variable_get('statuses_length', 140));

  $uids = devel_get_users();
  $uid = 0;
  while ($uid == 0) {
    $uid = $uids[array_rand($uids)];
  }

  fbss_comments_save_comment($sid, $text, $uid);
  $context['results']['num']++;
}

/**
 * Deletes all status comments.
 */
function fbss_comments_generate_killswitch(&$context) {
  $result = db_select('fbss_comments')
    ->fields('fbss_comments',  array('cid'))
    ->execute()
    ->fetchAll();
  foreach ($result as $comment) {
    fbss_comments_delete_comment($comment->cid);
  }
  drupal_set_message(format_plural(count($result), 'Deleted one status comment', 'Deleted @count status comments'));
}

/**
 * Alerts user that the batch processing is complete.
 */
function fbss_comments_generate_finished($success, $results, $operations) {
  $message = t('Finished creating status comments with an error.');
  if ($success && $results['num']) {
    $message = t('Finished creating @num status comments sucessfully.', array('@num' => $results['num']));
  }
  elseif ($success) {
    $message = t('0 status comments processed.');
  }
  drupal_set_message($message);
}
