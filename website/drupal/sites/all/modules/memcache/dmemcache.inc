<?php

/*
 * Core dmemcache functions required by:
 *   memcache.inc
 *   memcache.db.inc
 *   session-memcache.inc
 *   session-memcache.db.inc
 */

global $_memcache_statistics;
$_memcache_statistics = array();

/*
 * A memcache API for Drupal.
 */

/**
 *  Place an item into memcache
 *
 * @param $key The string with which you will retrieve this item later.
 * @param $value The item to be stored.
 * @param $exp Parameter expire is expiration time in seconds. If it's 0, the
 *   item never expires (but memcached server doesn't guarantee this item to be
 *   stored all the time, it could be deleted from the cache to make place for
 *   other items).
 * @param $bin The name of the Drupal subsystem that is making this call.
 *   Examples could be 'cache', 'alias', 'taxonomy term' etc. It is possible to
 *   map different $bin values to different memcache servers.
 * @param $mc Optionally pass in the memcache object.  Normally this value is
 *   determined automatically based on the bin the object is being stored to.
 *
 * @return bool
 */
function dmemcache_set($key, $value, $exp = 0, $bin = 'cache', $mc = NULL) {
  global $_memcache_statistics;
  $full_key = dmemcache_key($key, $bin);
  $_memcache_statistics[] = array('set', $bin, $full_key, '');
  if ($mc || ($mc = dmemcache_object($bin))) {
    if ($mc instanceof Memcached) {
      return $mc->set($full_key, $value, $exp);
    }
    else {
      return $mc->set($full_key, $value, MEMCACHE_COMPRESSED, $exp);
    }
  }
  return FALSE;
}

/**
 *  Add an item into memcache
 *
 * @param $key The string with which you will retrieve this item later.
 * @param $value The item to be stored.
 * @param $exp Parameter expire is expiration time in seconds. If it's 0, the
 *   item never expires (but memcached server doesn't guarantee this item to be
 *   stored all the time, it could be deleted from the cache to make place for
 *   other items).
 * @param $bin The name of the Drupal subsystem that is making this call.
 *   Examples could be 'cache', 'alias', 'taxonomy term' etc. It is possible
 *   to map different $bin values to different memcache servers.
 * @param $mc Optionally pass in the memcache object.  Normally this value is
 *   determined automatically based on the bin the object is being stored to.
 * @param $flag If using the older memcache PECL extension as opposed to the
 *   newer memcached PECL extension, the MEMCACHE_COMPRESSED flag can be set
 *   to use zlib to store a compressed copy of the item.  This flag option is
 *   completely ignored when using the newer memcached PECL extension.
 *
 * @return bool
 */
function dmemcache_add($key, $value, $exp = 0, $bin = 'cache', $mc = NULL, $flag = FALSE) {
  global $_memcache_statistics;
  $full_key = dmemcache_key($key, $bin);
  $_memcache_statistics[] = array('add', $bin, $full_key, '');
  if ($mc || ($mc = dmemcache_object($bin))) {
    if ($mc instanceof Memcached) {
      return $mc->add($full_key, $value, $exp);
    }
    else {
      return $mc->add($full_key, $value, $flag, $exp);
    }
  }
  return FALSE;
}

/**
 * Retrieve a value from the cache.
 *
 * @param $key The key with which the item was stored.
 * @param $bin The bin in which the item was stored.
 *
 * @return The item which was originally saved or FALSE
 */
function dmemcache_get($key, $bin = 'cache', $mc = NULL) {
  global $_memcache_statistics;
  $result = FALSE;
  $full_key = dmemcache_key($key, $bin);
  $statistics = array('get', $bin, $full_key);
  $success = '0';
  if ($mc || $mc = dmemcache_object($bin)) {
    $track_errors = ini_set('track_errors', '1');
    $php_errormsg = '';

    $result = @$mc->get($full_key);
    $statistics[] = (bool) $result;
    $_memcache_statistics[] = $statistics;

    if (!empty($php_errormsg)) {
      register_shutdown_function('watchdog', 'memcache', 'Exception caught in dmemcache_get: !msg', array('!msg' => $php_errormsg), WATCHDOG_WARNING);
      $php_errormsg = '';
    }
    ini_set('track_errors', $track_errors);
  }

  return $result;
}

