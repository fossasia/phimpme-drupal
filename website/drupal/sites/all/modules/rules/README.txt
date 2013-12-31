
--------------------------------------------------------------------------------
                                 Rules
--------------------------------------------------------------------------------

Maintainers:
 * Wolfgang Ziegler (fago), nuppla@zites.net

The Rules module allows site administrators to define conditionally executed
actions based on occurring events (ECA-rules).

Project homepage: http://drupal.org/project/rules


Installation
------------

*Before* starting, make sure that you have read at least the introduction - so
you know at least the basic concepts. You can find it here:

                      http://drupal.org/node/298480

 * Rules depends on the Entity API module, download and install it from
   http://drupal.org/project/entity
 * Copy the whole rules directory to your modules directory
   (e.g. DRUPAL_ROOT/sites/all/modules) and activate the Rules and Rules UI
   modules.
 * The administrative user interface can be found at admin/config/workflow/rules


Documentation
-------------
* Check out the general docs at http://drupal.org/node/298476
* Check out the developer targeted docs at http://drupal.org/node/878718


Rules Scheduler
---------------

 * If you enable the Rules scheduler module, you get new actions that allow you
   to schedule the execution of Rules components.
 * Make sure that you have configured cron for your drupal installation as cron
   is used for scheduling the Rules components. For help see
   http://drupal.org/cron
 * If the Views module (http://drupal.org/project/views) is installed, the module
   displays the list of scheduled tasks in the UI.


Upgrade from Rules 6.x-1.x to Rules 7.x-2.x
--------------------------------------------

 * In order to upgrade Rules from 6.x-1.x to 7.x-2.x just run "update.php". This
   is going to make sure Rules 2.x is properly installed, but it will leave your
   Rules 1.x configurations untouched. Thus, your rules won't be upgraded yet.
 * To convert your Rules 1.x configurations to Rules 2.x go to
   'admin/config/workflow/rules/upgrade'.
     * At this page, you may choose the Rules 1.x rules and rule sets to upgrade
       and whether the converted configurations should be immediately saved to
       your database or whether the configuration export should be generated.
     * Note that for importing an export the export needs to pass the
       configuration integrity check, what might be troublesome if the
       conversion was not 100% successful. In that case, try choosing the
       immediate saving method and correct the configuration after conversion.  
     * A rule configuration might require multiple modules to be in place and
       upgraded to work properly. E.g. if you used an action provided
       by a third party module, make sure the module is in place and upgraded
       before you convert the rule.
     * If all required modules are installed and have been upgraded but the rule
       conversion still fails, the cause might be that a module has not yet
       upgraded its Rules integration or does not implement the Rules conversion
       functionality. In that case, file an issue for the module that provided
       the action or condition causing the conversion to fail.
     * Note that any rule configurations containing token replacements or PHP
       input evaluations might need some manual corrections in order to stay
       working. This is, as some used token replacements might not be available
       in Drupal 7 any more and the PHP code might need to be updated in order
       to be compatible with Drupal 7.
     * Once the upgrade was successful, you may delete the left over Rules 1.x
       configurations by going to 'admin/config/workflow/rules/upgrade/clear'.
  * The Rules Scheduler module also comes with an upgrade routine that is
    invoked as usual via "update.php". Its actions can be upgraded via the usual
    Rules upgrade tool, see above.
    However, there is currently no support for upgrading already scheduled
    tasks. That means, all previously on Drupal 6 scheduled tasks won't apply
    for Drupal 7. The Drupal 6 tasks are preserved in the database as long as
    you do not clear your Rules 1.x configuration though.
  * The Rules Forms module has not been updated to Drupal 7 and there are no
    plans to do so, as unfortuntely the module's design does not allow for
    automatic configuration updates.
    Thus, a possible future Rules 2.x Forms module is likely to work
    different, e.g. by working only for entity forms on the field level.
