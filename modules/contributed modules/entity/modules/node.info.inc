<?php

/**
 * @file
 * Provides info about the node entity.
 */

/**
 * Implements hook_entity_property_info() on top of node module.
 *
 * @see entity_entity_property_info()
 */
function entity_metadata_node_entity_property_info() {
  $info = array();
  // Add meta-data about the basic node properties.
  $properties = &$info['node']['properties'];

  $properties['nid'] = array(
    'label' => t("Node ID"),
    'type' => 'integer',
    'description' => t("The unique ID of the node."),
    'schema field' => 'nid',
  );
  $properties['vid'] = array(
    'label' => t("Revision ID"),
    'type' => 'integer',
    'description' => t("The unique ID of the node's revision."),
    'schema field' => 'vid',
  );
  $properties['is_new'] = array(
    'label' => t("Is new"),
    'type' => 'boolean',
    'description' => t("Whether the node is new and not saved to the database yet."),
    'getter callback' => 'entity_metadata_node_get_properties',
  );
  $properties['type'] = array(
    'label' => t("Content type"),
    'type' => 'token',
    'description' => t("The type of the node."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'options list' => 'node_type_get_names',
    'required' => TRUE,
    'schema field' => 'type',
  );
  $properties['title'] = array(
    'label' => t("Title"),
    'description' => t("The title of the node."),
    'setter callback' => 'entity_property_verbatim_set',
    'schema field' => 'title',
    'required' => TRUE,
  );
  $properties['language'] = array(
    'label' => t("Language"),
    'description' => t("The language the node is written in."),
    'setter callback' => 'entity_property_verbatim_set',
    'options list' => 'entity_metadata_language_list',
    'schema field' => 'language',
    'setter permission' => 'administer nodes',
  );
  $properties['url'] = array(
    'label' => t("URL"),
    'description' => t("The URL of the node."),
    'getter callback' => 'entity_metadata_entity_get_properties',
    'type' => 'uri',
    'computed' => TRUE,
  );
  $properties['edit_url'] = array(
    'label' => t("Edit URL"),
    'description' => t("The URL of the node's edit page."),
    'getter callback' => 'entity_metadata_node_get_properties',
    'type' => 'uri',
    'computed' => TRUE,
  );
  $properties['status'] = array(
    'label' => t("Status"),
    'description' => t("Whether the node is published or unpublished."),
    // Although the status is expected to be boolean, its schema suggests
    // it is an integer, so we follow the schema definition.
    'type' => 'integer',
    'options list' => 'entity_metadata_status_options_list',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'schema field' => 'status',
  );
  $properties['promote'] = array(
    'label' => t("Promoted to frontpage"),
    'description' => t("Whether the node is promoted to the frontpage."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'schema field' => 'promote',
    'type' => 'boolean',
  );
  $properties['sticky'] = array(
    'label' => t("Sticky in lists"),
    'description' => t("Whether the node is displayed at the top of lists in which it appears."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'schema field' => 'sticky',
    'type' => 'boolean',
  );
  $properties['created'] = array(
    'label' => t("Date created"),
    'type' => 'date',
    'description' => t("The date the node was posted."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'schema field' => 'created',
  );
  $properties['changed'] = array(
    'label' => t("Date changed"),
    'type' => 'date',
    'schema field' => 'changed',
    'description' => t("The date the node was most recently updated."),
  );
  $properties['author'] = array(
    'label' => t("Author"),
    'type' => 'user',
    'description' => t("The author of the node."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer nodes',
    'required' => TRUE,
    'schema field' => 'uid',
  );
  $properties['source'] = array(
    'label' => t("Translation source node"),
    'type' => 'node',
    'description' => t("The original-language version of this node, if one exists."),
    'getter callback' => 'entity_metadata_node_get_properties',
  );
  $properties['log'] = array(
    'label' => t("Revision log message"),
    'type' => 'text',
    'description' => t("In case a new revision is to be saved, the log entry explaining the changes for this version."),
    'setter callback' => 'entity_property_verbatim_set',
    'access callback' => 'entity_metadata_node_revision_access',
  );
  $properties['revision'] = array(
    'label' => t("Creates revision"),
    'type' => 'boolean',
    'description' => t("Whether saving this node creates a new revision."),
    'setter callback' => 'entity_property_verbatim_set',
    'access callback' => 'entity_metadata_node_revision_access',
  );
  return $info;
}

/**
 * Implements hook_entity_property_info_alter() on top of node module.
 * @see entity_metadata_entity_property_info_alter()
 */
function entity_metadata_node_entity_property_info_alter(&$info) {
  // Move the body property to the node by default, as its usually there this
  // makes dealing with it more convenient.
  $info['node']['properties']['body'] = array(
    'type' => 'text_formatted',
    'label' => t('The main body text'),
    'getter callback' => 'entity_metadata_field_verbatim_get',
    'setter callback' => 'entity_metadata_field_verbatim_set',
    'property info' => entity_property_text_formatted_info(),
    'auto creation' => 'entity_property_create_array',
    'field' => TRUE,
  );
}