/**
 * Retrieve multiple values from the cache.
 *
 * @param $keys The keys with which the items were stored.
 * @param $bin The bin in which the item was stored.
 *
 * @return The item which was originally saved or FALSE
 */
function dmemcache_get_multi($keys, $bin = 'cache', $mc = NULL) {
  global $_memcache_statistics;
  $full_keys = array();
  $statistics = array();
  foreach ($keys as $key => $cid) {
    $full_key = dmemcache_key($cid, $bin);
    $statistics[$full_key] = array('getMulti', $bin, $full_key);
    $full_keys[$cid] = $full_key;
  }
  $results = array();
  if ($mc || ($mc = dmemcache_object($bin))) {
    if ($mc instanceof Memcached) {
      $results = $mc->getMulti($full_keys);
    }
    elseif ($mc instanceof Memcache) {
      $track_errors = ini_set('track_errors', '1');
      $php_errormsg = '';

      $results = @$mc->get($full_keys);

      if (!empty($php_errormsg)) {
        register_shutdown_function('watchdog', 'memcache', 'Exception caught in dmemcache_get_multi: !msg', array('!msg' => $php_errormsg), WATCHDOG_WARNING);
        $php_errormsg = '';
      }
      ini_set('track_errors', $track_errors);
    }
  }
  foreach ($statistics as $key => $values) {
    $values[] = isset($results[$key]) ? '1': '0';
    $_memcache_statistics[] = $values;
  }

  // If $results is FALSE, convert it to an empty array.
  if (!$results) {
    $results = array();
  }

  // Convert the full keys back to the cid.
  $cid_results = array();
  $cid_lookup = array_flip($full_keys);
  foreach ($results as $key => $value) {
    $cid_results[$cid_lookup[$key]] = $value;
  }
  return $cid_results;
}

/**
 * Deletes an item from the cache.
 *
 * @param $key The key with which the item was stored.
 * @param $bin The bin in which the item was stored.
 *
 * @return Returns TRUE on success or FALSE on failure.
 */
function dmemcache_delete($key, $bin = 'cache', $mc = NULL) {
  global $_memcache_statistics;
  $full_key = dmemcache_key($key, $bin);
  $_memcache_statistics[] = array('delete', $bin, $full_key, '');
  if ($mc || ($mc = dmemcache_object($bin))) {
    return $mc->delete($full_key, 0);
  }
  return FALSE;
}

/**
 * Immediately invalidates all existing items. dmemcache_flush doesn't actually free any
 * resources, it only marks all the items as expired, so occupied memory will be overwritten by
 * new items.
 *
 * @param $bin The bin to flush. Note that this will flush all bins mapped to the same server
 *   as $bin. There is no way at this time to empty just one bin.
 *
 * @return Returns TRUE on success or FALSE on failure.
 */
function dmemcache_flush($bin = 'cache', $mc = NULL) {
  global $_memcache_statistics;
  $_memcache_statistics[] = array('flush', $bin, '', '');
  if ($mc || ($mc = dmemcache_object($bin))) {
    return memcache_flush($mc);
  }
}

