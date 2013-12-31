<?php
/**
 * @file
 * This integrates the drupal APC cache backend.
 */

// The cli mode does not run the same php.ini file and could not have
// loaded the apc functions. Create dummy's to prevent fatal errors.
if (drupal_is_cli() && !function_exists('apc_fetch')) {
  function apc_fetch($key, $success = NULL) {
    return FALSE;
  }

  function apc_store($key, $var, $ttl = 0) {
    return FALSE;
  }
}

// Array to store statistics about the current page apc calls.
$apc_statistics = array();

/**
 * APC cache implementation.
 *
 * This is Drupal's APC cache implementation. It uses Alternative PHP
 * Cache to store cached data. Each cache bin corresponds to a prefix of
 * the apc variables with the same name.
 */
class DrupalAPCCache implements DrupalCacheInterface {
  /**
   * @var string
   */
  protected $bin;

  /**
   * @var string
   */
  protected $prefix;

  /**
   * Get prefix for bin using the configuration.
   *
   * @param string $bin
   *
   * @return string
   *   Can be an empty string, if no prefix set.
   */
  protected static function getPrefixSettingForBin($bin) {
    $prefixes = variable_get('cache_prefix', '');

    if (is_string($prefixes)) {
      // Variable can be a string, which then considered as a default behavior.
      return $prefixes;
    }

    if (isset($prefixes[$bin])) {
      if (FALSE !== $prefixes[$bin]) {
        // If entry is set and not FALSE, an explicit prefix is set for the bin.
        return $prefixes[$bin];
      } else {
        // If we have an explicit false, it means no prefix whatever is the
        // default configuration.
        return '';
      }
    } else {
      // Key is not set, we can safely rely on default behavior.
      if (isset($prefixes['default']) && FALSE !== $prefixes['default']) {
        return $prefixes['default'];
      } else {
        // When default is not set or an explicit FALSE, this means no prefix.
        return '';
      }
    }
  }

  function __construct($bin) {
    $this->bin = $bin;

    // First we determine the prefix from a setting.
    $prefix = self::getPrefixSettingForBin($this->bin);

    // If we do not have a configured prefix we use the HTTP_HOST.
    if (empty($prefix) && isset($_SERVER['HTTP_HOST'])) {
      // Provide a fallback for multisite. This is on purpose not inside the
      // getPrefixForBin() function in order to decouple the unified prefix
      // variable logic and custom module related security logic, that is not
      // necessary for all backends.
      $prefix = $_SERVER['HTTP_HOST'] . '::';
    } else {
      $prefix = $prefix . '::';
    }

    // When we are in testing mode we add the test prefix.
    if ($test_prefix = drupal_valid_test_ua()) {
      $prefix = $test_prefix . '::' . $prefix;
    }
    else if (isset($GLOBALS['drupal_test_info'])) {
      $prefix = $GLOBALS['drupal_test_info']['test_run_id'] . '::' . $prefix;
    }

    $this->prefix = $prefix;
  }

  /**
   * Function which retrieves the safe key for the cache bin.
   *
   * @return
   *   The safe APC key.
   */
  private function binKey() {
    return $this->prefix . $this->bin . '::';
  }

  /**
   * Function which retrieves the safe key for the cache cid.
   *
   * @param $cid
   *   The cache id.
   * @return
   *   The safe APC key.
   */
  private function key($cid) {
    return $this->binKey() . $cid;
  }

  function get($cid) {
    // Add a get to our statistics.
    $GLOBALS['apc_statistics'][] = array('get', $this->bin, array($cid));

    // Fetch the data.
    $cache = apc_fetch($this->key($cid));
    return $this->prepareItem($cache);
  }

  /**
   * Prepare a cached item.
   *
   * Checks that items are either permanent or did not expire.
   *
   * @param $cache
   *   An item loaded from cache_get() or cache_get_multiple().
   * @return
   *   The item with data unserialized as appropriate or FALSE if there is no
   *   valid item to load.
   */
  protected function prepareItem($cache) {
    if (!isset($cache->data)) {
      return FALSE;
    }

    // If enforcing a minimum cache lifetime, validate that the data is
    // currently valid for this user before we return it by making sure the cache
    // entry was created before the timestamp in the current session's cache
    // timer. The cache variable is loaded into the $user object by _drupal_session_read()
    // in session.inc. If the data is permanent or we're not enforcing a minimum
    // cache lifetime always return the cached data.
    global $user;
    if ($cache->expire != CACHE_PERMANENT && variable_get('cache_lifetime', 0) && (isset($user->cache) && $user->cache > $cache->created)) {
      // This cache data is too old and thus not valid for us, ignore it.
      return FALSE;
    }

    return $cache;
  }

