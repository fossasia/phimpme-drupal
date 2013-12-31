<?php

/**
 * @file
 *  Validation functions for LoginToboggan module.
/**

/**
 * Menu callback; validate the e-mail address as a one time URL, and redirects
 * to the user page on success.
 */
function logintoboggan_validate_email($account, $timestamp, $hashed_pass, $action = 'login') {
  global $user;

  // Test here for a valid pre-auth -- if the pre-auth is set to the auth user, we
  // handle things a bit differently.
  $validating_id = logintoboggan_validating_id();
  $pre_auth = !variable_get('user_email_verification', TRUE) && $validating_id != DRUPAL_AUTHENTICATED_RID;

  // No time out for first time login.
  // This conditional checks that:
  // - the user is still in the pre-auth role or didn't set
  //   their own password.
  // - the hashed password is correct.
  if (((variable_get('user_email_verification', TRUE) && empty($account->login)) || ($pre_auth && array_key_exists($validating_id, $account->roles))) && $hashed_pass == logintoboggan_eml_rehash($account->pass, $timestamp, $account->mail)) {
    watchdog('user', 'E-mail validation URL used for %name with timestamp @timestamp.', array('%name' => $account->name, '@timestamp' => $timestamp));

    _logintoboggan_process_validation($account);

    // Where do we redirect after confirming the account?
    $redirect = _logintoboggan_process_redirect(variable_get('logintoboggan_redirect_on_confirm', ''), $account);

    switch ($action) {
      // Proceed with normal user login, as long as it's open registration and their
      // account hasn't been blocked.
      case 'login':
        // Only show the validated message if there's a valid pre-auth role.
        if ($pre_auth) {
          drupal_set_message(t('You have successfully validated your e-mail address.'));
        }
        if (!$account->status) {
          drupal_set_message(t('Your account is currently blocked -- login cancelled.'), 'error');
          drupal_goto('');
        }
        else {
          $edit = array();
          $redirect = logintoboggan_process_login($account, $edit, $redirect);
          call_user_func_array('drupal_goto', $redirect);
        }
        break;
      // Admin validation.
      case 'admin':
        if ($pre_auth) {
          // Mail the user, letting them know their account now has auth user perms.
          _user_mail_notify('status_activated', $account);
        }

        drupal_set_message(t('You have successfully validated %user.', array('%user' => $account->name)));
        drupal_goto("user/$account->uid/edit");
        break;
      // Catch all.
      default:
        drupal_set_message(t('You have successfully validated %user.', array('%user' => $account->name)));
        drupal_goto('');
        break;
    }
  }
  else {
    $message = t("Sorry, you can only use your validation link once for security reasons.");
    // No one currently logged in, go straight to user login page.
    if (empty($user->uid)) {
      $message .= t(" Please log in with your username and password instead now.");
      $goto = 'user/login';
    }
    else {
      $goto = 'user';
    }
    drupal_set_message($message, 'error');
    drupal_goto($goto);
  }
}

/**
 * Re-sends validation e-mail to user specified by $uid.
 */
function logintoboggan_resend_validation($account) {
  $account->password = t('If required, you may reset your password from: !url', array('!url' => url('user/password', array('absolute' => TRUE))));

  _user_mail_notify('register_no_approval_required', $account);

  // Notify admin or user that e-mail was sent and return to user edit form.
  if (user_access('administer users')) {
    drupal_set_message(t("A validation e-mail has been sent to the user's e-mail address."));
  }
  else {
    drupal_set_message(t('A validation e-mail has been sent to your e-mail address. You will need to follow the instructions in that message in order to gain full access to the site.'));
  }

  drupal_goto('user/'. $account->uid .'/edit');
}

