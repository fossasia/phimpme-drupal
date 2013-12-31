<?php

/**
 * @file Content access administration UI.
 */

/**
 * Specifies the threshold until we try to mass update node grants immediately.
 */
define('CONTENT_ACCESS_MASS_UPDATE_THRESHOLD', 1000);

/**
 * Per node settings page.
 */
function content_access_page($form, &$form_state, $node) {
  drupal_set_title(t('Access control for @title', array('@title' => $node->title)));

  foreach (_content_access_get_operations() as $op => $label) {
    $defaults[$op] = content_access_per_node_setting($op, $node);
  }

  // Get roles form
  content_access_role_based_form($form, $defaults, $node->type);

  // Add an after_build handler that disables checkboxes, which are enforced by permissions.
  $form['per_role']['#after_build'] = array('content_access_force_permissions');

  // ACL form
  if (module_exists('acl')) {
    // This is disabled when there is no node passed.
    $form['acl'] = array(
      '#type' => 'fieldset',
      '#title' => t('User access control lists'),
      '#description' => t('These settings allow you to grant access to specific users.'),
      '#collapsible' => TRUE,
      '#tree' => TRUE,
    );

    foreach (array('view', 'update', 'delete') as $op) {
      $acl_id = content_access_get_acl_id($node, $op);
      acl_node_add_acl($node->nid, $acl_id, (int) ($op == 'view'), (int) ($op == 'update'), (int) ($op == 'delete'), content_access_get_settings('priority', $node->type));

      $form['acl'][$op] = acl_edit_form($form_state, $acl_id, t('Grant !op access', array('!op' => $op)));
      $form['acl'][$op]['#collapsed'] = !isset($_POST['acl_' . $acl_id]) && !unserialize($form['acl'][$op]['user_list']['#default_value']);
    }
  }

  $form_state['node'] = $node;

  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#weight' => 10,
    '#submit' => array('content_access_page_reset'),
    '#access' => count(content_access_get_per_node_settings($node)) > 0,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );

  // @todo not true anymore?
  // http://drupal.org/update/modules/6/7#hook_node_access_records
  if (!$node->status) {
    drupal_set_message(t("Warning: Your content is not published, so this settings are not taken into account as long as the content remains unpublished."), 'error');
  }

  return $form;
}

/**
 * Submit callback for content_access_page().
 */
