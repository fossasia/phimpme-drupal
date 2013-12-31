<?php
// $Id$

/**
 * @file
 * Plugin to provide access control based upon entity bundle.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Entity: bundle"),
  'description' => t('Control access by entity bundle.'),
  'callback' => 'ctools_entity_bundle_ctools_access_check',
  'default' => array('type' => array()),
  'settings form' => 'ctools_entity_bundle_ctools_access_settings',
  'settings form submit' => 'ctools_entity_bundle_ctools_access_settings_submit',
  'summary' => 'ctools_entity_bundle_ctools_access_summary',
  'restrictions' => 'ctools_entity_bundle_ctools_access_restrictions',
  'get child' => 'ctools_entity_bundle_ctools_access_get_child',
  'get children' => 'ctools_entity_bundle_ctools_access_get_children',
);

function ctools_entity_bundle_ctools_access_get_child($plugin, $parent, $child) {
  $plugins = ctools_entity_bundle_ctools_access_get_children($plugin, $parent);
  return $plugins[$parent . ':' . $child];
}

function ctools_entity_bundle_ctools_access_get_children($plugin, $parent) {
  $entities = entity_get_info();
  $plugins = array();
  foreach ($entities as $entity_type => $entity) {
    $plugin['title'] = t('@entity: Bundle', array('@entity' => $entity['label']));
    $plugin['keyword'] = $entity_type;
    $plugin['description'] = t('Control access by @entity entity bundle.', array('@entity' => $entity_type));
    $plugin['name'] = $parent . ':' . $entity_type;
    $plugin['required context'] = new ctools_context_required(t(ucfirst($entity_type)), $entity_type);
    $plugins[$parent . ':' . $entity_type] = $plugin;
  }

  return $plugins;
}

/**
 * Settings form for the 'by entity_bundle' access plugin
 */
function ctools_entity_bundle_ctools_access_settings($form, &$form_state, $conf) {
  $plugin = $form_state['plugin'];
  $entity_type = explode(':', $plugin['name']);
  $entity_type = $entity_type[1];
  $entity = entity_get_info($entity_type);
  foreach ($entity['bundles'] as $type => $info) {
    $options[$type] = check_plain($info['label']);
  }

  $form['settings']['type'] = array(
    '#title' => t('Entity Bundle'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#description' => t('Only the checked entity bundles will be valid.'),
    '#default_value' => $conf['type'],
  );
  return $form;
}

/**
 * Compress the entity bundles allowed to the minimum.
 */
function ctools_entity_bundle_ctools_access_settings_submit($form, &$form_state) {
  $form_state['values']['settings']['type'] = array_filter($form_state['values']['settings']['type']);
}

/**
 * Check for access.
 */
function ctools_entity_bundle_ctools_access_check($conf, $context, $plugin) {
  list($plugin_name, $entity_type) = explode(':', $plugin['name']);
  if (!$entity_type) {
    return FALSE;
  };

  $entity = entity_get_info($entity_type);
  // As far as I know there should always be a context at this point, but this
  // is safe.
  if (empty($context) || empty($context->data) || empty($context->data->{$entity['entity keys']['bundle']})) {
    return FALSE;
  }

  if (array_filter($conf['type']) && empty($conf['type'][$context->data->{$entity['entity keys']['bundle']}])) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Inform the UI that we've eliminated a bunch of possibilities for this
 * context.
 */
function ctools_entity_bundle_ctools_access_restrictions($conf, &$context) {
  if (isset($context->restrictions['type'])) {
    $context->restrictions['type'] = array_unique(array_merge($context->restrictions['type'], array_keys(array_filter($conf['type']))));
  }
  else {
    $context->restrictions['type'] = array_keys(array_filter($conf['type']));
  }
}

/**
 * Provide a summary description based upon the checked entity_bundle.
 */
function ctools_entity_bundle_ctools_access_summary($conf, $context, $plugin) {
  if (!isset($conf['type'])) {
    $conf['type'] = array();
  }

  list($plugin_name, $entity_type) = explode(':', $plugin['name']);
  if (!$entity_type) {
    return t('Error, misconfigured entity_bundle access plugin');
  };

  $entity = entity_get_info($entity_type);

  $names = array();
  foreach (array_filter($conf['type']) as $type) {
    $names[] = check_plain($entity['bundles'][$type]['label']);
  }

  if (empty($names)) {
    return t('@identifier is any bundle', array('@identifier' => $context->identifier));
  }

  return format_plural(count($names), '@identifier is bundle "@types"', '@identifier bundle is one of "@types"', array('@types' => implode(', ', $names), '@identifier' => $context->identifier));
}

