<?php

/**
 * @file
 * Provides info about statistics.
 */

/**
 * Implements hook_entity_property_info_alter() on top of statistics module.
 *
 * @see entity_entity_property_info_alter()
 */
function entity_metadata_statistics_entity_property_info_alter(&$info) {
  $properties = &$info['node']['properties'];

  $properties['views'] = array(
    'label' => t("Number of views"),
    'description' => t("The number of visitors who have read the node."),
    'type' => 'integer',
    'getter callback' => 'entity_metadata_statistics_node_get_properties',
    'computed' => TRUE,
  );
  $properties['day_views'] = array(
    'label' => t("Views today"),
    'description' => t("The number of visitors who have read the node today."),
    'type' => 'integer',
    'getter callback' => 'entity_metadata_statistics_node_get_properties',
    'computed' => TRUE,
  );
  $properties['last_view'] = array(
    'label' => t("Last view"),
    'description' => t("The date on which a visitor last read the node."),
    'type' => 'date',
    'getter callback' => 'entity_metadata_statistics_node_get_properties',
    'computed' => TRUE,
  );
}
