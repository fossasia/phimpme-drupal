$Id: README.txt,v 1.2 2010/11/12 19:30:14 dwightaspinwall Exp $

File Logger Module
------------------------
by Dwight Aspinwall, dwight.aspinwall@gmail.com


Description
-----------
This is a simple module that allows developers to configure a log file from within Drupal and
dumps variables to it from within a running Drupal app.  Its sole function is to support 
debugging, and avoids the awkwardness of dumping variables either to the console or to the 
watchdog table.  Instead, using the unix tail command, a developer can easily view debugging 
output.  This is particularly useful when dumping large data structures such as nodes, menus,
etc.


Installation 
------------
 * Copy the module's directory to your modules directory and activate the module.
 * In Site configuration > File logging (admin/settings/flog), specify the path
   to the log file(s) and the default log file name.
 * (Optionally) Configure the date string to be included with each logged variable.
 * Ensure that the user running the webserver has write permission on the file you specify.
   It may be simpler to create that file in advance using the unix command 'touch' as in
   'touch /var/log/drupal.log'.  Then, set the permissions on the log file so it is writeable
   by the web server user, e.g. 'chmod 777 /var/log/drupal.log'.
 * Enable file logging.  When disabled, no output is written.  You'll probably want to disable 
   file logging in a production environment. It's not necessary to disable the module itself.


Using File logging
------------------------
To dump a variable call the function flog_it() with any variable.  Here is an example:

<?php
...
function my_module_form_alter(&$form, $form_state, $form_id) {
  ...
  // What does the form data structure look like?
  flog_it($form);
  ...
}

You can also add a label to a flog_it() call, which makes it easier to find, especially when
dumping lots of stuff.  The label goes into the second argument:

<?php
...
flog_it($form, 'FORM ID: ' . $form_id);
...

You can optionally provide a filename to flog_it(), which overrides the default log file (but uses 
the configured directory):

<?php
...
flog_it($form, 'FORM ID: ' . $form_id, 'some_other_file_name.txt');
...

Finally, there is one other convenience function, when dumps the php stack at the time of
execution of the statement.  This function takes one optional argument, which is a label:

<?php
...
flog_stack('stack before first call to foo()');
...

On unix systems, the tail command can be used to see output from flog_it() and flog_stack() calls, e.g.:

tail -f /var/log/drupal.log



