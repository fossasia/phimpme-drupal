<?php

/**
 * @file
 * Rules specific functions that expose content_access' API.
 *
 * @todo
 * A way to enable per-node settings when a rule created, otherwise no effects
 *   will be noticed.
 * Clean-up function names.
 */

/**
 * Implements hook_rules_event_info().
 *
 * @ingroup rules
 */
function content_access_rules_rules_event_info() {
  $events['content_access_content_type'] = array('label' => t('Content type access control was changed'));
  $events['content_access_per_node'] = array('label' => t('Per node access control was changed'));

  if (module_exists('acl')) {
    $events['content_access_user_acl'] = array('label' => t('User was added to ACL'));
  }

  $items = array();
  foreach ($events as $name => $event) {
    $items[$name] = array(
      'label' => $event['label'],
      'group' => t('Content Access'),
    );
  }

  return $items;
}

/**
 * Implementation of hook_rules_action_info().
 *
 * @ingroup rules
 */
function content_access_rules_rules_action_info() {
  $role_actions = array(
    'content_access_action_grant_node_permissions' => array(
      'label' => t('Grant access by role'),
      'description' => t('Grant access to the following content'),
    ),
    'content_access_action_revoke_node_permissions' => array(
      'label' => t('Revoke access by role'),
      'description' => t('Revoke access to the following content'),
    ),
  );

  $reset_actions = array(
    'content_access_action_reset_node_permissions' => array(
      'label' => t('Reset access to content type defaults'),
      'description' => t('Reset node permissions to default permissions'),
    ),
  );

  $user_actions = array(
    'content_access_action_user_grant' => array(
      'label' => t('Grant access by user'),
      'description' => t('Grant access to the following content'),
      'ca_action' => 'Grant',
    ),
    'content_access_action_user_revoke' => array(
      'label' => t('Revoke access by user'),
      'description' => t('Revoke access to the following content'),
      'ca_action' => 'Revoke',
    ),
  );

  $items = array();
  foreach ($role_actions as $name => $action) {
    $items[$name] = array(
      'label' => $action['label'],
      'named parameter' => TRUE,
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'description' => $action['description'],
        ),
        'content_access' => array(
          'type' => 'list<text>',
          'label' => t('Role Based Access Control Settings'),
          'optional' => TRUE,
          'options list' => 'content_access_action_roles_permissions_list',
          'restriction' => 'input',
        ),
      ),
      'group' => t('Content Access Role'),
      'callbacks' => array(
        'form_alter' => 'content_access_action_form_alter',
      ),
    );
  }

  foreach ($reset_actions as $name => $action) {
    $items[$name] = array(
      'label' => $action['label'],
      'named parameter' => TRUE,
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'description' => $action['description'],
        ),
      ),
      'group' => t('Content Access Reset'),
    );
  }

  if (module_exists('acl')) {
    foreach ($user_actions as $name => $action) {
      $items[$name] = array(
        'label' => $action['label'],
        'named parameter' => TRUE,
        'parameter' => array(
          'node' => array(
            'type' => 'node',
            'label' => t('Content'),
            'description' => $action['description'],
          ),
          'content_access_user_view' => array(
            'type' => 'user',
            'label' => t('@action view access', array('@action' => $action['ca_action'])),
            'optional' => TRUE,
            'description' => t('@action view access to the following user.', array('@action' => $action['ca_action'])),
          ),
          'content_access_user_update' => array(
            'type' => 'user',
            'label' => t('@action update access', array('@action' => $action['ca_action'])),
            'optional' => TRUE,
            'description' => t('@action edit access to the following user.', array('@action' => $action['ca_action'])),
          ),
          'content_access_user_delete' => array(
            'type' => 'user',
            'label' => t('@action delete access', array('@action' => $action['ca_action'])),
            'optional' => TRUE,
            'description' => t('@action delete access to the following user.', array('@action' => $action['ca_action'])),
          ),
        ),
        'group' => t('Content Access User'),
      );
    }
  }

  return $items;
}

