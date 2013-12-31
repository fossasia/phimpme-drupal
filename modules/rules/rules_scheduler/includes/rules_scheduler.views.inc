<?php

/**
 * @file
 * Views integration for the rules scheduler module.
 */

/**
 * Implements hook_views_data(). Specifies the list of future scheduled
 * tasks displayed on the schedule page.
 */
function rules_scheduler_views_data() {
  $table = array(
    'rules_scheduler' => array(
      'table' => array(
        'group' => 'Rules scheduler',
        'base' => array(
          'field' => 'tid',
          'title' => t('Scheduled Rules components'),
          'help' => t("Scheduled Rules components that are executed based on time and cron"),
          'weight' => -10,
        ),
      ),
      'tid' => array(
        'title' => t('Tid'),
        'help' => t('The internal ID of the scheduled component'),
        'field' => array(
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
      ),
      'config' => array(
        'title' => t('Component name'),
        'help' => t('The name of the component'),
        'field' => array(
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'rules_scheduler_views_filter',
        ),
        'argument' => array(
          'handler' => 'views_handler_argument_string',
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
      ),
      'date' => array(
        'title' => t('Scheduled date'),
        'help' => t('Scheduled date and time stamp'),
        'field' => array(
          'handler' => 'views_handler_field_date',
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter',
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
      ),
      'identifier' => array(
        'title' => t('User provided identifier'),
        'help' => t('ID to recognize this specific scheduled task'),
        'field' => array(
          'click sortable' => TRUE,
        ),
        'filter' => array(
          'handler' => 'views_handler_filter',
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
      ),
    ),
  );
  return $table;
}
