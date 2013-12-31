No Current Password
Wesley Jones
http://www.wesjones.net
2012-02-08

This module disables the "current password" field that has been added to Drupal 7's user
edit form, at user/%/edit.

When you enable this module, the current password field will be removed by default.
To enable the password field again, go to admin/config/people/settings and uncheck the
"Do no require current password" checkbox.

This was a 5-year old issue: http://drupal.org/node/86299
Committed March 10, 2010.
However, I don't like the implementation. It was causing problems with my site. I feel
strongly that this should be optional. Therefore, this module was born.

If you don't agree with me, that's fine, that's why it's configurable!