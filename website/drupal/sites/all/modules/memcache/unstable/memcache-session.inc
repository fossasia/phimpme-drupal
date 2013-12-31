<?php

/**
 * @file
 * User session handling functions.
 *
 * Note session support is not fully ported to Drupal 7 and is not recommended
 * for production sites.
 *
 * An alternative to includes/session.inc.
 */

require_once dirname(__FILE__) . '/dmemcache.inc';

/**
 * Implement hook_user() using a required module's namespace since memcache is 
 * not a module and thus can't implement hooks directly.
 */
function filter_user($op, &$edit, &$account, $category = NULL) {
  if ($op == 'update') {
    // Invalidate cached user object.
    cache_clear_all($account->uid, 'users');
  }
}

function _drupal_session_open() {
  return TRUE;
}

function _drupal_session_close() {
  return TRUE;
}

function _drupal_session_read($key) {
  global $user;

  // Write and Close handlers are called after destructing objects since PHP 5.0.5
  // Thus destructors can use sessions but session handler can't use objects.
  // So we are moving session closure before destructing objects.
  register_shutdown_function('session_write_close');

  // Handle the case of first time visitors and clients that don't store
  // cookies (eg. web crawlers).
  if (!isset($_COOKIE[session_name()])) {
    $user = drupal_anonymous_user();
    return '';
  }

  // Otherwise, if the session is still active, we have a record of the
  // client's session in memcache.
  $session = dmemcache_get($key, 'session');

  $user = _memcache_session_user_load($session);

  // Record whether this session contains data so that in sess_write() it can
  // be determined whether to skip a write.
  $user->session_data_present_at_load = !empty($session->session);

  return $user->session;
}

/**
 * Write a session to session storage.
 *
 * We have the following cases to handle.
 * 1. Anonymous user
 *   1a. Without session data.
 *   1b. With session data.
 *   1c. Session saving has been turned off programatically
 *       (see drupal_save_session()).
 *   1d. Without session data but had session data at the beginning of the request
 *       (thus a write must be made to clear stored session data).
 * 2. Authenticated user.
 *   2a. Without session data.
 *   2b. With session data.
 *   2c. Session saving has been turned off programatically
 *       (see drupal_save_session()).
 *
 * @param $key
 *   The session ID.
 * @param $value
 *   Any data to store in the session.
 * @return
 *   TRUE.
 */
function _drupal_session_write($key, $value) {
  global $user;

  if (!drupal_save_session()) {
    return TRUE;
  }

  // Prepare the information to be saved.
  $session = new stdClass;
  $session->sid = $key;
  $session->uid = $user->uid;
  $session->cache = isset($user->cache) ? $user->cache : '';
  $session->hostname = ip_address();
  $session->session = $value;
  $session->timestamp = REQUEST_TIME;

  // Be sure that we have the latest user object.  If user_save() has been
  // called, we need to refresh the object from the database.
  $user = _memcache_session_user_load($session);

  // If this is an authenticated user, or there is something to save in the
  // session, or this is an anonymous user who currently has nothing in the
  // session but did have something in session storage, write it to memcache.
  // If $user->session_data_present_at_load is not set, the current user
  // was created during this request and it's safest to do a write.
  // Cases 1b, 1d, 2a, and 2b are covered here.
  if ($user->uid || !empty($value) || empty($value) && (!isset($user->session_data_present_at_load) || $user->session_data_present_at_load)) {
    dmemcache_set($key, $session, ini_get('session.gc_maxlifetime'), 'session');
    if ($user->uid && $session->timestamp - $user->access > variable_get('session_write_interval', 360)) {
      db_update('users')
        ->fields(array('access' => $user->uid))
        ->condition(array('uid' => $user->uid));
      // Update the user access time so that the dmemcache_set() call
      // caches the updated time.
      $user->access = $session->timestamp;
    }
    // Data stored in session is stored in session memcache; no need
    // to duplicate it in users memcache.
    unset($user->session);
    unset($user->session_data_present_at_load);
    // Store the session id so we can locate the session with the user id.
    $user->sid = $key;

    dmemcache_set($user->uid, $user, ini_get('session.gc_maxlifetime'), 'users');
  }

  return TRUE;
}


