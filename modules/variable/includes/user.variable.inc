<?php
/**
 * @file
 * Variable API module. Definition for Drupal core variables
 */

/**
 * User module variables
 */
function user_variable_info($options) {
  // $variables = array('#type' => 'fieldset', 'title' => t('User registration settings'));
  $variables['user_register'] = array(
    'type' => 'select', 
    'title' => t('Public registrations'),
    'default' => 1, 
    'options' => TRUE,
    'options callback' => 'user_variable_option_list',
    'group' => 'user_settings',
  );
  $variables['user_email_verification'] = array(
    'type' => 'boolean',
    'title' => t('Require e-mail verification when a visitor creates an account', array(), $options), 
    'default' => TRUE, 
    'description' => t('If this box is checked, new users will be required to validate their e-mail address prior to logging into the site, and will be assigned a system-generated password. With it unchecked, users will be logged in immediately upon registering, and may select their own passwords during registration.', array(), $options),
    'group' => 'user_settings',
  );
  $variables['user_registration_help'] = array(
    'type' => 'text', 
    'title' => t('User registration guidelines', array(), $options), 
    'default' => '', 
    'description' => t('This text is displayed at the top of the user registration form and is useful for helping or instructing your users.', array(), $options),
    'group' => 'user_settings',
  );

  // User e-mail settings.

  // These email tokens are shared for all settings, so just define
  // the list once to help ensure they stay in sync.
  $email_token_help = t('Available variables are: [site:name], [site:url], [user:name], [user:mail], [site:login-url], [site:url-brief], [user:edit-url], [user:one-time-login-url], [user:cancel-url].', array(), $options);

  $variables['user_mail_register_admin_created_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Welcome, new user created by administrator', array(), $options),
    'description' => t('Customize welcome e-mail messages sent to new member accounts created by an administrator.', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_register_no_approval_required_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Welcome, no approval required', array(), $options),
    'description' => t('Customize welcome e-mail messages sent to new members upon registering, when no administrator approval is required.', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_register_pending_approval_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Welcome, awaiting administrator approval', array(), $options),
    'description' => t('Customize welcome e-mail messages sent to new members upon registering, when administrative approval is required.', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_password_reset_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Password recovery email'),
    'description' => t('Customize e-mail messages sent to users who request a new password.') .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_status_activated_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Account activation email', array(), $options),
    'description' => t('Enable and customize e-mail messages sent to users upon account activation (when an administrator activates an account of a user who has already registered, on a site where administrative approval is required).', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_status_activated_notify'] = array(
    'type' => 'boolean',
    'title' => t('Notify user when account is activated.', array(), $options),
    'default' => TRUE,
    'group' => 'user_settings',
  );
  $variables['user_mail_status_blocked_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Account blocked email', array(), $options),
    'description' => t('Enable and customize e-mail messages sent to users when their accounts are blocked.') .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_status_blocked_notify'] = array(
    'type' => 'boolean',
    'title' => t('Notify user when account is blocked.', array(), $options),
    'default' => FALSE,
    'group' => 'user_settings',
  );
  $variables['user_mail_cancel_confirm_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Account cancellation confirmation'),
    'description' => t('Edit the e-mail messages sent to users when they attempt to cancel their accounts.', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_status_canceled_[mail_part]'] = array(
    'type' => 'user_mail',
    'title' => t('Account deleted email'),
    'description' => t('Enable and customize e-mail messages sent to users when their accounts are deleted.', array(), $options) .' '. $email_token_help,
    'group' => 'user_mails',
  );
  $variables['user_mail_status_deleted_notify'] = array(
    'type' => 'boolean',
    'title' => t('Notify user when account is deleted.', array(), $options),
    'default' => FALSE,
    'group' => 'user_settings',
  );
  // User signatures.
  $variables['user_signatures'] = array(
    'type' => 'enable',
    'title' => t('Signature support'),
    'default' => 0,
    'group' => 'user_settings',
  );

  // Picture support

  $picture_support = variable_get('user_pictures', 0);
  $form['user_pictures'] = array(
    'type' => 'enable',
    'title' => t('Picture support', array(), $options),
    'default' => 0,
    'group' => 'user_settings',
  );
  $variables['user_picture_path'] = array(
    'type' => 'file_path',
    'title' => t('Picture directory', array(), $options),
    'default' => 'pictures',
    'element' => array('#size' => 30, '#maxlength' => 255),
    'description' => t('Subdirectory in the file upload directory where pictures will be stored.', array(), $options),
    'group' => 'user_settings',
  );
  $variables['user_picture_default'] = array(
    'type' => 'url',
    'title' => t('Default picture', array(), $options),
    'default' => '',
    'description' => t('URL of picture to display for users with no custom picture selected. Leave blank for none.', array(), $options),
    'group' => 'user_settings',
  );
  $variables['user_picture_dimensions'] = array(
    'type' => 'string',
    'title' => t('Picture maximum dimensions', array(), $options),
    'default' => '85x85',
    'description' => t('Maximum dimensions for pictures, in pixels.', array(), $options),
    'element' => array('#size' => 15, '#maxlength' => 10),
    'group' => 'user_settings',
  );
  $variables['user_picture_file_size'] = array(
    'type' => 'number',
    'title' => t('Picture maximum file size', array(), $options),
    'default' => 30,
    'description' => t('Maximum file size for pictures, in kB.', array(), $options),
    'group' => 'user_settings',
  );
  $variables['user_picture_guidelines'] = array(
    'type' => 'text',
    'title' => t('Picture guidelines', array(), $options),
    'default' => '',
    'description' => t("This text is displayed at the picture upload form in addition to the default guidelines. It's useful for helping or instructing your users.", array(), $options),
    'group' => 'user_settings',
  );

  return $variables;
}

/**
 * Node module variable groups
 */
function user_variable_group_info() {
  $groups['user_settings'] = array(
    'title' => t('User settings'),
    'description' => t('User settings'),
    'access' => 'administer users',
    'path' => 'admin/user/settings',
  );
  $groups['user_mails'] = array(
    'title' => t('User emails'),
    'description' => t('User emails'),
    'access' => 'administer users',
    'path' => 'admin/user/settings',
  );
  return $groups;
}

/**
 * Implements hook_variable_type_info().
 */
function user_variable_type_info() {
  $types['user_mail'] = array(
    'title' => t('User mail text'),
    'type' => 'mail_text',
    'repeat' => array(
      'default callback' => 'user_variable_mail_default',
    ),
  );
  return $types;
}

/**
 * Default callback for user mail variables
 */
function user_variable_mail_default($variable, $options) {
  // Remove 'user_mail_' prefix 
  $name = substr($variable['name'], 10);
  return _user_mail_text($name, $options['language']);
}

/**
 * Implements hook_variable_option_list().
 */
function user_variable_option_list($variable, $options = array()) {
  switch ($variable['name']) {
    case 'user_register':
      return array(
        t('Only site administrators can create new user accounts.', array(), $options), 
        t('Visitors can create accounts and no administrator approval is required.', array(), $options), 
        t('Visitors can create accounts but administrator approval is required.', array(), $options)
      );
  }
}