/**
 * Returns an array of operations used by the module.
 */
function content_access_action_roles_permissions_list() {
  return _content_access_roles_list();
}

/**
 * Helper function to return the array of available access operations.
 */
function _content_access_roles_list() {
  $defaults = array();
  $roles = array_map('filter_xss_admin', user_roles());

  // Make sure defaults are set properly
  foreach (_content_access_get_operations() as $op => $label) {
    foreach ($roles as $rid => $role) {
      $defaults[$op][$op . ':' . $role] = $role;
    }
  }

  return $defaults;
}

/**
 * Alter the settings form to render the text<list> as checkboxes, and to add
 * a submit callback to the form submit button.
 */
function content_access_action_form_alter(&$form, &$form_state) {
  // per node access control should be enabled for this type of action to be
  // effective
  drupal_set_message(t('Access control settings will not be executed for the affected node unless you enabled \'%per_node\' from the Access Control tab on content type page of the node', array('%per_node' => 'Per content node access control settings')), 'warning');

  // Alter the text<list> to make it into checkboxes groups
  $ca_elements =& $form['parameter']['content_access']['settings']['content_access'];
  $ca_elements = content_access_list_to_checkboxes($ca_elements);

  // Check and disable default roles.
  foreach ($ca_elements as $op => $select) {
    $ca_elements[$op]['#process'] = array('form_process_checkboxes', 'content_access_rules_disable_checkboxes');
  }

  // Add submit callback to process content_access_rules_role_based_form()
  // rules_ui_edit_element_submit is the default submit callback so we add it
  // again.
  // @todo I think this should go into Rules just like in FAPI
  $form['submit']['#submit'] = array('content_access_action_settings_form_submit', 'rules_ui_edit_element_submit');
}

/**
 * Settings form submit handler.
 *
 * Get the form settings as checkboxes, convert them back to list<text> so
 * Rules can understand and process them.
 */
function content_access_action_settings_form_submit($form, &$form_state) {
  $ca_element =& $form_state['input']['parameter']['content_access']['settings']['content_access'];
  $ca_element = content_access_checkboxes_to_list_submit($ca_element);

  $form_state['rules_element']->settings['content_access'] = $ca_element;
}

/**
 * Desc.
 */
function content_access_list_to_checkboxes($element) {
  $checkboxes = array();
  $operations = _content_access_get_operations();

  foreach ($operations as $op => $label) {
    $checkboxes[$op] = $element;
    $checkboxes[$op]['#title'] = t("@label", array('@label' => $label));
    $checkboxes[$op]['#type'] = 'checkboxes';
    $checkboxes[$op]['#options'] = $element['#options'][$op];
    unset($checkboxes[$op]['#default_value']);

    // Set defaults values of checkboxes
    foreach ($checkboxes[$op]['#options'] as $op_role => $permission) {
      if (isset($element['#default_value'][$op_role])) {
        $checkboxes[$op]['#default_value'][] = $op_role;
      }
    }
  }

  return $checkboxes;
}

/**
 * Desc.
 */
function content_access_checkboxes_to_list($element) {
  $list = $element[key($element)];
  $list['#title'] = 'Value';
  $list['#type'] = 'select';
  unset($list['#options']);

  foreach ($element as $op => $value) {
    $list['#options'][$op] = $value['#options'];
  }
  return $list;
}

/**
 * Desc.
 */
function content_access_checkboxes_to_list_submit($element) {
  $list = array();
  foreach (_content_access_roles_list() as $op => $op_roles) {
    foreach ($op_roles as $op_role => $permission) {
      if (isset($element[$op][$op_role]) && $element[$op][$op_role]) {
        $list[$op_role] = $permission;
      }
    }
  }
  return $list;
}

/**
 * Action implementation: Grant permissions for a node.
 */
function content_access_action_grant_node_permissions($params) {
  content_access_action_save($params, array('op' => 'grant'));
}

