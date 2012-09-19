
Content Access Module
-----------------------
by Wolfgang Ziegler, nuppla@zites.net

Yet another node access module.
This module allows you to manage permissions for content types by role. It allows you to specifiy
custom view, view own, edit, edit own, delete and delete own permissions for each content type.
Optionally you can enable per content access settings, so you can customize the access for each
content node.

In particular
  * it comes with sensible defaults, so you need not configure anything and everything stays working
  * it is as flexible as you want. It can work with per content type settings, per content node settings
    as well as with flexible Access Control Lists (with the help of the ACL module).
  * it trys to reuse existing functionality instead of reimplementing it. So one can install the ACL
    module and set per user access control settings per content node.
    Furthermore the module provides conditions and actions for the rules module, which allows one
    to configure even rule-based access permissions.
  * it optimizes the written content node grants, so that only the really necessary grants are written.
    This is important for the performance of your site.
  * it takes access control as important as it is. E.g. the module has a bunch of simpletests to ensure
    everything is working right.
  * it respects and makes use of drupal's built in permissions as far as possible. Which means the
    access control tab provided by this module takes them into account and provides you a good overview
    about the really applied access control settings. [1]


So the module is simple to use, but can be configured to provide really fine-grained permissions!


Installation
------------
 * Copy the content access module's directory to your modules directory and activate the module.
 * Optionally download and install the ACL module too.
 * Edit a content type at admin/content/types. There will be a new tab "Access Control".


ACL Module
-----------
You can find the ACL module at http://drupal.org/project/acl. To make use of Access Control Lists
you'll need to enable per content node access control settings for a content type. At the access
control tab of such a content node you are able to grant view, edit or delete permission for specific
users.


Running multiple node access modules on a site (Advanced!)
-----------------------------------------------------------
A drupal node access module can only grant access to content nodes, but not deny it. So if you
are using multiple node access modules, access will be granted to a node as soon as one of the
module grants access to it.
However you can influence the behaviour by changing the priority of the content access module as
drupal applies *only* the grants with the highest priority. So if content access has the highest
priority *alone*, only its grants will be applied. 

By default node access modules use priority 0.



Footnotes
----------

[1] Note that this overview can't take other modules into account, which might also alter node access.
    If you have multiple modules installed that alter node access, read the paragraph about "Running 
    multiple node access modules on a site".
