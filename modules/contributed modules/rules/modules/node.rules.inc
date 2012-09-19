<?php

/**
 * @file rules integration for the node module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the node module.
 */
function rules_node_file_info() {
  return array('modules/node.eval');
}

/**
 * Implements hook_rules_event_info() on behalf of the node module.
 */
function rules_node_event_info() {
  $items = array(
    'node_insert' => array(
      'label' => t('After saving new content'),
      'group' => t('Node'),
      'variables' => rules_events_node_variables(t('created content')),
      'access callback' => 'rules_node_integration_access',
    ),
    'node_update' => array(
      'label' => t('After updating existing content'),
      'group' => t('Node'),
      'variables' => rules_events_node_variables(t('updated content'), TRUE),
      'access callback' => 'rules_node_integration_access',
    ),
    'node_presave' => array(
      'label' => t('Before saving content'),
      'group' => t('Node'),
      'variables' => rules_events_node_variables(t('saved content'), TRUE),
      'access callback' => 'rules_node_integration_access',
    ),
    'node_view' => array(
      'label' => t('Content is viewed'),
      'group' => t('Node'),
      'help' => t("Note that if drupal's page cache is enabled, this event won't be generated for pages served from cache."),
      'variables' => rules_events_node_variables(t('viewed content')) + array(
        'view_mode' => array(
          'type' => 'text',
          'label' => t('view mode'),
          'options list' => 'rules_get_entity_view_modes',
          // Add the entity-type for the options list callback.
          'options list entity type' => 'node',
        ),
      ),
      'access callback' => 'rules_node_integration_access',
    ),
    'node_delete' => array(
      'label' => t('After deleting content'),
      'group' => t('Node'),
      'variables' => rules_events_node_variables(t('deleted content')),
      'access callback' => 'rules_node_integration_access',
    ),
  );
  // Specify that on presave the node is saved anyway.
  $items['node_presave']['variables']['node']['skip save'] = TRUE;
  return $items;
}

/**
 * Returns some parameter suitable for using it with a node
 */
function rules_events_node_variables($node_label, $update = FALSE) {
  $args = array(
    'node' => array('type' => 'node', 'label' => $node_label),
  );
  if ($update) {
    $args += array(
      'node_unchanged' => array(
        'type' => 'node',
        'label' => t('unchanged content'),
        'handler' => 'rules_events_entity_unchanged',
      ),
    );
  }
  return $args;
}

/**
 * Implements hook_rules_condition_info() on behalf of the node module.
 */
function rules_node_condition_info() {
  $defaults = array(
    'parameter' => array(
      'node' => array('type' => 'node', 'label' => t('Content')),
    ),
    'group' => t('Node'),
    'access callback' => 'rules_node_integration_access',
  );
  $items['node_is_of_type'] = $defaults + array(
    'label' => t('Content is of type'),
    'help' => t('Evaluates to TRUE if the given content is of one of the selected content types.'),
    'base' => 'rules_condition_node_is_of_type',
  );
  $items['node_is_of_type']['parameter']['type'] = array(
    'type' => 'list<text>',
    'label' => t('Content types'),
    'options list' => 'node_type_get_names',
    'description' => t('The content type(s) to check for.'),
    'restriction' => 'input',
  );
  $items['node_is_published'] = $defaults + array(
    'label' => t('Content is published'),
    'base' => 'rules_condition_node_is_published',
  );
  $items['node_is_sticky'] = $defaults + array(
    'label' => t('Content is sticky'),
    'base' => 'rules_condition_node_is_sticky',
  );
  $items['node_is_promoted'] = $defaults + array(
    'label' => t('Content is promoted to frontpage'),
    'base' => 'rules_condition_node_is_promoted',
  );
  return $items;
}

/**
 * Provides the content type of a node as asserted metadata.
 */
function rules_condition_node_is_of_type_assertions($element) {
  return array('node' => array('bundle' => $element->settings['type']));
}

/**
 * Implements hook_rules_action_info() on behalf of the node module.
 */
function rules_node_action_info() {
  $defaults = array(
    'parameter' => array(
      'node' => array('type' => 'node', 'label' => t('Content'), 'save' => TRUE),
    ),
    'group' => t('Node'),
    'access callback' => 'rules_node_admin_access',
  );
  // Add support for hand-picked core actions.
  $core_actions = node_action_info();
  $actions = array('node_publish_action', 'node_unpublish_action', 'node_make_sticky_action', 'node_make_unsticky_action', 'node_promote_action', 'node_unpromote_action');
  foreach ($actions as $base) {
    $action_name = str_replace('_action', '', $base);
    $items[$action_name] = $defaults + array(
      'label' => $core_actions[$base]['label'],
      'base' => $base,
    );
  }
  return $items;
}

/**
 * Node integration access callback.
 */
function rules_node_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'node');
  }
}

/**
 * Node integration admin access callback.
 */
function rules_node_admin_access() {
  return user_access('administer nodes');
}

/**
 * @}
 */
