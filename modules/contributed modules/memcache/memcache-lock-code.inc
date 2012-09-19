<?php

/**
 * @file
 * A memcache based implementation of a locking mechanism.
 * See includes/lock.inc for documenation
 *
 * ATTENTION: Don't include this file directly - use the memcache-lock.inc to
 * have a failover solution.
 */

/**
 * Initialize the locking system.
 */
function lock_initialize() {
  global $locks;

  $locks = array();
}

/**
 * Acquire (or renew) a lock, but do not block if it fails.
 *
 * @param $name
 *   The name of the lock.
 * @param $timeout
 *   A number of seconds (int) before the lock expires (minimum of 1).
 * @return
 *   TRUE if the lock was acquired, FALSE if it failed.
 */
function lock_acquire($name, $timeout = 30) {
  global $locks;

  // Ensure that the timeout is at least 1 sec. This is a limitation
  // imposed by memcached.
  $timeout = (int) max($timeout, 1);

  if (dmemcache_add($name, _lock_id(), $timeout, 'semaphore')) {
    $locks[$name] = _lock_id();
  }
  elseif ($result = dmemcache_get($name, 'semaphore') && isset($locks[$name]) && $locks[$name] == _lock_id()) {
    // Only renew the lock if we already set it and it has not expired.
    dmemcache_set($name, _lock_id(), $timeout, 'semaphore');
  }
  else {
    // Failed to acquire the lock.  Unset the key from the $locks array even if
    // not set, PHP 5+ allows this without error or warning.
    unset($locks[$name]);
  }

  return isset($locks[$name]);
}

/**
 * Check if lock acquired by a different process may be available.
 *
 * If an existing lock has expired, it is removed.
 *
 * @param $name
 *   The name of the lock.
 * @return
 *   TRUE if there is no lock or it was removed, FALSE otherwise.
 */
function lock_may_be_available($name) {
  return !dmemcache_get($name, 'semaphore');
}

/**
 * Wait for a lock to be available.
 *
 * This function may be called in a request that fails to acquire a desired
 * lock. This will block further execution until the lock is available or the
 * specified delay in seconds is reached. This should not be used with locks
 * that are acquired very frequently, since the lock is likely to be acquired
 * again by a different request while waiting.
 *
 * @param $name
 *   The name of the lock.
 * @param $delay
 *   The maximum number of seconds to wait, as an integer.
 * @return
 *   TRUE if the lock holds, FALSE if it is available.
 */
function lock_wait($name, $delay = 30) {
  // Pause the process for short periods between calling
  // lock_may_be_available(). This prevents hitting the database with constant
  // database queries while waiting, which could lead to performance issues.
  // However, if the wait period is too long, there is the potential for a
  // large number of processes to be blocked waiting for a lock, especially
  // if the item being rebuilt is commonly requested. To address both of these
  // concerns, begin waiting for 25ms, then add 25ms to the wait period each
  // time until it reaches 500ms. After this point polling will continue every
  // 500ms until $delay is reached.

  // $delay is passed in seconds, but we will be using usleep(), which takes
  // microseconds as a parameter. Multiply it by 1 million so that all
  // further numbers are equivalent.
  $delay = (int) $delay * 1000000;

  // Begin sleeping at 25ms.
  $sleep = 25000;
  while ($delay > 0) {
    // This function should only be called by a request that failed to get a
    // lock, so we sleep first to give the parallel request a chance to finish
    // and release the lock.
    usleep($sleep);
    // After each sleep, increase the value of $sleep until it reaches
    // 500ms, to reduce the potential for a lock stampede.
    $delay = $delay - $sleep;
    $sleep = min(500000, $sleep + 25000, $delay);
    if (!dmemcache_get($name, 'semaphore')) {
      // No longer need to wait.
      return FALSE;
    }
  }
  // The caller must still wait longer to get the lock.
  return TRUE;
}

/**
 * Release a lock previously acquired by lock_acquire().
 *
 * This will release the named lock if it is still held by the current request.
 *
 * @param $name
 *   The name of the lock.
 */
function lock_release($name) {
  global $locks;

  dmemcache_delete($name, 'semaphore');
  // We unset unconditionally since caller assumes lock is released anyway.
  unset($locks[$name]);
}

/**
 * Generate a unique identifier for locks generated during this request.
 */
function _lock_id() {
  static $lock_id;
  if (!isset($lock_id)) {
    $lock_id = uniqid(mt_rand(), TRUE);
    // We only register a shutdown function if a lock is used.
    register_shutdown_function('lock_release_all', $lock_id);
  }
  return $lock_id;
}

/**
 * Release all locks acquired by this request.
 */
function lock_release_all($lock_id) {
  global $locks;
  foreach ($locks as $name => $id) {
    $value = dmemcache_get($name, 'semaphore');

    if ($value == $id) {
      dmemcache_delete($name, 'semaphore');
    }
  }
}
