This module integrates the Plupload library (available from http://plupload.com)
with Drupal forms. To install the Plupload library:

1. Download it (version 1.5.1.1 or later) from http://plupload.com.
2. Unzip it into sites/all/libraries, so that there's a
   sites/all/libraries/plupload/js/plupload.full.js file, in addition to the
   other files included in the library.

If you would like to use an alternate library location, you can install the
http://drupal.org/project/libraries module and/or add

  $conf['plupload_library_path'] = PATH/TO/PLUPLOAD;

to your settings.php file.

At this time, this module only provides a 'plupload' form element type that
other modules can use for providing multiple file upload capability to their
forms. It does not provide any end-user functionality on its own. This may
change, however, as this module evolves. See http://drupal.org/node/880300.
