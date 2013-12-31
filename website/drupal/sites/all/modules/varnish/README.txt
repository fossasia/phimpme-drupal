This module provides integration between your Drupal site and the
Varnish HTTP Accelerator, an advanced and very fast reverse-proxy system.
Basically, Varnish handles serving static files and anonymous page-views
for your site much faster and at higher volumes than Apache,
in the neighborhood of 3000 requests per second.


## What does it do?
The main function of the varnish.module is to dynamically expire/purge items
from the Varnish cache when things change in Drupal. Future innovations may
allow dynamic adjustment of the VCL (varnish configuration), but for now the
purpose is to clear the external cache on demand.


## Installation
First of all, you need to have Varnish installed on your server. For more
information on how to do this, please see:

http://www.varnish-cache.org/

Once you have Varnish working, use the module you need to do the following:

* Enable the module.
* Add something like this to your settings.php file:

// Add Varnish as the page cache handler.
$conf['cache_backends'] = array('sites/all/modules/varnish/varnish.cache.inc');
$conf['cache_class_cache_page'] = 'VarnishCache';
// Drupal 7 does not cache pages when we invoke hooks during bootstrap.
// This needs to be disabled.
$conf['page_cache_invoke_hooks'] = FALSE;

* Go to admin/config/development/varnish and configure your connection Varnish
  appropriately. It should be pretty straight forward from here on.


## Running the simpletests for Varnish
In order to test the Varnish module, you need a "working" Varnish
configuration that caches pages for anonymous users. You also need to specify
the variables that configures your Varnish connection in your $conf array in
your settings.php file.


## Troubleshooting
Please note that Varnish version 3 automatically configures a random key to
protect access to the control terminal, which the module needs to use. You
will either need to get the key from the secret key file (/etc/varnish/secret
in Ubuntu), or adjust your Varnish configuration not to use a key.

Debugging Varnish itself is beyond the scope of this module, but you can also
see if Varnish is functioning correctly with this site:

http://www.isvarnishworking.com/

There are also a number of resources available from the Varnish community:

http://www.varnish-cache.org/