  function getMultiple(&$cids) {
    // We need to search the cache with the proper keys and
    // be able to get the original $cid back.
    foreach ($cids as $cid) {
      $keys[$this->key($cid)] = $cid;
    }

    $fetch = apc_fetch(array_keys($keys));
    $cache = array();
    foreach ($fetch as $key => $data) {
      $cache[$keys[$key]] = $this->prepareItem($fetch[$key]);
    }
    unset($fetch);

    // Add a get to our statistics.
    $GLOBALS['apc_statistics'][] = array('get', $this->bin, $cids);

    $cids = array_diff($cids, array_keys($cache));

    return $cache;
  }

  function set($cid, $data, $expire = CACHE_PERMANENT, array $headers = NULL) {
    // Add set to statistics.
    $GLOBALS['apc_statistics'][] = array('set', $this->bin, $cid);

    // Create new cache object.
    $cache = new stdClass();
    $cache->cid = $cid;
    // APC will serialize any structure we give itself.
    $cache->serialized = 0;
    $cache->created = REQUEST_TIME;
    $cache->expire = $expire;
    $cache->headers = isset($headers) ? $headers : NULL;

    $cache->data = $data;

    // What kind of expiration is being used.
    switch ($expire) {
      case CACHE_PERMANENT:
        $set_result = apc_store($this->key($cid), $cache);
        break;

      case CACHE_TEMPORARY:
        if (variable_get('cache_lifetime', 0) > 0) {
          $set_result = apc_store($this->key($cid), $cache, variable_get('cache_lifetime', 0));
        }
        else {
          $set_result = apc_store($this->key($cid), $cache);
        }
        break;

      default:
        $set_result = apc_store($this->key($cid), $cache, $expire - time());
        break;
    }
  }

 /**
   * Escape string for raw using in a PCRE regex.
   *
   * @param string $unsafeString
   *
   * @return string
   *   Safe string.
   */
  protected function escapeStringForRegex($unsafeString) {
    return preg_replace('/[\/\.\[\]\(\)\|]/', '\\\\\\0', $unsafeString);
  }

  /**
   * Delete CID matching the given prefix.
   *
   * @param string $prefix
   */
  protected function deletePrefix($prefix) {
    $match = $this->escapeStringForRegex($this->binKey() . $prefix);
    $iterator = new APCIterator('user', '/^' . $match . '/', APC_ITER_KEY);
    foreach ($iterator as $key => $data) {
      apc_delete($key);
     }
   }

  /**
   * Flush all cache items in a bin.
   */
  function flush() {
    $this->deletePrefix('');
  }

  function clear($cid = NULL, $wildcard = FALSE) {
    if (drupal_is_cli() && function_exists('drush_log')) {
      drush_log($this->bin . '(' . $cid . ') was not cleared. APC cli uses a different memory storage then the webserver. For more info see: http://drupal.org/node/1278232', 'warning');
      return;
    }

    // Add a get to our statistics.
    $GLOBALS['apc_statistics'][] = array('clear', $this->bin, $cid, (int)$wildcard);

    if (!empty($cid)) {
      if ($wildcard) {
        if ($cid == '*') {
          $this->flush();
        }
        else {
          $this->deletePrefix($cid);
        }
      }
      else if (is_array($cid)) {
        foreach ($cid as $entry) {
          apc_delete($this->key($entry));
        }
      }
      else {
        apc_delete($this->key($cid));
      }
    }
  }

  function isEmpty() {
    $match = $this->escapeStringForRegex($this->binKey());
    $iterator = new APCIterator('user', '/^' . $match . '/', APC_ITER_KEY);
    return 0 === $iterator->getTotalCount();
  }
}
