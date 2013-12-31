<?php

/**
 * @file
 *   Administrative settings for the Statuses Twitter module.
 */

/**
 * The administrative settings form.
 */
function fbss_twitter_admin($form, &$form_state) {
  $form = array();
  $form['fbss_twitter_default'] = array(
    '#type' => 'radios',
    '#title' => t('Default Twitter option'),
    '#default_value' => variable_get('fbss_twitter_default', 'off'),
    '#required' => TRUE,
    '#options' => array(
      'on' => t('Always enabled by default'),
      'off' => t('Always disabled by default'),
      'on-user' => t('Let the user choose (enabled is default)'),
      'off-user' => t('Let the user choose (disabled is default)'),
      'disallow' => t('Do not allow posting to Twitter at all'),
    ),
    '#weight' => -70,
  );
  $form['fbss_twitter_select_account'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically set the first Twitter account added for use with status updates.'),
    '#description' => t('With this setting disabled, users must explicitly select a Twitter account to which to post status updates after connecting their Twitter account to their Drupal account.') . ' ' .
      t('With this setting enabled, the first Twitter account users connect to their Drupal account will be automatically chosen as the one to which status updates can be posted.') . ' ' .
      t('Users can always change which Twitter account they want to use (including setting it back to "none").') . ' ' .
      t('Changing this setting will not affect users who have already connected at least one Twitter account to their Drupal account.'),
    '#default_value' => variable_get('fbss_twitter_select_account', 0),
    '#weight' => -30,
  );
  return system_settings_form($form);
}

/**
 * Validate function for the Statuses settings form alter.
 */
function fbss_twitter_admin_validate(&$form, &$form_state) {
  $len = $form_state['values']['statuses_length'];
  if (($len > 140 || $len == 0) && variable_get('fbss_twitter_default', 'off') != 'disallow') {
    $message = t('These settings could allow users to attempt to post messages to Twitter which are too long for Twitter to handle.') . ' ' .
      t('In this situation, Statuses will attempt to truncate the tweet and include a link to view the full message on your site.') . ' ' .
      t('Occasionally it is not possible to get a link to the status message, in which case the tweet will simply be shortened to 140 characters by Twitter.');
    if ($len > 140) {
      $message = t('The maximum number of characters allowed in a status is set to a number above 140, and users can post status updates to Twitter.') . ' ' . $message;
    }
    else {
      $message = t('The maximum number of characters allowed in a status is set to 0 (unlimited), and users can post status updates to Twitter.') . ' ' . $message;
    }
    drupal_set_message($message, 'warning');
  }
}
