<?php

/**
 * @file
 *   Provides implementations of contexts (stream types).
 */

/**
 * The parent context class.
 */
abstract class statuses_context {

  /**
   * The context type.
   */
  abstract function type();

  /**
   * Determines whether the current context actually applies.
   */
  function is_applicable() {
    return arg(0) == $this->type() && is_numeric(arg(1));
  }

  /**
   * Returns an object representing the recipient, determined automatically.
   */
  function find_recipient() {
    $obj = menu_get_object($this->type());
    if (empty($obj) && is_numeric(arg(1)) && function_exists($this->type() .'_load')) {
      $obj = call_user_func($this->type() .'_load', arg(1));
    }
    return $this->is_applicable() ? $obj : new stdClass();
  }

  /**
   * Returns an object representing the recipient given the recipient's ID.
   */
  function load_recipient($id) {
    return call_user_func($this->type() . '_load', $id);
  }

  /**
   * Returns an object representing a randomly selected recipient of this type.
   * Used with the Devel Generate module to send generated messages to random
   * recipients.
   */
  abstract function load_random_recipient();

  /**
   * Returns the recipient's ID given an object representing the recipient.
   * Almost every subclass will want to override this.
   */
  function recipient_id($recipient) {
    return isset($recipient->uid) ? $recipient->uid : 0;
  }

  /**
   * Returns a link to the recipient given an object representing the
   * recipient.
   */
  function recipient_link($recipient) {
    return l($this->recipient_name($recipient), $this->recipient_url($recipient));
  }

  /**
   * Returns a URL to the recipient given an object representing the recipient.
   */
  function recipient_url($recipient) {
    return $this->type() . '/' . $this->recipient_id($recipient);
  }

  /**
   * Returns the recipient's (raw, unfiltered) name given an object
   * representing the recipient.
   */
  function recipient_name($recipient) {
    return isset($recipient->title) ? $recipient->title : (isset($recipient->name) ? $recipient->name : '');
  }

  /**
   * Determines whether the $sender has permission to post a status message to
   * $recipient's stream.
   */
  function access_add($recipient, $sender) {
    return user_access('post status messages to other streams', $sender);
  }

  /**
   * Determines whether $account has permission to delete $status.
   */
  function access_delete($status, $account) {
    return user_access('delete all statuses', $account) ||
      (user_access('delete own statuses', $account) && $status->sender == $account->uid);
  }

  /**
   * Determines whether $account has permission to edit $status.
   */
  function access_edit($status, $account) {
    return user_access('edit all statuses', $account) ||
      (user_access('edit own statuses', $account) && $status->sender == $account->uid);
  }

  /**
   * Determines whether $account has permission to view $status.
   */
  function access_view($status, $account) {
    return user_access('view all statuses', $account) ||
      (user_access('update and view own stream', $account) && $status->sender == $account->uid);
  }

  /**
   * Determines whether $account has permission to view $recipient's stream.
   */
  function access_stream($recipient, $account) {
    return user_access('view all statuses', $account);
  }
}

/**
 * The user context.
 */
class statuses_user_context extends statuses_context {
  function type() {
    return 'user';
  }
  function is_applicable() {
    // The user context is the "base" context; if everything else fails, we fall back to updating the user's own status.
    return TRUE;
  }
  function find_recipient() {
    global $user;
    // On the conversation page, the active user is actually the *other* user.
    if (arg(0) == 'statuses' && arg(1) == 'conversation') {
      $args = explode(',', arg(2));
      $count = count($args);
      if ($count === 1) {
        return is_numeric($args[0]) ? user_load($args[0]) : $user;
      }
      elseif ($count == 2) {
        if ($args[0] == $user->uid) {
          return is_numeric($args[1]) ? user_load($args[1]) : $user;
        }
        elseif ($args[1] == $user->uid) {
          return is_numeric($args[0]) ? user_load($args[0]) : $user;
        }
      }
      return $user;
    }
    return arg(0) == 'user' && is_numeric(arg(1)) ? /* menu_get_object('user') */ user_load(arg(1)) : $GLOBALS['user'];
  }
  function load_random_recipient() {
    $uid = db_query_range("SELECT uid FROM {users} WHERE status = 1 ORDER BY RAND() ASC", 0, 1)->fetchField();
    return user_load($uid);
  }
  function recipient_link($recipient) {
    return theme('username', array('account' => $recipient));
  }
  function access_add($recipient, $sender) {
    return ($sender->uid == $recipient->uid && user_access('update and view own stream', $sender)) ||
      ($sender->uid != $recipient->uid && parent::access_add($recipient, $sender));
  }
  function access_delete($status, $account) {
    return parent::access_delete($status, $account) ||
      (user_access('delete status messages on own profile', $account) && $status->recipient == $account->uid);
  }
  function access_view($status, $account) {
    return user_access('view all statuses', $account) ||
      (user_access('update and view own stream', $account) && ($status->recipient == $account->uid || $status->sender == $account->uid));
  }
  function access_stream($recipient, $account) {
    return parent::access_stream($recipient, $account) || $recipient->uid == $account->uid;
  }
}

/**
 * The node context.
 */
