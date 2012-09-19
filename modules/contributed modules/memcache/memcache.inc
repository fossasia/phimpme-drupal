<?php

require_once dirname(__FILE__) . '/dmemcache.inc';

/**
 * Defines the period after which wildcard clears are not considered valid.
 */
define('MEMCACHE_WILDCARD_INVALIDATE', 86400 * 28);

define('MEMCACHE_CONTENT_CLEAR', 'MEMCACHE_CONTENT_CLEAR');

/** Implementation of cache.inc with memcache logic included **/

class MemCacheDrupal implements DrupalCacheInterface {
  function __construct($bin) {
    $this->memcache = dmemcache_object($bin);
    $this->bin = $bin;

    $this->reloadVariables();
  }

  function get($cid) {
    $cache = dmemcache_get($cid, $this->bin, $this->memcache);
    return $this->valid($cid, $cache) ? $cache : FALSE;
  }

  function getMultiple(&$cids) {
    $results = dmemcache_get_multi($cids, $this->bin, $this->memcache);
    foreach ($results as $cid => $result) {
      if (!$this->valid($result->cid, $result)) {
        // This object has expired, so don't return it.
        unset($results[$cid]);
      }
    }
    // Remove items from the referenced $cids array that we are returning,
    // per the comment in cache_get_multiple() in includes/cache.inc.
    $cids = array_diff($cids, array_keys($results));
    return $results;
  }

  protected function valid($cid, $cache) {
    if ($cache) {
      $cache_tables = isset($_SESSION['cache_flush']) ? $_SESSION['cache_flush'] : NULL;
      // Items that have expired are invalid.
      if (isset($cache->expire) && $cache->expire !== CACHE_PERMANENT && $cache->expire <= $_SERVER['REQUEST_TIME']) {
        // If the memcache_stampede_protection variable is set, allow one process
        // to rebuild the cache entry while serving expired content to the
        // rest. Note that core happily returns expired cache items as valid and
        // relies on cron to expire them, but this is mostly reliant on its
        // use of CACHE_TEMPORARY which does not map well to memcache.
        // @see http://drupal.org/node/534092
        if (variable_get('memcache_stampede_protection', FALSE)) {
          // The process that acquires the lock will get a cache miss, all
          // others will get a cache hit.
          if (lock_acquire("memcache_$cid:$this->bin", variable_get('memcache_stampede_semaphore', 15))) {
            $cache = FALSE;
          }
        }
        else {
          $cache = FALSE;
        }
      }
      // Items created before the last full wildcard flush against this bin are
      // invalid.
      elseif ($cache->created <= $this->cache_flush) {
        $cache = FALSE;
      }
      // Items created before the last content flush on this bin i.e.
      // cache_clear_all() are invalid.
      elseif ($cache->expire != CACHE_PERMANENT && $cache->created + $this->cache_lifetime <= $this->cache_content_flush) {
        $cache = FALSE;
      }
      // Items cached before the cache was last flushed by the current user are
      // invalid.
      elseif ($cache->expire != CACHE_PERMANENT && is_array($cache_tables) && isset($cache_tables[$this->bin]) && $cache_tables[$this->bin] >= $cache->created) {
        // Cache item expired, return FALSE.
        $cache = FALSE;
      }
      // Finally, check for wildcard clears against this cid.
      else {
        if (!$this->wildcard_valid($cid, $cache)) {
          $cache = FALSE;
        }
      }
    }

    // On cache misses, attempt to avoid stampedes when the
    // memcache_stampede_protection variable is enabled.
    if (!$cache) {
      if (variable_get('memcache_stampede_protection', FALSE) && !lock_acquire("memcache_$cid:$this->bin", variable_get('memcache_stampede_semaphore', 15))) {
        // Prevent any single request from waiting more than three times due to
        // stampede protection. By default this is a maximum total wait of 15
        // seconds. This accounts for two possibilities - a cache and lock miss
        // more than once for the same item. Or a cache and lock miss for
        // different items during the same request.
        // @todo: it would be better to base this on time waited rather than
        // number of waits, but the lock API does not currently provide this
        // information. Currently the limit will kick in for three waits of 25ms
        // or three waits of 5000ms.
        static $lock_count = 0;
        $lock_count++;
        if ($lock_count <= variable_get('memcache_stampede_wait_limit', 3)) {
          // The memcache_stampede_semaphore variable was used in previous releases
          // of memcache, but the max_wait variable was not, so by default divide
          // the semaphore value by 3 (5 seconds).
          lock_wait("memcache_$cid:$this->bin", variable_get('memcache_stampede_wait_time', 5));
          $cache = $this->get($cid);
        }
      }
    }

    return (bool) $cache;
  }