function dmemcache_stats($stats_bin = 'cache', $stats_type = 'default', $aggregate = FALSE) {
  $memcache_bins = variable_get('memcache_bins', array('cache' => 'default'));
  // The stats_type can be over-loaded with an integer slab id, if doing a
  // cachedump.  We know we're doing a cachedump if $slab is non-zero.
  $slab = (int)$stats_type;

  foreach ($memcache_bins as $bin => $target) {
    if ($stats_bin == $bin) {
      if ($mc = dmemcache_object($bin)) {
        if ($mc instanceof Memcached) {
          $stats[$bin] = $mc->getStats();
        }
        // The PHP Memcache extension 3.x version throws an error if the stats
        // type is NULL or not in {reset, malloc, slabs, cachedump, items,
        // sizes}. If $stats_type is 'default', then no parameter should be
        // passed to the Memcache memcache_get_extended_stats() function.
        else if ($mc instanceof Memcache) {
          if ($stats_type == 'default' || $stats_type == '') {
            $stats[$bin] = $mc->getExtendedStats();
          }
          // If $slab isn't zero, then we are dumping the contents of a
          // specific cache slab.
          else if (!empty($slab))  {
            $stats[$bin] = $mc->getStats('cachedump', $slab);
          }
          else {
            $stats[$bin] = $mc->getExtendedStats($stats_type);
          }
        }
      }
    }
  }
  // Optionally calculate a sum-total for all servers in the current bin.
  if ($aggregate) {
    // Some variables don't logically aggregate.
    $no_aggregate = array('pid', 'time', 'version', 'pointer_size', 'accepting_conns', 'listen_disabled_num');
    foreach($stats as $bin => $servers) {
      if (is_array($servers)) {
        foreach ($servers as $server) {
          if (is_array($server)) {
            foreach ($server as $key => $value) {
              if (!in_array($key, $no_aggregate)) {
                if (isset($stats[$bin]['total'][$key])) {
                  $stats[$bin]['total'][$key] += $value;
                }
                else {
                  $stats[$bin]['total'][$key] = $value;
                }
              }
            }
          }
        }
      }
    }
  }
  return $stats;
}

/**
 * Returns an Memcache object based on the bin requested. Note that there is
 * nothing preventing developers from calling this function directly to get the
 * Memcache object. Do this if you need functionality not provided by this API
 * or if you need to use legacy code. Otherwise, use the dmemcache (get, set,
 * delete, flush) API functions provided here.
 *
 * @param $bin The bin which is to be used.
 *
 * @param $flush Rebuild the bin/server/cache mapping.
 *
 * @return an Memcache object or FALSE.
 */
