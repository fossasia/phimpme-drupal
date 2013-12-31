<?php

/**
 * @file
 * Provides info about the comment entity.
 */

/**
 * Implements hook_entity_property_info() on top of comment module.
 *
 * @see entity_entity_property_info()
 */
function entity_metadata_comment_entity_property_info() {
  $info = array();
  // Add meta-data about the basic comment properties.
  $properties = &$info['comment']['properties'];

  $properties['cid'] = array(
    'label' => t("Comment ID"),
    'type' => 'integer',
    'description' => t("The unique ID of the comment."),
    'schema field' => 'cid',
  );
  $properties['hostname'] = array(
    'label' => t("IP Address"),
    'description' => t("The IP address of the computer the comment was posted from."),
    'schema field' => 'hostname',
  );
  $properties['name'] = array(
    'label' => t("Name"),
    'description' => t("The name left by the comment author."),
    'getter callback' => 'entity_metadata_comment_get_properties',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'sanitize' => 'filter_xss',
    'schema field' => 'name',
  );
  $properties['mail'] = array(
    'label' => t("Email address"),
    'description' => t("The email address left by the comment author."),
    'getter callback' => 'entity_metadata_comment_get_properties',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'validation callback' => 'valid_email_address',
    'schema field' => 'mail',
  );
  $properties['homepage'] = array(
    'label' => t("Home page"),
    'description' => t("The home page URL left by the comment author."),
    'sanitize' => 'filter_xss_bad_protocol',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'schema field' => 'homepage',
  );
  $properties['subject'] = array(
    'label' => t("Subject"),
    'description' => t("The subject of the comment."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'sanitize' => 'filter_xss',
    'required' => TRUE,
    'schema field' => 'subject',
  );
  $properties['url'] = array(
    'label' => t("URL"),
    'description' => t("The URL of the comment."),
    'getter callback' => 'entity_metadata_entity_get_properties',
    'type' => 'uri',
    'computed' => TRUE,
  );
  $properties['edit_url'] = array(
    'label' => t("Edit URL"),
    'description' => t("The URL of the comment's edit page."),
    'getter callback' => 'entity_metadata_comment_get_properties',
    'type' => 'uri',
    'computed' => TRUE,
  );
  $properties['created'] = array(
    'label' => t("Date created"),
    'description' => t("The date the comment was posted."),
    'type' => 'date',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'schema field' => 'created',
  );
  $properties['parent'] = array(
    'label' => t("Parent"),
    'description' => t("The comment's parent, if comment threading is active."),
    'type' => 'comment',
    'getter callback' => 'entity_metadata_comment_get_properties',
    'schema field' => 'pid',
  );
  $properties['node'] = array(
    'label' => t("Node"),
    'description' => t("The node the comment was posted to."),
    'type' => 'node',
    'setter callback' => 'entity_metadata_comment_setter',
    'setter permission' => 'administer comments',
    'required' => TRUE,
    'schema field' => 'nid',
  );
  $properties['author'] = array(
    'label' => t("Author"),
    'description' => t("The author of the comment."),
    'type' => 'user',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'required' => TRUE,
    'schema field' => 'uid',
  );
  $properties['status'] = array(
    'label' => t("Status"),
    'description' => t("Whether the comment is published or unpublished."),
    'setter callback' => 'entity_property_verbatim_set',
    // Although the status is expected to be boolean, its schema suggests
    // it is an integer, so we follow the schema definition.
    'type' => 'integer',
    'options list' => 'entity_metadata_status_options_list',
    'setter permission' => 'administer comments',
    'schema field' => 'status',
  );
  return $info;
}

/**
 * Implements hook_entity_property_info_alter() on top of comment module.
 * @see entity_entity_property_info_alter()
 */
function entity_metadata_comment_entity_property_info_alter(&$info) {
  // Add info about comment module related properties to the node entity.
  $properties = &$info['node']['properties'];
  $properties['comment'] = array(
    'label' => t("Comments allowed"),
    'description' => t("Whether comments are allowed on this node: 0 = no, 1 = closed (read only), 2 = open (read/write)."),
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer comments',
    'type' => 'integer',
  );
  $properties['comment_count'] = array(
    'label' => t("Comment count"),
    'description' => t("The number of comments posted on a node."),
    'getter callback' => 'entity_metadata_comment_get_node_properties',
    'type' => 'integer',
  );
  $properties['comment_count_new'] = array(
    'label' => t("New comment count"),
    'description' => t("The number of comments posted on a node since the reader last viewed it."),
    'getter callback' => 'entity_metadata_comment_get_node_properties',
    'type' => 'integer',
  );

  // The comment body field is usually available for all bundles, so add it
  // directly to the comment entity.
  $info['comment']['properties']['comment_body'] = array(
    'type' => 'text_formatted',
    'label' => t('The main body text'),
    'getter callback' => 'entity_metadata_field_verbatim_get',
    'setter callback' => 'entity_metadata_field_verbatim_set',
    'property info' => entity_property_text_formatted_info(),
    'field' => TRUE,
    'required' => TRUE,
  );
  unset($info['comment']['properties']['comment_body']['property info']['summary']);
}
