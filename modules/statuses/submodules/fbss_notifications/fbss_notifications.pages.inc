<?php

/**
 * @file
 *   Page callbacks and functions for the Statuses Notifications module.
 */

//================
// MENU CALLBACKS
//================

/**
 * Page callback for admin/settings/notifications/status.
 * Display global settings for what subscriptions are enabled and where they are displayed.
 */
function fbss_notifications_settings_form($form) {
  $form = array();
  $options = _notifications_subscription_types('long', array('event_type' => 'status')) + _notifications_subscription_types('long', array('event_type' => 'stream'));
  $form['fbss_notifications_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Global options'),
    '#options' => $options,
    '#default_value' => variable_get('fbss_notifications_type', array()),
    '#description' => t('Define the available subscription types that will be enabled globally'),
  );
  $options = array(
    'user' => t('User profiles'),
    'node' => t('Nodes'),
  );
  if (module_exists('og')) {
    $options['og'] = t('Organic groups');
  }
  $form['fbss_notifications_links'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show "subscribe to status messages" links on these entities'),
    '#options' => $options,
    '#default_value' => variable_get('fbss_notifications_links', array('user' => 'user', 'node' => 0, 'og' => 'og')),
  );
  return system_settings_form($form);
}

/**
 * Page callback for user/%user/notifications/status.
 * List status subscriptions.
 *
 * 'type' is 'status', analogous to 'thread'
 * 'event_type' is 'status', analogous to 'node'
 */