  function set($cid, $data, $expire = CACHE_PERMANENT, array $headers = NULL) {
    $created = time();

    // Create new cache object.
    $cache = new stdClass;
    $cache->cid = $cid;
    $cache->data = is_object($data) ? clone $data : $data;
    $cache->created = $created;
    $cache->headers = $headers;
    // Record the previous number of wildcard flushes affecting our cid.
    $cache->flushes = $this->wildcard_flushes($cid);
    if ($expire == CACHE_TEMPORARY) {
      // Convert CACHE_TEMPORARY (-1) into something that will live in memcache
      // until the next flush.
      $cache->expire = REQUEST_TIME + 2591999;
    }
    // Expire time is in seconds if less than 30 days, otherwise is a timestamp.
    else if ($expire != CACHE_PERMANENT && $expire < 2592000) {
      // Expire is expressed in seconds, convert to the proper future timestamp
      // as expected in dmemcache_get().
      $cache->expire = REQUEST_TIME + $expire;
    }
    else {
      $cache->expire = $expire;
    }

    // We manually track the expire time in $cache->expire.  When the object
    // expires, we only allow one request to rebuild it to avoid cache
    // stampedes. Other requests for the expired object while it is still being
    // rebuilt get the expired object.
    dmemcache_set($cid, $cache, 0, $this->bin, $this->memcache);
  }

  function clear($cid = NULL, $wildcard = FALSE) {
    if ($this->memcache === FALSE) {
      // No memcache connection.
      return;
    }

    // It is not possible to detect a cache_clear_all() call other than looking
    // at the backtrace unless http://drupal.org/node/81461 is added.
    $backtrace = debug_backtrace();
    if ($cid == MEMCACHE_CONTENT_CLEAR || (isset($backtrace[2]) && $backtrace[2]['function'] == 'cache_clear_all' && empty($backtrace[2]['args']))) {
      // Update the timestamp of the last global flushing of this bin.  When
      // retrieving data from this bin, we will compare the cache creation
      // time minus the cache_flush time to the cache_lifetime to determine
      // whether or not the cached item is still valid.
      $this->cache_content_flush = time();
      $this->variable_set('cache_content_flush_' . $this->bin, $this->cache_content_flush);
      if (variable_get('cache_lifetime', 0)) {
        // We store the time in the current user's session. We then simulate
        // that the cache was flushed for this user by not returning cached
        // data to this user that was cached before the timestamp.
        if (isset($_SESSION['cache_flush']) && is_array($_SESSION['cache_flush'])) {
          $cache_bins = $_SESSION['cache_flush'];
        }
        else {
          $cache_bins = array();
        }
        // Use time() rather than request time here for correctness.
        $cache_tables[$this->bin] = $this->cache_content_flush;
        $_SESSION['cache_flush'] = $cache_tables;
      }
    }
    if (empty($cid) || $wildcard === TRUE) {
      // system_cron() flushes all cache bins returned by hook_flush_caches()
      // with cache_clear_all(NULL, $bin); This is for garbage collection with
      // the database cache, but serves no purpose with memcache. So return
      // early here.
      if (!isset($cid)) {
        return;
      }
      elseif ($cid == '*') {
        $cid = '';
      }
      if (empty($cid)) {
        // Update the timestamp of the last global flushing of this bin.  When
        // retrieving data from this bin, we will compare the cache creation
        // time minus the cache_flush time to the cache_lifetime to determine
        // whether or not the cached item is still valid.
        $this->cache_flush = time();
        $this->variable_set("cache_flush_$this->bin", $this->cache_flush);
        $this->flushed = min($this->cache_flush, time() - $this->cache_lifetime);

        if ($this->cache_lifetime) {
          // We store the time in the current user's session which is saved into
          // the sessions table by sess_write().  We then simulate that the cache
          // was flushed for this user by not returning cached data to this user
          // that was cached before the timestamp.
          if (isset($_SESSION['cache_flush']) && is_array($_SESSION['cache_flush'])) {
            $cache_bins = $_SESSION['cache_flush'];
          }
          else {
            $cache_bins = array();
          }
          $cache_bins[$this->bin] = $this->cache_flush;
          $_SESSION['cache_flush'] = $cache_bins;
        }
      }
      else {
        // Register a wildcard flush for current cid
        $this->wildcards($cid, TRUE);
      }
    }
    else {
      $cids = is_array($cid) ? $cid : array($cid);
      foreach ($cids as $cid) {
        dmemcache_delete($cid, $this->bin, $this->memcache);
      }
    }
  }

