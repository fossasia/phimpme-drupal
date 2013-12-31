<?php

/**
 * @file
 * Plugin to provide access control based upon node (un)published status.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Node: (un)published"),
  'description' => t('Control access by the nodes published status.'),
  'callback' => 'ctools_node_status_ctools_access_check',
  'summary' => 'ctools_node_status_ctools_access_summary',
  'required context' => new ctools_context_required(t('Node'), 'node'),
);

/**
 * Check for access.
 */
function ctools_node_status_ctools_access_check($conf, $context) {
  return (!empty($context->data) && $context->data->status);
}

/**
 * Provide a summary description based upon the checked node_statuss.
 */
function ctools_node_status_ctools_access_summary($conf, $context) {
  return t('Returns true if the nodes status is "published".');
}

