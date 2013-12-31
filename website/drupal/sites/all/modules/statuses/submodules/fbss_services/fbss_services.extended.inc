<?php

/**
 * @file
 *   Callbacks for extended Services integration with Statuses.
 */

//===============
// API CALLBACKS
//===============

// ALL

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_all_access() {
  return user_access('view all statuses');
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_all($trim_user, $include_entities) {
  $statuses = array();
  $result = db_query_range("SELECT * FROM {statuses} WHERE private = :private ORDER BY sid DESC", array(':private' => 0));
  while ($status = db_fetch_object($result)) {
    $statuses[] = fbss_services_get_rich_data($status, $include_entities, $trim_user);
  }
  return $statuses;
}

// TODO

// HOME_TIMELINE

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_home_timeline_access() {
  return user_access('update and view own stream');
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_home_timeline($since_id, $max_id, $count, $page, $include_entities) {
  global $user;
  $args = array($user->uid);
  $private = '';
  if (module_exists('fbss_privacy') && !user_access('view all private status messages')) {
    $private = "(private = 0 OR sender = %d OR (type = 'user' AND recipient = %d)) AND";
    $args[] = $user->uid;
    $args[] = $user->uid;
  }
  $og = '';
  if (module_exists('og')) {
    $og = "OR (type = 'og' AND recipient IN (SELECT nid FROM {og_uid} WHERE uid = %d AND is_active = 1))";
    $args[] = $user->uid;
  }
  $ur = '';
  if (module_exists('user_relationships_api')) {
    $ur = "
      OR (type = 'user' AND sender = recipient AND (
        sender IN (SELECT requester_id uid FROM {user_relationships} WHERE requestee_id = %d AND approved = 1) OR
        sender IN (SELECT requestee_id uid FROM {user_relationships} WHERE requester_id = %d AND approved = 1)
      ))
    ";
    $args[] = $user->uid;
    $args[] = $user->uid;
  }
  $ff = '';
  if (module_exists('flag_friend')) {
    $ff = ""; // TODO: figure out the right query to integrate Flag Friend here
  }
  $flag = '';
  if (module_exists('flag') && $follow = flag_get_flag('follow')) {
    $flag = "OR (type = 'user' AND sender = recipient AND sender IN (SELECT content_id FROM {flag_content} WHERE uid = %d AND fid = %d))";
    $args[] = $user->uid;
    $args[] = $follow->fid;
  }
  $tags = '';
  if (module_exists('statuses_tags')) {
    $tags = "OR sid IN (SELECT sid FROM {statuses_tags} WHERE type = 'user' AND rid = %d)";
    $args[] = $user->uid;
  }
  $args[] = $since_id;
  $max = '';
  if ($max_id != 0) {
    $max = "AND sid <= %d";
    $args[] = $max_id;
  }
  $statuses = array();
  // TODO Please convert this statement to the D7 database API syntax.
  $result = db_query_range("
    SELECT * FROM {statuses}
    WHERE $private ( /* message is not private OR involves you OR you have permission to see all private messages */
      (type = 'user' AND recipient = %d) /* messages to you */
      $og /* messages to your groups */
      $ur $ff $flag /* messages by people you follow or your friends */
      $tags /* messages mentioning you. TODO: only add if FBSST is installed. */
    ) AND sid > %d $max AND message <> ''
    ORDER BY sid DESC
  ", $args);
  while ($status = db_fetch_object($result)) {
    $statuses[] = fbss_services_get_rich_data($status, $include_entities);
  }
  return $statuses;
}

// MENTIONS

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_mentions_access() {
  return module_exists('statuses_tags') && user_access('view all statuses');
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_mentions($since_id, $max_id, $count, $page, $trim_user, $include_entities) {
  global $user;
  $args = array($user->uid, $since_id);
  $max = '';
  if ($max_id != 0) {
    $max = "AND f.sid <= %d";
    $args[] = $max_id;
  }
  $private = '';
  if (module_exists('fbss_privacy') && !user_access('view all private status messages')) {
    $private = "(private = 0 OR sender = %d OR (f.type = 'user' AND recipient = %d)) AND";
    $args[] = $user->uid;
    $args[] = $user->uid;
  }
  $og = '';
  if (module_exists('og')) {
    $og = "AND (f.type <> 'og' OR (recipient IN (SELECT u.nid FROM {og_uid} u LEFT JOIN {og} o ON u.nid = o.nid WHERE (uid = %d AND is_active = 1) OR og_private = 0)))";
    $args[] = $user->uid;
  }
  $statuses = array();
  // TODO Please convert this statement to the D7 database API syntax.
  $result = db_query_range("
    SELECT f.* FROM {statuses_tags} t
    LEFT JOIN {statuses} f ON t.sid = f.sid
    WHERE
      t.type = 'user' AND rid = %d
      AND f.sid > %d
      $max
      AND message <> ''
      $private /* message is not private OR involves you OR you have permission to see all private messages */
      $og /* messages to your groups or public groups you don't belong to */
    ORDER BY f.sid DESC
  ", $args);
  while ($status = db_fetch_object($result)) {
    $statuses[] = fbss_services_get_rich_data($status, $include_entities, $trim_user);
  }
  return $statuses;
}

// USER_TIMELINE

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_user_timeline_access($args) {
  list($user_id, $screen_name) = $args;
  global $user;
  // Requested user is definitely the current user.
  if ($user->uid == $user_id && $user->name == $screen_name) {
    return user_access('view all statuses');
  }
  // Probably only one parameter was set.
  if ($user->uid == $user_id) {
    $account = array_shift(user_load_multiple(array(), array('name' => $screen_name)));
    return !empty($account->uid) && statuses_user_access('view_stream', $account, 'user');
  }
  elseif ($user->name == $screen_name) {
    $account = array_shift(user_load_multiple(array(), array('id' => $user_id)));
    return !empty($account->uid) && statuses_user_access('view_stream', $account, 'user');
  }

  // The ID and name are both not the current user. Use the ID if possible even if the name doesn't match.
  $id_account = _statuses_user_load($user_id);
  if (!empty($id_account->uid)) {
    return statuses_user_access('view_stream', $id_account, 'user');
  }
  $account = array_shift(user_load_multiple(array(), array('name' => $screen_name)));
  if (!empty($account->uid)) {
    return statuses_user_access('view_stream', $account, 'user');
  }
  // Neither the ID nor the name validated as existing users.
  return FALSE;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_user_timeline($user_id, $screen_name, $since_id, $max_id, $count, $page, $trim_user, $include_entities) {
  $account = _statuses_user_load($user_id);
  if ($account->name != $screen_name && $account->uid == $GLOBALS['user']->uid) {
    $account = array_shift(user_load_multiple(array(), array('name' => $screen_name)));
  }
  $args = array($account->uid, $since_id);
  $max = '';
  if ($max_id != 0) {
    $max = "AND sid <= %d";
    $args[] = $max_id;
  }
  // TODO Please convert this statement to the D7 database API syntax.
  $result = db_query_range("
    SELECT *
    FROM {statuses}
    WHERE type = 'user'
      AND sender = recipient
      AND sender = %d
      AND message <> ''
      AND sid > %d
      $max
    ORDER BY sid DESC
  ", $args);
  $statuses = array();
  while ($status = db_fetch_object($result)) {
    $statuses[] = fbss_services_get_rich_data($status, $include_entities, $trim_user);
  }
  return $statuses;
}

//==================
// HELPER FUNCTIONS
//==================

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_get_rich_data($status, $include_entities = TRUE, $trim_user = FALSE) {
  $object = array(
    'created_at' => fbss_services_get_date($status->created),
    'id' => $status->sid,
    'text' => check_plain($status->message),
    'source' => 'web', // We don't support tracking alternate sources.
    'truncated' => false, // Because we don't support tracking alternate sources, statuses can't be submitted if they are too long.
    'in_reply_to_status_id' => NULL, // We don't support tracking this.
    'in_reply_to_user_id' => $status->sender == $status->recipient ? NULL : $status->recipient,
    'favorited' => fbss_services_get_flagged($status->sid, 'like'),
    'in-reply_to_screen_name' => $status->sender == $status->recipient ? NULL : check_plain(_statuses_user_load($status->recipient)->name),
    'geo' => NULL, // We don't support tracking this.
    'place' => NULL, // We don't support tracking this.
    'coordinates' => NULL, // We don't support tracking this.
    'contributors' => NULL, // We don't support tracking this.
    'annotations' => NULL, // We don't support tracking this.
  );
  if ($trim_user == 't' || $trim_user == 'true' || $trim_user == '1') {
    $object['user'] = array(
      'id' => $status->sender,
    );
  }
  else {
    $account = _statuses_user_load($status->sender);
    $object['user'] = array(
      'id' => $account->uid,
      'name' => isset($account->realname) ? check_plain($account->realname) : check_plain($account->name),
      'screen_name' => check_plain($account->name),
      'location' => NULL, // We don't support tracking this.
      'description' => fbss_services_find_attrs($account, array('about', 'description')),
      'profile_image_url' => fbss_services_get_image_url($account),
      'url' => fbss_services_find_attrs($account, array('website', 'url', 'link', 'twitter')),
      'protected' => FALSE, // We don't support tracking this.
      'followers_count' => fbss_services_count_flags($account->uid, 'follow'),
      'profile_background_color' => NULL, // We don't support tracking this.
      'profile_text_color' => NULL, // We don't support tracking this.
      'profile_link_color' => NULL, // We don't support tracking this.
      'profile_sidebar_fill_color' => NULL, // We don't support tracking this.
      'profile_sidebar_border_color' => NULL, // We don't support tracking this.
      'friends_count' => fbss_services_get_friends_count($account->uid),
      'created_at' => fbss_services_get_date($account->created),
      'favourites_count' => fbss_services_count_user_flags($account->uid, 'like'),
      'utc_offset' => isset($account->timezone) ? $account->timezone : variable_get('date_default_timezone', 0),
      'time_zone' => date('e', strtotime('Mon, Jun 13 2010 13:08:00 ' . isset($account->timezone) ? $account->timezone : variable_get('date_default_timezone', 0))),
      'profile_background_image_url' => NULL, // We don't support tracking this.
      'profile_background_tile' => NULL, // We don't support tracking this.
      'profile_use_background_image' => 'false',
      'notifications' => 'false', // This is meaningless in this context.
      'geo_enabled' => 'false', // We don't support tracking this.
      'verified' => 'false', // We don't support tracking this.
      'following' => fbss_services_get_flagged($account->uid, 'follow'),
      'statuses_count' => statuses_has_status($account->uid, 'user', $account->uid),
      'lang' => user_preferred_language($account)->language,
      'contributors_enabled' => 'false', // We don't support tracking this.
      'follow_request_sent' => NULL, // Appears to be cruft?
    );
  }
  return $object;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_get_date($timestamp) {
  $request_format = drupal_substr($_GET['q'], strrpos($_GET['q'], '.') + 1);
  $date_format = variable_get('date_format_medium', 'D, m/d/Y - H:i');
  switch ($request_format) {
    case 'xml':
    case 'json':
      $date_format = 'D M j H:i:s O Y'; // Thu Jul 15 23:26:04 +0000 2010
    case 'rss':
      $date_format = 'D, j M Y H:i:s O'; // Thu, 15 Jul 2010 23:26:04 +0000
  }
  return date($date_format, $timestamp);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_get_flagged($id, $flag_name) {
  if (module_exists('flag')) {
    if ($flag = flag_get_flag($flag_name)) {
      return $flag->is_flagged($id) ? 'true' : 'false';
    }
  }
  return 'false';
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_find_attrs($account, $attrs = array()) {
  foreach ($attrs as $attr) {
    if ($value = fbss_services_find_attr($account, $attr)) {
      return $value;
    }
  }
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_find_attr($account, $attr) {
  static $loaded = array();
  if (module_exists('profile')) {
    if (empty($loaded[$account->uid])) {
      profile_load_profile($account);
      $loaded[$account->uid] = TRUE;
    }
    foreach ($account as $name => $property) {
      if (strpos($name, 'profile_' . $attr) === 0) {
        if (is_string($property)) {
          return check_plain($property);
        }
        elseif (is_array($property)) {
          return check_plain($property[0]);
        }
      }
    }
  }
  return NULL;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_get_image_url($account) {
  if (variable_get('user_pictures', 0)) {
    if (!empty($account->picture) && file_exists($account->picture)) {
      return file_create_url($account->picture);
    }
    elseif (variable_get('user_picture_default', '')) {
      return file_create_url(variable_get('user_picture_default', ''));
    }
  }
  return NULL;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_count_flags($id, $flag_name) {
  if (module_exists('flag')) {
    if ($flag = flag_get_flag($flag_name)) {
      return $flag->get_count($id);
    }
  }
  return 0;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_get_friends_count($uid) {
  if (module_exists('flag_friend')) {
    return fbss_services_count_flags($account->uid, 'friend');
  }
  elseif (module_exists('user_relationships_api')) {
    return user_relationships_load(array('user' => $account->uid), array('count' => TRUE));
  }
  return 0;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function fbss_services_count_user_flags($uid, $flag_name) {
  if (module_exists('flag') && $flag = flag_get_flag($flag_name)) {
    return $flag->get_user_count($uid);
  }
  return 0;
}
