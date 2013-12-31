<?php

/**
 * @file
 *   Allows sending a status message to every user.
 */

/**
 * Admin form callback.
 */
function statuses_announce_admin($form, &$form_state) {
  $form = array();
  drupal_set_message(t('<span class="error">Warning</span>: this can be VERY SLOW on sites with lots of users.'), 'warning');
  $form['from'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender'),
    '#description' => t('The username of the user from whom the messages will be sent.'),
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => TRUE,
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#required' => TRUE,
  );
  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  return $form;
}

/**
 * Admin form submit callback.
 */
function statuses_announce_admin_submit($form, &$form_state) {
  $v = $form_state['values'];
  $from = user_load_by_name($v['from']);
  if (!$from->uid) {
    $from = FALSE;
  }

  $count = db_query("SELECT COUNT(*) FROM {users} WHERE status = 1")->fetchField();
  $batch = array(
    'operations' => array(
      array('statuses_announce_generate_status', array($v['message'], $from, $count)),
    ),
    'finished' => 'statuses_announce_batch_finished',
    'title' => t('Sending message to all users'),
    'file' => drupal_get_path('module', 'statuses') . '/includes/utility/statuses.announce.inc',
  );
  batch_set($batch);
}

/**
 * Sends the status message to users.
 *
 * @param $message
 *   The message that will be sent.
 * @param $from
 *   The $user object of the status poster.
 * @param $count
 *   The number of active users.
 */
function statuses_announce_generate_status($message, $from, $count, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }
  $uid = db_query_range("SELECT uid FROM {users} WHERE status = 1", $context['sandbox']['progress'], 1)->fetchField();
  $to = user_load($uid);
  statuses_save_status($to, 'user', $message, $from);
  $context['message'] = t('Sending message to %user (@current of @total)',
    array('%user' => $to->name, '@current' => $context['sandbox']['progress'], '@total' => $count));
  $context['sandbox']['progress']++;
  $context['results'][] = check_plain($to->name);
  $context['finished'] = $context['sandbox']['progress'] / $count;
}

/**
 * Batch finished callback.
 */
function statuses_announce_batch_finished($success, $results, $operations) {
  if ($success) {
    $count = count($results);
    if ($count < 50) {
      drupal_set_message(format_plural(count($results),
        'Sent message to 1 user: !list',
        'Sent message to @count users: !list',
        array('!list' => theme('item_list', array('items' => $results)))
      ));
    }
    else {
      drupal_set_message(t('Sent message to @count users.', array('@count' => $count)));
    }
  }
  else {
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing %error_operation with arguments: @arguments',
      array(
        '%error_operation' => $error_operation[0],
        '@arguments' => print_r($error_operation[1], TRUE),
      )
    ));
  }
}