function fbss_notifications_page_thread($account = NULL) {
  if (empty($account)) {
    $account = $GLOBALS['user'];
  }
  // query string for status subscriptions
  $result = pager_query("
    SELECT
      n.sid, n.uid, n.type, n.event_type, n.conditions, n.send_interval, n.send_method, n.cron, n.module, n.status, n.destination,
      nf.value AS nf_sid,
      fs.sid as fs_sid, fs.sender, fs.recipient, fs.type as fs_type, fs.message, fs.created
    FROM {notifications} n
    INNER JOIN {notifications_fields} nf
      ON n.sid = nf.sid
    LEFT JOIN {statuses} fs
      ON nf.intval = fs.sid
    WHERE
      n.uid = %d AND
      n.type = 'status' AND
      n.event_type = 'status' AND
      n.conditions = 1 AND
      nf.field = 'sid'
    ORDER BY
      fs.type ASC,
      fs.sid DESC
  ", FBSS_NOTIFICATIONS_PAGER, 0, NULL, $account->uid);
  $subscriptions = $list = array();
  $content_types = statuses_all_contexts();
  while ($sub = db_fetch_object($result)) {
    $subscriptions[$sub->sid] = $sub;
    $message = $sub->message;
    // 100 is an arbitrary length.
    if (drupal_strlen($message) > 100) {
      // "\xE2\x80\xA6" is the unicode escape sequence for the HTML entity &hellip; (an ellipsis)
      $message = drupal_substr($message, 0, 99) . "\xE2\x80\xA6";
    }
    $list[$sub->fs_sid] = '[' . $content_types[$sub->fs_type]['title'] . '] ' . l($message, 'statuses/' . $sub->fs_sid);
  }
  if (empty($subscriptions)) {
    $output = t('You are not currently subscribed to any active threads');
  }
  else {
    $output = t('You are currently subscribed to the following threads:');
    $defaults = array(
      'type' => 'status',
      'event_type' => 'status',
    );
    $options = array('title' => t('Message'));
    $output .= drupal_get_form('notifications_user_form', $account, 'status', $subscriptions, $list, $defaults, $options);
    $output .= theme('pager', array('tags' => NULL));
  }
  return $output;
}

/**
 * Page callback for user/%user/notifications/status-$type.
 * List stream subscriptions.
 *
 * 'type' is arg(3), analogous to 'author'
 * 'event_type' is 'status', analogous to 'node'
 */
function fbss_notifications_page_type($account = NULL) {
  if (empty($account)) {
    $account = $GLOBALS['user'];
  }
  $type = drupal_substr(arg(3), 0, -7); // $status->type
  if (!empty($type)) {
    $context = statuses_determine_context($type);
  }
  else {
    return NULL;
  }
  // List all author subscriptions and build author list with the same query
  $subscriptions = $list = array();
  $result = pager_query("
    SELECT
      n.sid as sub_id, n.uid, n.type, n.event_type, n.conditions, n.send_interval, n.send_method, n.cron, n.module, n.status, n.destination,
      f.intval,
      fs.sid as fs_sid, fs.sender, fs.recipient, fs.type as fs_type, fs.message, fs.created
    FROM {notifications} n
    INNER JOIN {notifications_fields} f ON f.sid = n.sid
    LEFT JOIN {statuses} fs ON fs.sid = f.intval
    WHERE n.uid = %d AND n.type = '%s' AND n.event_type = 'stream'
  ", FBSS_NOTIFICATIONS_PAGER, 0, NULL, $account->uid, arg(3));
  while ($sub = db_fetch_object($result)) {
    $recipient = $context['handler']->load_recipient($sub->recipient);
    $list[$sub->recipient] = $context['handler']->recipient_link($recipient);
    $sub->fields['recipient'] = $sub->intval;
    $sub->fields['fs_type'] = $type;
    $sub->fields['fs_sid'] = $sub->fs_sid;
    $sub->fields['message'] = $sub->message;
    $subscriptions[$sub->sid] = $sub;
  }
  if (empty($subscriptions)) {
    $output = t('There are no active !type subscriptions.', array('!type' => $context['title']));
  }
  else {
    $defaults = array(
      'type' => arg(3),
      'event_type' => 'status',
    );
    $options = array('title' => t('Subscribed to'));
    $output = drupal_get_form('notifications_user_form', $account, arg(3), $subscriptions, $list, $defaults, $options);
    $output .= theme('pager', array('tags' => NULL));
  }
  return $output;
}

//==================
// OPTION CALLBACKS
//==================

/**
 * Callback for the "Recipient type" option on the "add subscription" form.
 */
function fbss_notifications_types_callback() {
  $types = array();
  foreach (statuses_all_contexts() as $type => $info) {
    $types[$type] = $info['title'];
  }
  if (arg(0) == 'user' && is_numeric(arg(1)) && arg(2) == 'notifications' && arg(3) == 'add') {
    $type = drupal_substr(arg(4), 0, -7);
    if (isset($types[$type])) {
      return array($type => $types[$type]);
    }
  }
  return $types;
}

/**
 * Autocompletes recipient names.
 */
function fbss_notifications_autocomplete_recipient($string = '') {
  /**
   * Each implementation should return an array like this:
   * array(
   *   'stream_type' => array(
   *     'raw_value_list' => 'HTML_safe_value',
   *     'raw_value_list, raw_value_list_2' => 'HTML_safe_value_2',
   *     ...
   *   ),
   *   ...
   * );
   * Technically there's no reason for this to support multiple-autocomplete,
   * but it can't hurt to have that capability.
   */
  $results = module_invoke_all('fbss_notifications_autocomplete_recipient', $string);
  $count = count($results);
  if ($count > 10) {
    $matches = $results['user'];
  }
  else {
    $total = 0;
    $matches = array();
    foreach ($results as $type => $values) {
      for ($i = 0; (($i <= 10 / $count && $total <= 10) || $i < 2) && $i < count($values); $i++) {
        $vals = array_values($values);
        $keys = array_keys($values);
        $matches[$keys[$i]] = $vals[$i];
        $total++;
      }
    }
  }
  drupal_json_output($matches);
}

/**
 * Given a recipient ID and a subscription type, returns the HTML-safe recipient name.
 */
function fbss_notifications_recipient_name_callback($id, $subs_type = '') {
  fbss_notifications_author_name($id, FALSE, $subs_type);
}

/**
 * Given a recipient ID and a subscription type, returns the recipient name.
 */
function fbss_notifications_author_name($id, $html = FALSE, $subs_type = '') {
  if (!empty($subs_type)) {
    $type = drupal_substr($subs_type, -7);
    $context = statuses_determine_context($type);
    if (!empty($context)) {
      $recipient = $context['handler']->load_recipient($id);
      return $html ? $context['handler']->recipient_link($recipient) : $context['handler']->recipient_name($recipient);
    }
  }
}

/**
 * Given a recipient name and a subscription type, returns the recipient ID.
 */
function fbss_notifications_author_uid($name, $field, $subs_type = '') {
  if (!empty($subs_type)) {
    $type = drupal_substr($subs_type, -7);
    foreach (statuses_all_contexts() as $type => $info) {
      if (isset($info['name to ID callback']) && function_exists($info['name to ID callback'])) {
        $id = call_user_func($info['name to ID callback'], $name, $type);
        if (!empty($id)) {
          return $id;
        }
      }
      switch ($type) {
        case 'og':
        case 'node':
          return db_query_range("SELECT nid FROM {node} WHERE title = :title", array(':title' => $name))->fetchField();
        case 'user':
          return db_query_range("SELECT uid FROM {users} WHERE name = :name", array(':name' => $name))->fetchField();
        case 'term':
          if (module_exists('taxonomy')) {
            return db_query_range("SELECT tid FROM {taxonomy_term_data} WHERE name = :name", array(':name' => $name))->fetchField();
          }
      }
    }
  }
  if ($field) {
    form_set_error($field, t('Stream not found.'));
  }
}

//======================
// HOOK IMPLEMENTATIONS
//======================

/**
 * Implements hook_fbss_notifications_autocomplete_recipient().
 */
function fbss_notifications_fbss_notifications_autocomplete_recipient($string) {
  $orig_string = $string;
  $array = drupal_explode_tags($string);
  $string = trim(array_pop($array));
  if (empty($string)) {
    return array();
  }
  $prefix = count($array) ? implode(', ', $array) . ', ' : '';
  global $user;
  $matches = array();

  // Users.
  $matches['user'] = array();
  // TODO Please convert this statement to the D7 database API syntax.
  $result = db_query_range("SELECT name FROM {users} WHERE LOWER(name) LIKE LOWER('%s%%')", $string);
  while ($account = db_fetch_object($result)) {
    $key = $prefix . _fbss_notifications_autocomplete_key_helper($account->name);
    $matches['user'][$key] = check_plain($account->name);
  }

  // Organic groups.
  if (module_exists('og')) {
    $matches['og'] = array();
    // TODO Please convert this statement to the D7 database API syntax.
    $result = db_query_range(db_rewrite_sql("
      SELECT n.nid, n.title
      FROM {og_uid} ou
        LEFT JOIN {og} og
          ON ou.nid = og.nid
        LEFT JOIN {node} n
          ON og.nid = n.nid
      WHERE
        LOWER(n.title) LIKE LOWER('%s%%') AND
        (og.og_private = 0 OR ou.uid = %d) AND
        n.status = 1
    "), $string, $user->uid);
    while ($node = db_fetch_object($result)) {
      $key = $prefix . _fbss_notifications_autocomplete_key_helper($node->title);
      $matches['og'][$key] = check_plain($node->title);
    }
  }

  $matches['node'] = array();
  if (module_exists('og')) {
    // TODO Please convert this statement to the D7 database API syntax.
    $result = db_query_range(db_rewrite_sql("
      SELECT n.nid, n.title
      FROM {node} n
        LEFT JOIN {og} o
          ON n.nid = og.nid
      WHERE
        og.nid IS NULL AND
        n.status = 1 AND
        LOWER(n.title) LIKE LOWER('%s%%')
    "), $string);
  }
  else {
    // TODO Please convert this statement to the D7 database API syntax.
    $result = db_query_range(db_rewrite_sql("
      SELECT nid, title
      FROM {node}
      WHERE
        status = 1 AND
        LOWER(title) LIKE LOWER('%s%%')
    "), $string);
  }
  while ($node = db_fetch_object($result)) {
    $key = $prefix . _fbss_notifications_autocomplete_key_helper($node->title);
    $matches['node'][$key] = check_plain($node->title);
  }

  // Taxonomy terms.
  if (module_exists('taxonomy') && module_exists('statuses_tags') && variable_get('statuses_tags_vid', -1) != -1) {
    $matches['term'] = array();
    // TODO Please convert this statement to the D7 database API syntax.
    $result = db_query_range("SELECT name FROM {statuses_tags} WHERE LOWER(name) LIKE LOWER('%s%%')", $string);
    while ($term = db_fetch_object($result)) {
      $key = $prefix . _fbss_notifications_autocomplete_key_helper($term->name);
      $matches['node'][$key] = check_plain($term->name);
    }
  }

  return $matches;
}

//==================
// HELPER FUNCTIONS
//==================

/**
 * Specially encodes autocomplete key strings.
 */
function _fbss_notifications_autocomplete_key_helper($string) {
  // @see taxonomy_autocomplete()
  if (strpos($string, ',') !== FALSE || strpos($string, '"') !== FALSE) {
    $string = '"' . str_replace('"', '""', $string) . '"';
  }
  return $string;
}
