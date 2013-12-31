<?php

/**
 * @file
 * Views 3 support for Location Fax.
 */

/**
 * Implements hook_views_data().
 */
function location_fax_views_data() {
  // ----------------------------------------------------------------
  // location_fax table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.

  $data['location_fax']['table']['group'] = t('Location');

  // Join location_fax to all the tables that would be useful
  $data['location_fax']['table']['join'] = array(
    // location_fax links to location via lid.
    'location' => array(
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_fax links to node through location_instance via lid.
    'node' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_fax links to node_revision through location_instance via lid.
    'node_revision' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // location_fax links to users through location_instance via lid.
    'users' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
  );

  // ----------------------------------------------------------------
  // location_fax table -- fields

  $data['location_fax']['fax'] = array(
    'title' => t('Fax'),
    'help' => t('The fax number of the selected location.'),
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
