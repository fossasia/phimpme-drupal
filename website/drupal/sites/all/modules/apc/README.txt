1. Installation
2. Testing

/*****************
 * 1. Installation
 ****************/
- Step 1
Enable the module and make sure the APC extension is installed properly on
the status page (http://yoursite/admin/reports/status).

Step two is important to think about because it can make your site faster
or slower depending on the right configuration. APC normally has a limited
memory allocation (32M) and should be used to cache the entries which are 
used most since it's the cache closest (and maybe fastest) to PHP. When 
the memory allocated by APC is big enough to cache the entire drupal
cache (to do so check the size of the cache_% tables in the database when
the cache is hot) you can use Step2b, if not use step 2a.

- Step 2a
Add the following code to your settings.php file:

/**
 * Add APC Caching.
 */
$conf['cache_backends'] = array('sites/all/modules/apc/drupal_apc_cache.inc');
$conf['cache_class_cache'] = 'DrupalAPCCache';
$conf['cache_class_cache_bootstrap'] = 'DrupalAPCCache';
//$conf['apc_show_debug'] = TRUE;  // Remove the slashes to use debug mode.

- Step 2b
Add the following code to your settings.php file:

/**
 * Add APC Caching.
 */
$conf['cache_backends'] = array('sites/all/modules/apc/drupal_apc_cache.inc');
$conf['cache_default_class'] = 'DrupalAPCCache';
//$conf['apc_show_debug'] = TRUE;  // Remove the slashes to use debug mode.

- Step 3
Visit your site to see or it's still working!

- Step 4 (OPTIONAL)
When using DrupalAPCCache as default or manually caching the 'cache_page' bin
in your settings file you do not need to start the database because Drupal can
use the APC cache for pages. Add the following code to your settings.php file
to do so:

$conf['page_cache_without_database'] = TRUE;
$conf['page_cache_invoke_hooks'] = FALSE;

- Step 5 (OPTIONAL)
Visit your site to see or it's still working!


/*****************
 * 2. Testing
 ****************/
To be able to test this module open DRUPAL_ROOT/includes/cache.inc and search
for `variable_get('cache_default_class', 'DrupalDatabaseCache')`. and change
this to DrupalAPCCache. This is because the $conf[''] array in settings.php
is not always loaded properly.