/**
 * Action implementation: Revoke permissions for a node.
 */
function content_access_action_revoke_node_permissions($params) {
  content_access_action_save($params, array('op' => 'revoke'));
}

/**
 * Action implementation: Reset permissions for a node.
 */
function content_access_action_reset_node_permissions($params) {
  content_access_delete_per_node_settings($params['node']);
  content_access_action_aquire_grants($params['node']);
  return array('node' => $params['node']);
}

/**
 * Execute grant or revoke action
 */
function content_access_action_save($params, $options) {
  if (!empty($params['node']->nid) && _content_access_rules_check_setting($params['node'])) {
    $settings = content_access_action_settings($params['content_access']);

    if ($options['op'] == 'revoke') {
      $ca_settings = array();
      foreach (content_access_action_settings() as $op => $value) {
        $ca_settings[$op] = array_diff(content_access_per_node_setting($op, $params['node']), $settings[$op]);
      }
      $settings = $ca_settings;
    }

    content_access_save_per_node_settings($params['node'], $settings);
    content_access_action_aquire_grants($params['node']);

    // A following node_save() updates the grants for us.
    return array('node' => $params['node']);
  }
}

/**
 * Verifies that per content settings are activated for the given node.
 */
function _content_access_rules_check_setting($node) {
  $type = $node->type;
  $settings = variable_get('content_access_settings', array());

  foreach ($settings as $setting) {
    if (isset($setting[$type]) && $setting[$type]) {
      return TRUE;
    }
  }

  // If we didn't find any settings in content access for this type return
  // false as we don't want to process it.
  rules_log(t("Can't set per content permissions for content type @type. Make sure to have per content settings activated for content types you want to alter access control for.", array('@type' => $node->type)), TRUE);
  return FALSE;
}

/**
 * Split the settings string into array.
 */
function content_access_action_settings($action_settings = array()) {
  $roles_ids = array_flip(user_roles());

  foreach (_content_access_get_operations() as $op => $label) {
    $settings[$op] = array();
  }

  foreach ($action_settings as $op_role => $role) {
    $op = substr($op_role, 0, strpos($op_role, ':'));
    $rid = $roles_ids[$role];
    $settings[$op][] = $rid;
  }

  return $settings;
}

/**
 * Action implementation: Grant user access.
 */
function content_access_action_user_grant($params) {
  content_access_action_user($params, 'grant');
}

/**
 * Action implementation: Revoke user access.
 */
function content_access_action_user_revoke($params) {
  content_access_action_user($params, 'revoke');
}

/**
 * Process Rule's param, and grant by the passed operation.
 */
function content_access_action_user($params, $type) {
  $ops = array('view', 'update', 'delete');
  $settings = array();
  $node = $params['node'];

  foreach ($ops as $op) {
    if ($params['content_access_user_' . $op]) {
      $settings[$op] = $params['content_access_user_' . $op]->uid;
    }
  }

  foreach ($settings as $op => $uid) {
    $acl_id = content_access_get_acl_id($node, $op);
    acl_node_add_acl($node->nid, $acl_id, (int) ($op == 'view'), (int) ($op == 'update'), (int) ($op == 'delete'), content_access_get_settings('priority', $node->type));

    db_delete('acl_user')
      ->condition('acl_id', $acl_id)
      ->condition('uid', $uid)
      ->execute();

    if ($type == 'grant') {
      db_insert('acl_user')
        ->fields(array(
          'acl_id' => $acl_id,
          'uid' => $uid,
        ))
        ->execute();
    }
  }

  content_access_action_aquire_grants($node);
}

/**
 * Apply the new grants to the affected node.
 */
function content_access_action_aquire_grants($node) {
  // node_save() does implement node_access_acquire_grants() so we don't want
  // to execute it again or we'll get a duplicated key exception
  if (!isset($node->op) ||
      (isset($node->op) && $node->op != 'Save')) {
    node_access_acquire_grants($node);
  }
}