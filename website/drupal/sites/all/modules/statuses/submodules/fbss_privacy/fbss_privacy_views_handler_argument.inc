<?php

/**
 * @file
 *   The handler for the Privacy argument.
 */

/**
 * Controls whether private statuses are included in the View results.
 */
class fbss_privacy_views_handler_argument extends views_handler_argument {
  function option_definition() {
    $options = parent::option_definition();
    $options['privacy'] = array('default' => 1);
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['privacy'] = array(
      '#type' => 'radios',
      '#default_value' => $this->options['privacy'],
      '#required' => TRUE,
      '#options' => array(
        0 => t('Show only non-private status messages'),
        1 => t('Show only private status messages'),
        'all' => t('Show private and non-private status messages'),
      ),
      '#description' => t('If you an option that shows private messages, only private status messages which the user is permitted to see will be shown.'),
    );
  }
  function query() {
    global $user;
    $argument = $this->argument;
    $privacy = $this->options['privacy'];
    $query = "{statuses}.private = %d";
    // The argument user must have participated in the status message.
    $this->query->add_where('fbss_privacy', db_prefix_tables("{statuses}.sender = %d OR ({statuses}.recipient = %d AND {facebook_status}.type = 'user')"), $argument, $argument);
    // Show only private or only non-private status messages.
    if (is_numeric($privacy)) {
      // Only show private messages if the current is the argument user or has admin permissions.
      if (!$privacy || $user->uid == $argument || user_access('view all private status messages')) {
        $this->query->add_where('fbss_privacy', db_prefix_tables($query), $privacy);
      }
      else {
        // Return no results.
        $this->query->add_where('fbss_privacy', db_prefix_tables("$query AND $query"), 0, 1);
      }
    }
    // Show private and non-private messages.
    elseif ($user->uid != $argument && !user_access('view all private status messages')) {
      // Return no results if the current user is not the argument user and has no admin permissions.
      $this->query->add_where('fbss_privacy', db_prefix_tables("$query AND $query"), 0, 1);
    }
  }
}
