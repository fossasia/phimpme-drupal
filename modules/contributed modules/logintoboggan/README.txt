
*******************************************************
    README.txt for logintoboggan.module for Drupal
*******************************************************

Co-developed by Jeff Robbins (jjeff) and Chad Phillips (hunmonk) with several
features added by Raven Brooks (rbrooks00).

The Login Toboggan module improves the Drupal login system in an external
module by offering the following features:

   1. Allow users to login using either their username OR their email address.
   2. Allow users to login immediately.
   3. Provide a login form on Access Denied pages for non-logged-in
      (anonymous) users.
   4. The module provides two login block options: One uses JavaScript to
      display the form within the block immediately upon clicking "log in".
      The other brings the user to a separate page, but returns the user to
      their original page upon login.
   5. Customize the registration form with two e-mail fields to ensure
      accuracy.
   6. Optionally redirect the user to a specific page when using the
      'immediate login' feature.
   7. Optionally redirect the user to a specific page upon validation of their
      e-mail address.
   8. Optionally display a user message indicating a successful login.
   9. Optionally combine both the login and registration form on one page.
  10. Optionally display a 'Request new password' link on the user login form.
  11. Optionally have unvalidated users purged from the system at a pre-defined
      interval
      (please read the CAVEATS section of INSTALL.txt for important information
       on configuringthis feature!).

Users who choose their own password can be automatically assigned to a selected
'non-authenticated' role. This role could have more permissions than anonymous
but less than authenticated - thus preventing spoof accounts and spammers. The
user will only be removed from the non-authenticated role and granted
authenticated permissions when they verify their account via a special email
link, or when an administrator removes them from the non-authenticated role.

