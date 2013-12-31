<?php

/**
 * @file
 * Cache include file, to be used in settings.php file.
 */

/**
 * Varnish cache implementation.
 *
 * This cache implementation can be used together with Varnish. You can't really
 * use it to store or get any values, but you can use it to purge your caches.
 * This cache implementation should ONLY be used for cache_page and no other
 * cache bin!
 */
class VarnishCache implements DrupalCacheInterface {
  protected $bin;

  function __construct($bin) {
    $this->bin = $bin;
  }

  function get($cid) {
    return FALSE;
  }

  function getMultiple(&$cids) {
    return array();
  }

  function set($cid, $data, $expire = CACHE_PERMANENT) {
    // We won't do anything here.
  }

  function clear($cid = NULL, $wildcard = FALSE) {
    global $user;
    // Check that we really want to do a cache flush.
    if (!module_exists('varnish') ||
      (variable_get('varnish_flush_cron', 0) && lock_may_be_available('cron'))) {
      return;
    }
    if (empty($cid) && variable_get('varnish_cache_clear', 1)) {
      if (variable_get('cache_lifetime', 0)) {
        // We store the time in the current user's $user->cache variable which
        // will be saved into the sessions bin by _drupal_session_write(). We then
        // simulate that the cache was flushed for this user by not returning
        // cached data that was cached before the timestamp.
        $user->cache = REQUEST_TIME;

        $cache_flush = variable_get('cache_flush_' . $this->bin, 0);
        if ($cache_flush == 0) {
          // This is the first request to clear the cache, start a timer.
          variable_set('cache_flush_' . $this->bin, REQUEST_TIME);
        }
        elseif (REQUEST_TIME > ($cache_flush + variable_get('cache_lifetime', 0))) {
          // Clear the cache for everyone, cache_lifetime seconds have
          // passed since the first request to clear the cache.
          varnish_purge_all_pages();
          variable_set('cache_flush_' . $this->bin, 0);
        }
      }
      else {
        // No minimum cache lifetime, flush all temporary cache entries now.
        varnish_purge_all_pages();
      }
    }
    else {
      if ($wildcard) {
        if ($cid == '*') {
          varnish_purge_all_pages();
        }
        else {
          $host = _varnish_get_host();
          $base = base_path();
          $purge = $cid . '(.*)';
          _varnish_terminal_run(array('purge req.http.host ~ ' . $host . ' && req.url ~ "^' . $base . $purge . '$"'));
        }
      }
      elseif (is_array($cid)) {
        varnish_expire_cache($cid);
      }
      else {
        varnish_expire_cache(array($cid));
      }
    }
  }

  function isEmpty() {
    return FALSE;
  }
}