function dmemcache_object($bin = NULL, $flush = FALSE) {
  static $extension, $memcacheCache = array(), $memcache_servers, $memcache_bins, $memcache_persistent, $failed_connection_cache;

  if (!isset($extension)) {
    // If an extension is specified in settings.php, use that when available.
    $preferred = variable_get('memcache_extension', NULL);
    if (isset($preferred) && class_exists($preferred)) {
      $extension = $preferred;
    }
    // If no extension is set, default to Memcache.
    // The Memcached extension has some features that the older extension lacks
    // but also an unfixed bug that affects cache clears.
    // @see http://pecl.php.net/bugs/bug.php?id=16829
    elseif (class_exists('Memcache')) {
      $extension = 'Memcache';
    }
    elseif (class_exists('Memcached')) {
      $extension = 'Memcached';
    }

    // Indicate whether to connect to memcache using a persistent connection.
    // Note: this only affects the Memcache PECL extension, and does not
    // affect the Memcached PECL extension.  For a detailed explanation see:
    //  http://drupal.org/node/822316#comment-4427676
    if (!isset($memcache_persistent)) {
      $memcache_persistent = variable_get('memcache_persistent', FALSE);
    }
  }

  if ($flush) {
    foreach ($memcacheCache as $cluster) {
      memcache_close($cluster);
    }
    $memcacheCache = array();
  }

  if (empty($memcacheCache) || empty($memcacheCache[$bin])) {
    // $memcache_servers and $memcache_bins originate from settings.php.
    // $memcache_servers_custom and $memcache_bins_custom get set by
    // memcache.module. They are then merged into $memcache_servers and
    // $memcache_bins, which are statically cached for performance.
    if (empty($memcache_servers)) {
      // Values from settings.php
      $memcache_servers = variable_get('memcache_servers', array('127.0.0.1:11211' => 'default'));
      $memcache_bins    = variable_get('memcache_bins', array('cache' => 'default'));
    }

    // If there is no cluster for this bin in $memcache_bins, cluster is 'default'.
    $cluster = empty($memcache_bins[$bin]) ? 'default' : $memcache_bins[$bin];

    // If this bin isn't in our $memcache_bins configuration array, and the
    // 'default' cluster is already initialized, map the bin to 'cache' because
    // we always map the 'cache' bin to the 'default' cluster.
    if (empty($memcache_bins[$bin]) && !empty($memcacheCache['cache'])) {
      $memcacheCache[$bin] = &$memcacheCache['cache'];
    }
    else {
      // Create a new Memcache object. Each cluster gets its own Memcache object.
      if ($extension == 'Memcached') {
        $memcache = new Memcached;
        $default_opts = array(
          Memcached::OPT_COMPRESSION => FALSE,
          Memcached::OPT_DISTRIBUTION => Memcached::DISTRIBUTION_CONSISTENT,
        );
        foreach ($default_opts as $key => $value) {
          $memcache->setOption($key, $value);
        }
        // See README.txt for setting custom Memcache options when using the
        // memcached PECL extension.
        $memconf = variable_get('memcache_options', array());
        foreach ($memconf as $key => $value) {
          $memcache->setOption($key, $value);
        }
      }
      elseif ($extension == 'Memcache') {
        $memcache = new Memcache;
      }
      else {
        drupal_set_message(t('You must enable the PECL memcached or memcache extension to use memcache.inc.'), 'error');
        return;
      }
      // A variable to track whether we've connected to the first server.
      $init = FALSE;

      // Link all the servers to this cluster.
      foreach ($memcache_servers as $s => $c) {
        if ($c == $cluster && !isset($failed_connection_cache[$s])) {
          list($host, $port) = explode(':', $s);

          // Support unix sockets in the format 'unix:///path/to/socket'.
          if ($host == 'unix') {
            // When using unix sockets use the full path for $host.
            $host = $s;
            // Port is always 0 for unix sockets.
            $port = 0;
          }

          // Using the Memcache PECL extension.
          if ($memcache instanceof Memcache) {
            // When using the PECL memcache extension, we must use ->(p)connect
            // for the first connection.
            if (!$init) {
              $track_errors = ini_set('track_errors', '1');
              $php_errormsg = '';

              if ($memcache_persistent && @$memcache->pconnect($host, $port)) {
                $init = TRUE;
              }
              elseif (!$memcache_persistent && @$memcache->connect($host, $port)) {
                $init = TRUE;
              }

              if (!empty($php_errormsg)) {
                register_shutdown_function('watchdog', 'memcache', 'Exception caught in dmemcache_object: !msg', array('!msg' => $php_errormsg), WATCHDOG_WARNING);
                $php_errormsg = '';
              }
              ini_set('track_errors', $track_errors);
            }
            else {
              $memcache->addServer($host, $port, $memcache_persistent);
            }
          }
          else if ($memcache->addServer($host, $port) && !$init) {
            $init = TRUE;
          }

          if (!$init) {
            // Ensure we use an available t() function.
            $t = get_t();
            $error_msg = $t(
              'Failed to connect to memcache server: %server',
              array('%server' => $s)
            );
            // We can't use watchdog because this happens in a bootstrap phase
            // where watchdog is non-functional. Thus use trigger_error() to
            // start drupal_error_handler().
            trigger_error($error_msg, E_USER_ERROR);
            $failed_connection_cache[$s] = FALSE;
          }
        }
      }

      if ($init) {
        // Map the current bin with the new Memcache object.
        $memcacheCache[$bin] = $memcache;

        // Now that all the servers have been mapped to this cluster, look for
        // other bins that belong to the cluster and map them too.
        foreach ($memcache_bins as $b => $c) {
          if ($c == $cluster && $b != $bin) {
            // Map this bin and cluster by reference.
            $memcacheCache[$b] = &$memcacheCache[$bin];
          }
        }
      }
    }
  }

  return empty($memcacheCache[$bin]) ? FALSE : $memcacheCache[$bin];
}

function dmemcache_key($key, $bin = 'cache') {
  $prefix = '';
  if ($prefix = variable_get('memcache_key_prefix', '')) {
    $prefix .= '-';
  }
  // When simpletest is running, emulate the simpletest database prefix here
  // to avoid the child site setting cache entries in the parent site.
  if (isset($GLOBALS['drupal_test_info']['test_run_id'])) {
    $prefix .= $GLOBALS['drupal_test_info']['test_run_id'];
  }
  $full_key = urlencode($prefix . $bin . '-' . $key);

  // Memcache only supports key lengths up to 250 bytes.  If we have generated
  // a longer key, hash it with sha1 which will shrink the key down to 40 bytes
  // while still keeping it unique.
  if (strlen($full_key) > 250) {
    $full_key = $prefix . $bin . '-' . sha1($key);
  }

  return $full_key;
}
