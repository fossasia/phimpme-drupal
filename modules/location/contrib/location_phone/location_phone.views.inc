<?php

/**
 * @file
 * Views 3 support for Location Phone.
 */

/**
 * Implements hook_views_data().
 */
function location_phone_views_data() {
  // ----------------------------------------------------------------
  // location_phone table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.

  $data['location_phone']['table']['group'] = t('Location');

  // Join location_phone to all the tables that would be useful
  $data['location_phone']['table']['join'] = array(
    // location_phone links to location via lid.
    'location' => array(
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_phone links to node through location_instance via lid.
    'node' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_phone links to node_revision through location_instance via lid.
    'node_revision' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_phone links to users through location_instance via lid.
    'users' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
  );

  // ----------------------------------------------------------------
  // location_phone table -- fields

  $data['location_phone']['phone'] = array(
    'title' => t('Phone'),
    'help' => t('The phone number of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'empty field name' => t('None'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
      'allow empty' => TRUE,
    ),
  );

  return $data;
}