function content_access_page_submit($form, &$form_state) {
  $settings = array();
  $node = $form_state['node'];

  foreach (_content_access_get_operations() as $op => $label) {
    // Set the settings so that further calls will return this settings.
    $settings[$op] = array_keys(array_filter($form_state['values'][$op]));
  }

  // Save per-node settings.
  content_access_save_per_node_settings($node, $settings);

  if (module_exists('acl')) {
    foreach (array('view', 'update', 'delete') as $op) {
      acl_save_form($form_state['values']['acl'][$op]);
      module_invoke_all('user_acl', $settings);
    }
  }

  // Apply new settings.
  node_access_acquire_grants($node);

  module_invoke_all('per_node', $settings);

  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Submit callback for reset on content_access_page().
 */
function content_access_page_reset($form, &$form_state) {
  content_access_delete_per_node_settings($form_state['node']);
  node_access_acquire_grants($form_state['node']);

  drupal_set_message(t('The permissions have been reseted to the content type defaults.'));
}

/**
 * Per content type settings form.
 */
function content_access_admin_settings($form, $form_state, $content_type) {
  $type = $content_type->type;

  $form_state['type'] = $type;

  // Add role based per content type settings
  $defaults = array();
  foreach (_content_access_get_operations() as $op => $label) {
    $defaults[$op] = content_access_get_settings($op, $type);
  }
  content_access_role_based_form($form, $defaults, $type);

  // Per node:
  $form['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Per content node access control settings'),
    '#collapsible' => TRUE,
    '#description' => t('Optionally you can enable per content node access control settings. If enabled, a new tab for the content access settings appears when viewing content. You have to configure permission to access these settings at the !permissions page.', array('!permissions' => l(t('permissions'), 'admin/people/permissions'))),
  );
  $form['node']['per_node'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable per content node access control settings'),
    '#default_value' => content_access_get_settings('per_node', $type),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced']['priority'] = array(
    '#type' => 'weight',
    '#title' => t('Give content node grants priority'),
    '#default_value' => content_access_get_settings('priority', $type),
    '#description' => t('If you are only using this access control module, you can safely ignore this. If you are using multiple access control modules you can adjust the priority of this module.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Submit handler for per content type settings form.
 */
function content_access_admin_settings_submit($form, &$form_state) {
  $roles_permissions = user_role_permissions(user_roles());
  $permissions = user_permission_get_modules();

  // Remove disabled modules permissions, so they can't raise exception
  // in content_access_save_permissions()
  foreach ($roles_permissions as $rid => $role_permissions) {
    foreach ($role_permissions as $permission => $value) {
      if (!array_key_exists($permission, $permissions)) {
        unset($roles_permissions[$rid][$permission]);
      }
    }
  }

  foreach (array('update', 'update_own', 'delete', 'delete_own') as $op) {
    foreach ($form_state['values'][$op] as $rid => $value) {
      $permission = content_access_get_permission_by_op($op, $form_state['type']);
      if ($value) {
        $roles_permissions[$rid][$permission] = TRUE;
      }
      else {
        $roles_permissions[$rid][$permission] = FALSE;
      }
    }
    // Don't save the setting, so its default value (get permission) is applied always.
    unset($form_state['values'][$op]);
  }
  content_access_save_permissions($roles_permissions);

  // Update content access settings
  $settings = content_access_get_settings();
  foreach (content_access_available_settings() as $setting) {
    unset($settings[$setting][$form_state['type']]);
    if (isset($form_state['values'][$setting])) {
      $settings[$setting][$form_state['type']] = is_array($form_state['values'][$setting]) ? array_keys(array_filter($form_state['values'][$setting])) : $form_state['values'][$setting];
    }
  }
  content_access_set_settings($settings);

  $type = $form_state['type'];
  // Mass update the nodes, but only if necessary.
  if (content_access_get_settings('per_node', $type) ||
      content_access_get_settings('view', $type) != $form['per_role']['view']['#default_value'] ||
      content_access_get_settings('view_own', $type) != $form['per_role']['view_own']['#default_value'] ||
      content_access_get_settings('priority', $type) != $form['advanced']['priority']['#default_value'] ||
      content_access_get_settings('per_node', $type) != $form['node']['per_node']['#default_value']
     ) {

    // If per node has been disabled and we use the ACL integration, we have to remove possible ACLs now.
    if (!content_access_get_settings('per_node', $type) && $form['node']['per_node']['#default_value'] && module_exists('acl')) {
      _content_access_remove_acls($type);
    }

    if (content_access_mass_update(array($type))) {
      drupal_set_message(t('Permissions have been successfully rebuilt for the content type @types.', array('@types' => node_type_get_name($type))));
    }
  }

  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Mass updates node access records for nodes of the given types.
 * @param $types
 *   An array of content type names.
 * @return
 *   Whether the operation has been processed successfully (TRUE) or postponed (FALSE).
 */
function content_access_mass_update($types) {
  $q = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('n.type', $types, 'IN');

  $count = $q->countQuery()->execute()->fetchField();

  node_access_needs_rebuild(TRUE);

  // If there not too much nodes affected, try to do it.
  if ($count <= CONTENT_ACCESS_MASS_UPDATE_THRESHOLD) {
    $records = $q->execute();
    foreach ($records as $node) {
      node_access_acquire_grants(node_load($node->nid));
    }

    cache_clear_all();
    node_access_needs_rebuild(FALSE);
    return TRUE;
  }
  return FALSE;
}

/**
 * Saves the given permissions by role to the database.
 */
function content_access_save_permissions($roles_permissions) {
  foreach ($roles_permissions as $rid => $permissions) {
    user_role_change_permissions($rid, $permissions);
  }
}

/**
 * Builds the role based permission form for the given defaults.
 *
 * @param $defaults
 *   Array of defaults for all operations.
 */
function content_access_role_based_form(&$form, $defaults = array(), $type = NULL) {
  $form['per_role'] = array(
    '#type' => 'fieldset',
    '#title' => t('Role based access control settings'),
    '#collapsible' => TRUE,
    '#description' => t('Note that users need at least the %access_content permission to be able to deal in any way with content.', array('%access_content' => t('access content'))) .
      ' ' . t('Furthermore note that content which is not @published is treated in a different way by drupal: It can be viewed only by its author or users with the %administer_nodes permission.', array('@published' => t('published'), '%administer_nodes' => t('administer nodes'))),
  );

  $operations = _content_access_get_operations($type);
  $roles = array_map('filter_xss_admin', user_roles());
  foreach ($operations as $op => $label) {
    // Make sure defaults are set properly
    $defaults += array($op => array());

    $form['per_role'][$op] = array('#type' => 'checkboxes',
      '#prefix' => '<div class="content_access-div">',
      '#suffix' => '</div>',
      '#options' => $roles,
      '#title' => $label,
      '#default_value' => $defaults[$op],
      '#process' => array('form_process_checkboxes', 'content_access_disable_checkboxes'),
    );
  }

  $form['per_role']['clearer'] = array(
    '#value' => '<br clear="all" />',
  );

  drupal_add_css(drupal_get_path('module', 'content_access') . '/content_access.css');

  return $form;
}

/**
 * Formapi #after_build callback, that disables checkboxes for roles without access to content.
 */
function content_access_force_permissions($element, &$form_state) {
  foreach (array('update', 'update_own', 'delete', 'delete_own') as $op) {
    foreach (content_access_get_settings($op, $form_state['node']->type) as $rid) {
      $element[$op][$rid]['#disabled'] = TRUE;
      $element[$op][$rid]['#attributes']['disabled'] = 'disabled';
      $element[$op][$rid]['#value'] = TRUE;
      $element[$op][$rid]['#checked'] = TRUE;
      $element[$op][$rid]['#prefix'] = '<span title="' . t("Permission is granted due to the content type's access control settings.") . '">';
      $element[$op][$rid]['#suffix'] = "</span>";
    }
  }
  return $element;
}

/**
 * Submit callback for the user permissions form.
 * Trigger changes to node permissions to rebuild our grants.
 */
function content_access_user_admin_perm_submit($form, $form_state) {
  // Check for each content type, which has per node access activated
  // whether permissions have been changed.
  $types = array();
  foreach (array_filter(content_access_get_settings('per_node')) as $type => $value) {
    foreach (_content_access_get_node_permissions($type) as $perm) {
      foreach (user_roles() as $rid => $role) {
        if (isset($form_state['values'][$rid]) && in_array($perm, $form['checkboxes'][$rid]['#default_value']) != in_array($perm, $form_state['values'][$rid])) {
          //permission changed!
          $types[$type] = node_get_types('name', $type);
          continue 2;
        }
      }
    }
  }
  if ($types && content_access_mass_update(array_keys($types))) {
    drupal_set_message(format_plural(count($types),
      'Permissions have been successfully rebuilt for the content type @types.',
      'Permissions have been successfully rebuilt for the content types @types.',
      array('@types' => implode(', ', $types))
    ));
  }
}