/**
 * Called by PHP session handling with the PHP session ID to end a user's session.
 *
 * @param  string $sid
 *   the session id
 */
function _drupal_session_destroy($sid) {
  dmemcache_delete($sid, 'session');
  // Reset $_SESSION and $user to prevent a new session from being started
  // in drupal_session_commit().
  $_SESSION = array();
  $user = drupal_anonymous_user();

  // Unset the session cookies.
  _drupal_session_delete_cookie(session_name());
}

function _drupal_session_garbage_collection($lifetime) {
  // Automatic with memcached.
  // Be sure to adjust 'php_value session.gc_maxlifetime' to a large enough
  // value. For example, if you want user sessions to stay in your database
  // for three weeks before deleting them, you need to set gc_maxlifetime
  // to '1814400'. At that value, only after a user doesn't log in after
  // three weeks (1814400 seconds) will his/her session be removed.
  return TRUE;
}

/**
 * Initialize the session handler, starting a session if needed.
 */
function drupal_session_initialize() {
  global $user;

  session_set_save_handler('_drupal_session_open', '_drupal_session_close', '_drupal_session_read', '_drupal_session_write', '_drupal_session_destroy', '_drupal_session_garbage_collection');

  if (isset($_COOKIE[session_name()])) {
    // If a session cookie exists, initialize the session. Otherwise the
    // session is only started on demand in drupal_session_commit(), making
    // anonymous users not use a session cookie unless something is stored in
    // $_SESSION. This allows HTTP proxies to cache anonymous pageviews.
    drupal_session_start();
    if (!empty($user->uid) || !empty($_SESSION)) {
      drupal_page_is_cacheable(FALSE);
    }
  }
  else {
    // Set a session identifier for this request. This is necessary because
    // we lazyly start sessions at the end of this request, and some
    // processes (like drupal_get_token()) needs to know the future
    // session ID in advance.
    $user = drupal_anonymous_user();
    session_id(md5(uniqid('', TRUE)));
  }
}


/**
 * Counts how many users have sessions. Can count either anonymous sessions, authenticated sessions, or both.
 * Would be insane slow with memcached as we would need to retrieve at least the stats of all object.
 * Not implemented.
 */
function drupal_session_count($timestamp = 0, $anonymous = true) {
}

/**
 * Forcefully start a session, preserving already set session data.
 *
 * @ingroup php_wrappers
 */
function drupal_session_start() {
  if (!drupal_session_started()) {
    // Save current session data before starting it, as PHP will destroy it.
    $session_data = isset($_SESSION) ? $_SESSION : NULL;

    session_start();
    drupal_session_started(TRUE);

    // Restore session data.
    if (!empty($session_data)) {
      $_SESSION += $session_data;
    }
  }
}

/**
 * Commit the current session, if necessary.
 *
 * If an anonymous user already have an empty session, destroy it.
 */
function drupal_session_commit() {
  global $user;

  if (!drupal_save_session()) {
    // We don't have anything to do if we are not allowed to save the session.
    return;
  }

  if (empty($user->uid) && empty($_SESSION)) {
    // There is no session data to store, destroy the session if it was
    // previously started.
    if (drupal_session_started()) {
      session_destroy();
    }
  }
  else {
    // There is session data to store. Start the session if it is not already
    // started.
    if (!drupal_session_started()) {
      drupal_session_start();
    }
    // Write the session data.
    session_write_close();
  }
}

/**
 * Return whether a session has been started.
 */
function drupal_session_started($set = NULL) {
  static $session_started = FALSE;
  if (isset($set)) {
    $session_started = $set;
  }
  return $session_started && session_id();
}

/**
 * Called when an anonymous user becomes authenticated or vice-versa.
 *
 * @ingroup php_wrappers
 */
function drupal_session_regenerate() {
  global $user;
  if (drupal_session_started()) {
    $old_session_id = session_id();
    session_regenerate_id();
    $new_session_id = session_id();
  }
  else {
    // Start the session when it doesn't exist yet.
    // Preserve the logged in user, as it will be reset to anonymous
    // by _drupal_session_read.
    $account = $user;
    drupal_session_start();
    $user = $account;
  }

  if (isset($old_session_id)) {
    $info = dmemcache_get($old_session_id, 'session');
    $info->sid = $new_session_id;
    dmemcache_set($new_session_id, $info, ini_get('session.gc_maxlifetime'), 'session');
    // Clear the old data from the cache.
    dmemcache_delete($old_session_id, 'session');
  }
}