  /**
   * Sum of all matching wildcards.  Checking any single cache item's flush
   * value against this single-value sum tells us whether or not a new wildcard
   * flush has affected the cached item.
   */
  protected function wildcard_flushes($cid) {
    return array_sum($this->wildcards($cid));
  }

  /**
   * Utilize multiget to retrieve all possible wildcard matches, storing
   * statically so multiple cache requests for the same item on the same page
   * load doesn't add overhead.
   */
  protected function wildcards($cid, $flush = FALSE) {
    static $wildcards = array();
    $matching = array();

    $length = strlen($cid);

    if (isset($this->wildcard_flushes[$this->bin]) && is_array($this->wildcard_flushes[$this->bin])) {
      // Wildcard flushes per table are keyed by a substring equal to the
      // shortest wildcard clear on the table so far. So if the shortest
      // wildcard was "links:foo:", and the cid we're checking for is
      // "links:bar:bar", then the key will be "links:bar:".
      $keys = array_keys($this->wildcard_flushes[$this->bin]);
      $wildcard_length = strlen(reset($keys));
      $wildcard_key = substr($cid, 0, $wildcard_length);

      // Determine which lookups we need to perform to determine whether or not
      // our cid was impacted by a wildcard flush.
      $lookup = array();

      // Find statically cached wildcards, and determine possibly matching
      // wildcards for this cid based on a history of the lengths of past
      // valid wildcard flushes in this bin.
      if (isset($this->wildcard_flushes[$this->bin][$wildcard_key])) {
        foreach ($this->wildcard_flushes[$this->bin][$wildcard_key] as $flush_length => $timestamp) {
          if ($length >= $flush_length && $timestamp >= ($_SERVER['REQUEST_TIME'] - $this->invalidate)) {
            $wildcard = '.wildcard-' . substr($cid, 0, $flush_length);
            if (isset($wildcards[$this->bin][$wildcard])) {
              $matching[$wildcard] = $wildcards[$this->bin][$wildcard];
            }
            else {
              $lookup[$wildcard] = $wildcard;
            }
          }
        }
      }

      // Do a multi-get to retrieve all possibly matching wildcard flushes.
      if (!empty($lookup)) {
        $values = dmemcache_get_multi($lookup, $this->bin, $this->memcache);
        if (is_array($values)) {
          // Prepare an array of matching wildcards.
          $matching = array_merge($matching, $values);
          // Store matches in the static cache.
          if (isset($wildcards[$this->bin])) {
            $wildcards[$this->bin] = array_merge($wildcards[$this->bin], $values);
          }
          else {
            $wildcards[$this->bin] = $values;
          }
          $lookup = array_diff_key($lookup, $values);
        }

        // Also store failed lookups in our static cache, so we don't have to
        // do repeat lookups on single page loads.
        foreach ($lookup as $key => $key) {
          $wildcards[$this->bin][$key] = 0;
        }
      }
    }

    if ($flush) {
      $key_length = $length;
      if (isset($this->wildcard_flushes[$this->bin])) {
        $keys = array_keys($this->wildcard_flushes[$this->bin]);
        $key_length = strlen(reset($keys));
      }
      $key = substr($cid, 0, $key_length);
      // Avoid too many calls to variable_set() by only recording a flush for
      // a fraction of the wildcard invalidation variable, per cid length.
      // Defaults to 28 / 4, or one week.
      if (!isset($this->wildcard_flushes[$this->bin][$key][$length]) || ($_SERVER['REQUEST_TIME'] - $this->wildcard_flushes[$this->bin][$key][$length] > $this->invalidate / 4)) {

        // If there are more than 50 different wildcard keys for this bin
        // shorten the key by one, this should reduce variability by
        // an order of magnitude and ensure we don't use too much memory.
        if (isset($this->wildcard_flushes[$this->bin]) && count($this->wildcard_flushes[$this->bin]) > 50) {
          $key = substr($cid, 0, $key_length - 1);
          $length = strlen($key);
        }

        // If this is the shortest key length so far, we need to remove all
        // other wildcards lengths recorded so far for this bin and start
        // again. This is equivalent to a full cache flush for this table, but
        // it ensures the minimum possible number of wildcards are requested
        // along with cache consistency.
        if ($length < $key_length) {
          $this->wildcard_flushes[$this->bin] = array();
          $this->variable_set("cache_flush_$this->bin", time());
          $this->cache_flush = time();
        }
        $key = substr($cid, 0, $key_length);
        $this->wildcard_flushes[$this->bin][$key][$length] = $_SERVER['REQUEST_TIME'];

        variable_set('memcache_wildcard_flushes', $this->wildcard_flushes);
      }
      $key = '.wildcard-' . $cid;
      if (isset($wildcards[$this->bin][$key])) {
        $wildcards[$this->bin][$key]++;
      }
      else {
        $wildcards[$this->bin][$key] = 1;
      }
      dmemcache_set($key, $wildcards[$this->bin][$key], 0, $this->bin);
    }
    return $matching;
  }