class statuses_node_context extends statuses_context {
  function type() {
    return 'node';
  }
  function recipient_id($recipient) {
    return isset($recipient->nid) ? $recipient->nid : 0;
  }
  function load_random_recipient() {
    $nid = db_query_range("SELECT nid FROM {node} ORDER BY RAND() ASC", 0, 1)->fetchField();
    return node_load($nid);
  }
  function access_add($recipient, $sender) {
    return parent::access_add($recipient, $sender) || $sender->uid == $recipient->uid;
  }
  function access_delete($status, $account) {
    return parent::access_delete($status, $account) ||
      (user_access('delete status messages on own nodes', $account) && $this->load_recipient($status->recipient)->uid == $account->uid);
  }
  function access_view($status, $account) {
    return parent::access_view($status, $account) ||
      $this->load_recipient($status->recipient)->uid == $account->uid;
  }
  function access_stream($recipient, $account) {
    return parent::access_stream($recipient, $account) || $recipient->uid == $account->uid;
  }
}

/**
 * The group context.
 */
class statuses_og_context extends statuses_node_context {
  function type() {
    return 'og';
  }
  function is_applicable() {
    $g_ctx = og_context();
    return !(empty($g_ctx));
  }
  function find_recipient() {
    $obj = menu_get_object();
    if (empty($obj) && is_numeric(arg(1))) {
      $obj = node_load(arg(1));
    }
    return $this->is_applicable() ? $obj : new stdClass();
  }
  function load_recipient($id) {
    return node_load($id);
  }
  function recipient_url($recipient) {
    return 'node/' . $recipient->nid;
  }
  function access_add($recipient, $sender) {
    // Only group members can add content of any kind.
    $group_ids = og_get_group_ids('node', array($recipient->nid));
    if (!empty($group_ids)) {
      return parent::access_add($recipient, $sender) && og_is_member(current($group_ids), 'user', $sender);
    }
    return FALSE;
  }
  function access_delete($status, $account) {
    $group_ids = og_get_group_ids('node', array($status->recipient));
    if (!empty($group_ids)) {
      return parent::access_delete($status, $account) || in_array(OG_ADMINISTRATOR_ROLE, og_get_user_roles(current($group_ids), $account->uid, FALSE));
    }
    return parent::access_delete($status, $account);
  }
  function access_view($status, $account) {
    // Only group members can view group statuses if the group is private.
    $group_ids = og_get_group_ids('node', array($status->recipient));
    if (!empty($group_ids)) {
      return parent::access_view($status, $account) &&
      (og_is_member(current($group_ids), 'user', $account->uid) || empty($this->load_recipient($status->recipient)->og_private));
    }
    return parent::access_view($status, $account) && empty($this->load_recipient($status->recipient)->og_private);
  }
  function access_stream($recipient, $account) {
    // Only group members can view group statuses if the group is private.
    $group_ids = og_get_group_ids('node', array($recipient->nid));
    if (!empty($group_ids)) {
      return parent::access_stream($recipient, $account) &&
      (og_is_member(current($group_ids), 'user', $account->uid) || empty($recipient->og_private));
    }
    return parent::access_stream($recipient, $account) && empty($recipient->og_private);
  }
}

/**
 * The taxonomy term context.
 */
class statuses_term_context extends statuses_context {
  function type() {
    return 'term';
  }
  function is_applicable() {
    return arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2));
  }
  function find_recipient() {
    return $this->is_applicable() ? taxonomy_term_load(arg(2)) : new stdClass();
  }
  function load_recipient($id) {
    return taxonomy_term_load($id);
  }
  function load_random_recipient() {
    $tid = db_query_range("SELECT tid FROM {taxonomy_term_data} ORDER BY RAND() ASC", 0, 1)->fetchField();
    return taxonomy_term_load($tid);
  }
  function recipient_id($recipient) {
    return isset($recipient->tid) ? $recipient->tid : 0;
  }
  function recipient_url($recipient) {
    return 'taxonomy/term/' . $this->recipient_id($recipient);
  }
  function access_view($status, $account) {
    return parent::access_view($status, $account) || user_access('administer taxonomy', $account);
  }
  function access_stream($recipient, $account) {
    return parent::access_stream($recipient, $account) || user_access('administer taxonomy', $account);
  }
}

/**
 * The "site" context.
 *
 * Lets FBSS work like a site guestbook, letting users post statuses to the
 * site itself. Integrates with Domain Access to allow posting to specific
 * domains.
 */
class statuses_domain_context extends statuses_context {
  function type() {
    return 'domain';
  }
  function is_applicable() {
    return TRUE;
  }
  function find_recipient() {
    if (module_exists('fbss_domain')) {
      $domain = domain_get_domain();
      return $domain['domain_id'];
    }
    return 0;
  }
  function load_recipient($id) {
    return module_exists('fbss_domain') ? domain_load($id) : 0;
  }
  function load_random_recipient() {
    return 0;
  }
  function recipient_id($recipient) {
    return is_array($recipient) && isset($recipient['domain_id']) ? $recipient['domain_id'] : 0;
  }
  function recipient_url($recipient) {
    if (module_exists('fbss_domain')) {
      $domain = domain_get_domain();
      return url(domain_url_encode($domain['subdomain']));
    }
    return url('<front>');
  }
  function recipient_name($recipient) {
    if (module_exists('fbss_domain')) {
      $domain = domain_get_domain();
      return $domain['sitename'];
    }
    return variable_get('site_name', 'Drupal');
  }
}
