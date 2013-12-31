<?php

/**
 * @file
 *   Integrates Statuses Private Statuses with Views.
 */

/**
 * Implements hook_views_query_alter().
 */
function fbss_privacy_views_query_alter(&$view, &$query) {
  global $user;
  // Alter any view that shows status updates and exclude private statuses (unless we've already set a related filter).
  foreach ($query->table_queue as $table) {
    if ($table['table'] == 'statuses' && !isset($query->where['fbss_privacy']) && !user_access('view all private status messages')) {
      $query->add_where('fbss_privacy',
        $table['alias'] . '.private = 0 OR ' .
        $table['alias'] . '.sender = %d OR (' .
          $table['alias'] . ".type = 'user' AND " . $table['alias'] . ".recipient = %d)",
        $user->uid, $user->uid);
      break;
    }
  }
}

/**
 * Implements hook_views_data().
 */
function fbss_privacy_views_data() {
  $data = array();
  $data['statuses']['private'] = array(
    'title' => t('Show private status messages'),
    'help' => t('Enable showing private status messages in this view'),
    'field' => array(
      'handler' => 'fbss_privacy_views_handler_field',
      'title' => t('The word "<em>(Private)</em>" if the status message is private'),
      'help' => t('Distinguish between private and non-private messages'),
    ),
    'filter' => array(
      'handler' => 'fbss_privacy_views_handler_filter',
      'label' => t('Show private status messages'),
    ),
    'argument' => array(
      'handler' => 'fbss_privacy_views_handler_argument',
      'label' => t('Show private status messages'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'title' => t('Private messages first'),
      'help' => t('Choose "Descending" to float private messages to the top of the list.'),
    ),
  );
  return $data;
}

/**
 * Implements hook_views_handlers().
 */
function fbss_privacy_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'fbss_privacy'),
    ),
    'handlers' => array(
      'fbss_privacy_views_handler_argument' => array(
        'parent' => 'views_handler_argument',
      ),
      'fbss_privacy_views_handler_field' => array(
        'parent' => 'views_handler_field',
      ),
      'fbss_privacy_views_handler_filter' => array(
        'parent' => 'views_handler_filter',
      ),
    ),
  );
}
