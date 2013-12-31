
## REQUIREMENTS ##

- PHP 5.1 or greater
- Availability of a memcached daemon: http://memcached.org/
- One of the two PECL memcache packages:
  - http://pecl.php.net/package/memcache (recommended):
  - http://pecl.php.net/package/memcached

## INSTALLATION ##

These are the broad steps you need to take in order to use this software. Order
is important.

 1. Install the memcached binaries on your server. See for instance:
      http://www.lullabot.com/articles/how_install_memcache_debian_etch
 2. Install the PECL memcache extension for PHP. This must be version 2.2.1 or 
    higher or you will experience errors.
 3. Put your site into offline mode.
 4. Download and install the memcache module.
 5. If you have previously been running the memcache module, run update.php.
 6. Start at least one instance of memcached on your server.
 7. Edit settings.php to configure the servers, clusters and bins that memcache
    is supposed to use.
 8. Edit settings.php to make memcache the default cache class, for example:
      $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
      $conf['cache_default_class'] = 'MemCacheDrupal';
 9. Make sure the following line also exists, to ensure that the special
    cache_form bin is assigned to non-volatile storage:
      $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';
10. Bring your site back online.

For instructions on 1 and 2 above, please see the INSTALLATION.txt file that
comes with the memcache module download.

## SERVERS ##

If you want the simple version, you can start one default memcache instance on
your web server like this: memcached -m 24 -p 11211 -d
If that is enough to meet your needs, there is no more configuration needed. If
you want to utilize this module's sophisticated clustering feature and spread
your cache over several machines, or if your cache is found on a machine other
than your web server, read on.

The available memcached servers are specified in $conf in settings.php. If
you do not specify any servers, memcache.inc assumes that you have a
memcached instance running on localhost:11211. If this is true, and it is
the only memcached instance you wish to use, no further configuration is
required.

If you have more than one memcached instance running, you need to add two
arrays to $conf; memcache_servers and memcache_bins. The arrays follow this
pattern:

'memcache_servers' => array(
  host1:port => cluster, 
  host2:port => cluster, 
  hostN:port => cluster
)

'memcache_bins' => array(bin1 => cluster, bin2 => cluster, binN => cluster)

The bin/cluster/server model can be described as follows:

- Servers are memcached instances identified by host:port.

- Bins are groups of data that get cached together and map 1:1 to the $table
  parameter of cache_set(). Examples from Drupal core are cache_filter,
  cache_menu. The default is 'cache'.

- Clusters are groups of servers that act as a memory pool.

- many bins can be assigned to a cluster.

- The default cluster is 'default'.

Here is a simple setup that has two memcached instances, both running on
localhost. The 11212 instance belongs to the 'pages' cluster and the table
cache_page is mapped to the 'pages' cluster. Thus everything that gets cached,
with the exception of the page cache (cache_page), will be put into 'default',
or the 11211 instance. The page cache will be in 11212.

$conf = array(
  ...
  // Important to define a default cluster in both the servers
  // and in the bins. This links them together.
  'memcache_servers' => array('localhost:11211' => 'default',
                              'localhost:11212' => 'pages'),
  'memcache_bins' => array('cache' => 'default',
                           'cache_page' => 'pages'),
);

Here is an example configuration that has two clusters, 'default' and
'cluster2'. Five memcached instances are divided up between the two
clusters. 'cache_filter' and 'cache_menu' bins go to 'cluster2'. All other
bins go to 'default'.

$conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
$conf['cache_default_class'] = 'MemCacheDrupal';
// The 'cache_form' bin must be assigned no non-volatile storage.
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';
$conf = array(
  'cache_default_class' = 'MemCacheDrupal',
  'memcache_servers' => array('localhost:11211' => 'default',
                              'localhost:11212' => 'default',
                              '123.45.67.890:11211' => 'default',
                              '123.45.67.891:11211' => 'cluster2',
                              '123.45.67.892:11211' => 'cluster2'),

  'memcache_bins' => array('cache' => 'default',
                           'cache_filter' => 'cluster2',
                           'cache_menu' => 'cluster2'),
);

## PREFIXING ##

If you want to have multiple Drupal installations share memcached instances,
you need to include a unique prefix for each Drupal installation in the $conf
array of settings.php:

$conf = array(
  ...
  'memcache_key_prefix' => 'something_unique',
);

## SESSIONS ##

NOTE: Session.inc is not yet ported to Drupal 7 and is not recommended for use
in production..

Here is a sample config that uses memcache for sessions. Note you MUST have
a session and a users server set up for memcached sessions to work.

$conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
$conf['cache_default_class'] = 'MemCacheDrupal';
// The 'cache_form' bin must be assigned no non-volatile storage.
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';
$conf = array(
  'cache_default_class' = 'MemCacheDrupal',
  'session_inc' => './sites/all/modules/memcache/memcache-session.inc',
  'memcache_servers' => array(
    'localhost:11211' => 'default',
    'localhost:11212' => 'filter',
    'localhost:11213' => 'menu',
    'localhost:11214' => 'page',
    'localhost:11215' => 'session',
    'localhost:11216' => 'users',
  ),
  'memcache_bins' => array(
    'cache' => 'default',
    'cache_filter' => 'filter',
    'cache_menu' => 'menu',
    'cache_page' => 'page',
    'session' => 'session',
    'users' => 'users',
  ),
);


## TROUBLESHOOTING ##

PROBLEM:
Error:
Failed to set key: Failed to set key: cache_page-......

SOLUTION:
Upgrade your PECL library to PECL package (2.2.1) (or higher).

WARNING: 
Zlib compression at the php.ini level and Memcache conflict. 
See http://drupal.org/node/273824

## MEMCACHE ADMIN ##

A module offering a UI for memcache is included. It provides stats, a
way to clear the cache, and an interface to organize servers/bins/clusters.

## Memcached PECL Extension Support

We also now support the Memcached PECL extension. This new extension backends
to libmemcached and allows you to use some of the newer advanced features in
memcached 1.4. 

NOTE: It is important to realize that the memcache php.ini options do not impact
the memcached extension, this new extension doesn't read in options that way.
Instead, it takes options directly from Drupal. Because of this, you must
configure memcached in settings.php. Please look here for possible options:

http://us2.php.net/manual/en/memcached.constants.php

An example configuration block is below, this block also illustrates our
default options. These will be set unless overridden in settings.php.

$conf['memcache_options'] = array(
  Memcached::OPT_COMPRESSION => FALSE,
  Memcached::OPT_DISTRIBUTION => Memcached::DISTRIBUTION_CONSISTENT,
);

These are as follows:

 * Turn off compression, as this takes more CPU cycles than its worth for most
   users
 * Turn on consistent distribution, which allows you to add/remove servers
   easily

If you are using memcached 1.4 or above, you should enable the binary protocol,
which is more advanced and faster, by adding the following to settings.php:

$conf['memcache_options'] = array(
  Memcached::OPT_BINARY_PROTOCOL => TRUE,
);
