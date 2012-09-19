<?php

/**
 * @file
 *   Provides Devel Generate integration.
 */

/**
 * The Generate statuses form.
 */
function statuses_generate_form($form_state) {
  $form['killswitch'] = array(
    '#type' => 'checkbox',
    '#title' => t('<strong>Delete all statuses</strong> before generating new content'),
    '#default_value' => FALSE,
  );
  if (module_exists('statuses_tags')) {
    $form['tags'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include #hashtags and @mentions in the statuses'),
      '#default_value' => FALSE,
    );
  }
  $form['types'] = array(
    '#type' => 'checkboxes',
    '#title' => t("Post to stream types"),
    '#required' => TRUE,
    '#options' => drupal_map_assoc(array_keys(statuses_all_contexts())),
    '#default_value' => array('user' => 'user'),
  );
  $options = array(1 => t('Now'));
  foreach (array(3600, 86400, 604800, 2592000, 31536000) as $interval) {
    $options[$interval] = t('!time ago', array('!time' => format_interval($interval, 1)));
  }
  $form['time'] = array(
    '#type' => 'select',
    '#title' => t('How far back in time should the statuses be dated?'),
    '#description' => t('Status creation dates will be distributed randomly from the current time back to the selected time.'),
    '#options' => $options,
    '#default_value' => 604800,
    '#required' => TRUE,
  );
  $form['num'] = array(
    '#type' => 'textfield',
    '#title' => t('How many statuses would you like to generate?'),
    '#default_value' => 50,
    '#size' => 10,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate status messages'),
  );
  return $form;
}

/**
 * The validation handler for the Generate statuses form.
 */
function statuses_generate_form_validate($form, &$form_state) {
  $n = $form_state['values']['num'];
  // A user might want to "generate" 0 statuses if they really just want to delete everything.
  if (!is_numeric($n) || $n < 0 || $n != round($n)) {
    form_set_error('num', t('You must generate at least 0 statuses.'));
  }
}

/**
 * The submit handler for the Generate statuses form.
 */
function statuses_generate_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  $operations = array();
  if ($v['killswitch']) {
    $operations[] = array('statuses_generate_killswitch', array());
  }
  if ($v['num'] > 50 || $v['killswitch']) {
    for ($i = 0; $i < $v['num']; $i += 50) {
      $count = ($v['num'] - $i < 50 ? $v['num'] - $i : 50);
      $operations[] = array('statuses_generate_status_bulk', array($count, $v['time'], $v['types'], $v['tags']));
    }
    batch_set(array(
      'title' => t('Generating statuses'),
      'operations' => $operations,
      'finished' => 'statuses_generate_finished',
      'file' => drupal_get_path('module', 'statuses') . '/includes/utility/statuses.generate.inc',
    ));
  }
  else {
    $context = array('results' => array('num' => 0));
    for ($i = 0; $i < $v['num']; $i++) {
      statuses_generate_status($v['time'], $v['types'], $v['tags'], $context);
    }
    statuses_generate_finished(TRUE, $context['results'], array());
  }
}

/**
 * Creates a lot of status updates at once.
 *
 * @param $count
 *   The number of statuses to create.
 * @param $time
 *   The number of seconds ago that corresponds to the created time of the
 *   oldest status we could create.
 * @param $types
 *   An array of context stream types to which status messages can be posted.
 * @param $tags
 *   TRUE to make some statuses include #hashtags and @mentions.
 */
function statuses_generate_status_bulk($count, $time, $types, $tags, &$context) {
  for ($i = 0; $i < $count; $i++) {
    statuses_generate_status($time, $types, $tags, $context);
  }
}

/**
 * Generates a status.
 *
 * @param $time
 *   The number of seconds ago that corresponds to the created time of the
 *   oldest status we could create.
 * @param $types
 *   An array of context stream types to which status messages can be posted.
 * @param $tags
 *   TRUE to make some statuses include #hashtags and @mentions.
 */
function statuses_generate_status($time, $types, $tags, &$context) {
  module_load_include('inc', 'devel_generate', 'devel_generate');
  $stime = REQUEST_TIME - mt_rand(0, $time);
  $uids = devel_get_users();
  $uid = 0;
  while ($uid == 0) {
    $uid = $uids[array_rand($uids)];
  }

  foreach ($types as $key => $type) {
    if (!$type) {
      unset($types[$key]);
    }
  }
  $types = array_values($types);
  $type = $types[array_rand($types)];
  $ctxt = statuses_determine_context($type);
  $recipient = $ctxt['handler']->load_random_recipient();
  if (!$ctxt['handler']->recipient_id($recipient)) {
    if (!isset($context['results']['recipient_type_error'])) {
      $context['results']['recipient_type_error'] = array();
    }
    $context['results']['recipient_type_error'][] = $type;
    return;
  }

  $text = '';
  // Mentions.
  if (module_exists('statuses_tags') && $tags && $type == 'user' && mt_rand(0, 1)) {
    $rid = 0;
    while ($rid == 0 || $rid == $uid) {
      $rid = $uids[array_rand($uids)];
    }
    $person = user_load($rid);
    $text .= '@' . $person->name . ' ';
    if (preg_match('/.+\b.+/', $person->name)) {
      $text .= '[@' . $person->name . '] ';
    }
  }

  $mtext = devel_create_greeking(mt_rand(2, variable_get('statuses_length', 140) / 7));
  $text .= drupal_substr($mtext, 0, variable_get('statuses_length', 140) - drupal_strlen($text));
  // Hashtags.
  if (module_exists('statuses_tags') && $tags && variable_get('statuses_tags_vid', -1) != -1 && mt_rand(0, 2) > 0) {
    $word = devel_generate_word(mt_rand(3, 7));
    if (drupal_strlen($text . $word) + 2 <= variable_get('statuses_length', 140)) {
      $text .= ' #' . $word;
    }
    $word = devel_generate_word(mt_rand(3, 5));
    $word2 = devel_generate_word(mt_rand(3, 5));
    if (drupal_strlen($text . $word . $word2) + 5 <= variable_get('statuses_length', 140)) {
      $text .= ' [#' . $word . ' ' . $word2 . ']';
    }
  }

  $options = array(
    'discard duplicates' => FALSE,
    'timed override' => FALSE,
    'discard blank statuses' => FALSE,
  );
  statuses_save_status($recipient, $type, $text, user_load($uid), $options);
  $context['results']['num']++;
}

/**
 * Deletes all statuses.
 */
function statuses_generate_killswitch(&$context) {
  $result = db_select('statuses')
    ->fields('statuses',  array('sid'))
    ->execute()
    ->fetchAll();
  foreach ($result as $status) {
    statuses_delete_status($status->sid);
  }
  drupal_set_message(format_plural(count($result), 'Deleted one status', 'Deleted @count statuses'));
}

/**
 * Alerts user that the batch processing is complete.
 */
function statuses_generate_finished($success, $results, $operations) {
  $message = t('Finished creating statuses with an error.');
  if (isset($results['recipient_type_error'])) {
    drupal_set_message(format_plural(
      count($results['recipient_type_error']),
      'Could not find recipient for context stream type %types',
      'Could not find recipient for context stream types %types',
      array('%types' => implode(', ', $results['recipient_type_error']))
    ));
  }
  if ($success && $results['num']) {
    $message = format_plural($results['num'], 'Finished creating 1 status successfully.', 'Finished creating @count statuses successfully.');
  }
  elseif ($success) {
    $message = t('Finished processing 0 statuses.');
  }
  drupal_set_message($message);
}