  /**
   * Check if a wildcard flush has invalidated the current cached copy.
   */
  protected function wildcard_valid($cid, $cache) {
    // Previously cached content won't have ->flushes defined.  We could
    // force flush, but instead leave this up to the site admin.
    $flushes = isset($cache->flushes) ? (int)$cache->flushes : 0;
    if ($flushes < (int)$this->wildcard_flushes($cid)) {
      return FALSE;
    }
    return TRUE;
  }

  function isEmpty() {
    // We do not know so err on the safe side?
    return FALSE;
  }

  /**
   * Helper function to reload variables.
   *
   * This is used by the tests to verify that the cache object used the correct
   * settings.
   */
  function reloadVariables() {
    $this->wildcard_flushes = variable_get('memcache_wildcard_flushes', array());
    $this->invalidate = variable_get('memcache_wildcard_invalidate', MEMCACHE_WILDCARD_INVALIDATE);
    $this->cache_lifetime = variable_get('cache_lifetime', 0);
    $this->cache_flush = variable_get('cache_flush_' . $this->bin);
    $this->cache_content_flush = variable_get('cache_content_flush_' . $this->bin, 0);
    $this->flushed = min($this->cache_flush, REQUEST_TIME - $this->cache_lifetime);
  }

  /**
   * Re-implementation of variable_set() that writes through instead of clearing.
   */
  function variable_set($name, $value) {
    global $conf;

    db_merge('variable')
      ->key(array('name' => $name))
      ->fields(array('value' => serialize($value)))
      ->execute();
    // If the variables are cached, get a fresh copy, update with the new value
    // and set it again.
    if ($cached = cache_get('variables', 'cache_bootstrap')) {
      $variables = $cached->data;
      $variables[$name] = $value;
      cache_set('variables', $variables, 'cache_bootstrap');
    }
    // If the variables aren't cached, there's no need to do anything.
    $conf[$name] = $value;
  }
}