/**
 * End a specific user's session.
 */
function drupal_session_destroy_uid($uid) {
  $user = dmemcache_get($uid, 'users');
  if (is_object($user) && isset($user->sid)) {
    dmemcache_delete($user->sid, 'session');
  }
  dmemcache_delete($uid, 'users');
}


/**
 * Determine whether to save session data of the current request.
 *
 * This function allows the caller to temporarily disable writing of session data,
 * should the request end while performing potentially dangerous operations, such as
 * manipulating the global $user object.  See http://drupal.org/node/218104 for usage
 *
 * @param $status
 *   Disables writing of session data when FALSE, (re-)enables writing when TRUE.
 * @return
 *   FALSE if writing session data has been disabled. Otherwise, TRUE.
 */
function drupal_save_session($status = NULL) {
  $save_session = &drupal_static(__FUNCTION__, TRUE);
  if (isset($status)) {
    $save_session = $status;
  }
  return ($save_session);
}

/**
 * Create the user object.
 *
 * @param $session
 *   The session object (see sess_write() for the structure).
 * @return $user
 *   The user object.
 */
function _memcache_session_user_load($session) {
  // We found the client's session record and they are an authenticated user.
  if ($session && $session->uid != 0) {
    $user = dmemcache_get($session->uid, 'users');
    // If the 'users' memcache bin is unavailable, $user will be NULL.
    // If the cached user was not found in the 'users' memcache bin, $user will
    // be FALSE.
    // In either of these cases, the user must be retrieved from the database.
    if (!$user && isset($session->uid) && $session->uid != 0) {
      $user = db_query('SELECT u.* FROM {users} u WHERE u.uid = :uid', array('uid' => $session->uid))->fetchObject();

      if (!$user->status) {
        $user = drupal_anonymous_user($session->session);
      }
      else {
        $user = drupal_unpack($user);
        // Normally we would join the session and user tables. But we already
        // have the session information. So add that in.
        $user->sid = $session->sid;
        $user->cache = $session->cache;
        $user->hostname = $session->hostname;
        $user->timestamp = $session->timestamp;
        $user->session = empty($session->session) ? '' : $session->session;

        // Add roles element to $user
        $user->roles = array();
        $user->roles[DRUPAL_AUTHENTICATED_RID] = 'authenticated user';
        $result = db_query("SELECT r.rid, r.name FROM {role} r INNER JOIN {users_roles} ur ON ur.rid = r.rid WHERE ur.uid = :uid", array(':uid' => $user->uid));
        while ($role = $result->fetchObject()) {
          $user->roles[$role->rid] = $role->name;
        }
      }
    }
    else if ($user->uid) {
      // Got a user object from 'users' memcache bin. Mark it in case modules
      // want to know that this user was created from memcache.
      $user->from_cache = TRUE;
      $user->session = empty($session->session) ? '' : $session->session;
    }
    else {
      // We will only get here when the session has a nonzero uid, a user object
      // was successfully retrieved from the 'users' bin, and that user
      // object's uid is 0. Not sure why this would ever happen. Leaving former
      // comment in:
      // This is a rare case that we have a session cached, but no session user object cached.
      // This usually only happens if you kill memcached and restart it.
      $user = drupal_anonymous_user($session->session);
    }
  }
  // We didn't find the client's record (session has expired), or they are an
  // anonymous user.
  else  {
    $session = isset($session->session) ? $session->session : '';
    $user = drupal_anonymous_user($session);
  }

  return $user;
}

/**
 * Deletes the session cookie.
 *
 * @param $name
 *   Name of session cookie to delete.
 * @param $force_insecure
 *   Fornce cookie to be insecure.
 */
function _drupal_session_delete_cookie($name, $force_insecure = FALSE) {
  if (isset($_COOKIE[$name])) {
    $params = session_get_cookie_params();
    setcookie($name, '', REQUEST_TIME - 3600, $params['path'], $params['domain'], !$force_insecure && $params['secure'], $params['httponly']);
    unset($_COOKIE[$name]);
  }
}

