<?php

/**
 * @file rules integration for the user module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the user module.
 */
function rules_user_file_info() {
  return array('modules/user.eval');
}

/**
 * Implementation of hook_rules_event_info().
 */
function rules_user_event_info() {
  return array(
    'user_insert' => array(
      'label' => t('After saving a new user account'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('registered user')),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
    'user_update' => array(
      'label' => t('After updating an existing user account'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('updated user')),
        'account_unchanged' => array('type' => 'user', 'label' => t('unchanged user'), 'handler' => 'rules_events_entity_unchanged'),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
    'user_presave' => array(
      'label' => t('Before saving a user account'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('saved user'), 'skip save' => TRUE),
        'account_unchanged' => array('type' => 'user', 'label' => t('unchanged user'), 'handler' => 'rules_events_entity_unchanged'),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
    'user_view' => array(
      'label' => t('User account page is viewed'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('viewed user')),
        'view_mode' => array(
          'type' => 'text',
          'label' => t('view mode'),
          'options list' => 'rules_get_entity_view_modes',
        ),
      ),
      'access callback' => 'rules_user_integration_access',
      'help' => t("Note that if drupal's page cache is enabled, this event won't be generated for pages served from cache."),
    ),
    'user_delete' => array(
      'label' => t('After a user account has been deleted'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('deleted user')),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
    'user_login' => array(
      'label' => t('User has logged in'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('logged in user')),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
    'user_logout' => array(
      'label' => t('User has logged out'),
      'group' => t('User'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('logged out user')),
      ),
      'access callback' => 'rules_user_integration_access',
    ),
  );
}

/**
 * Options list for user cancel methods.
 * @todo: Use for providing a user_cancel action.
 */
function rules_user_cancel_methods() {
  module_load_include('inc', 'user', 'user.pages');
  foreach (user_cancel_methods() as $method => $form) {
    $methods[$method] = $form['#title'];
  }
  return $methods;
}

/**
 * User integration access callback.
 */
function rules_user_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'user');
  }
  // Else return admin access.
  return user_access('administer users');
}

/**
 * Implements hook_rules_condition_info() on behalf of the user module.
 */
function rules_user_condition_info() {
  return array(
    'user_has_role' => array(
      'label' => t('User has role(s)'),
      'parameter' => array(
        'account' => array('type' => 'user', 'label' => t('User')),
        'roles' => array(
          'type' => 'list<integer>',
          'label' => t('Roles'),
          'options list' => 'rules_user_roles_options_list',
        ),
        'operation' => array(
          'type' => 'text',
          'label' => t('Match roles'),
          'options list' => 'rules_user_condition_operations',
          'restriction' => 'input',
          'optional' => TRUE,
          'default value' => 'AND',
          'description' => t('If matching against all selected roles, the user must have <em>all</em> the roles selected.'),
        ),
      ),
      'group' => t('User'),
      'access callback' => 'rules_user_integration_access',
      'base' => 'rules_condition_user_has_role',
    ),
    'user_is_blocked' => array(
      'label' => t('User is blocked'),
      'parameter' => array(
        'account' => array('type' => 'user', 'label' => t('User')),
      ),
      'group' => t('User'),
      'access callback' => 'rules_user_integration_access',
      'base' => 'rules_condition_user_is_blocked',
    ),
  );
}

/**
 * User has role condition help callback.
 */
function rules_condition_user_has_role_help() {
  return t('Whether the user has the selected role(s).');
}

/**
 * Options list callback for the operation parameter of condition user has role.
 */
function rules_user_condition_operations() {
  return array(
    'AND' => t('all'),
    'OR' => t('any'),
  );
}

/**
 * Implements hook_rules_action_info() on behalf of the user module.
 */
function rules_user_action_info() {
  $defaults = array(
   'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'description' => t('The user whose roles should be changed.'),
        'save' => TRUE,
      ),
      'roles' => array(
        'type' => 'list<integer>',
        'label' => t('Roles'),
        'options list' => 'rules_user_roles_options_list',
      ),
    ),
    'group' => t('User'),
    'access callback' => 'rules_user_role_change_access',
  );
  $items['user_add_role'] = $defaults + array(
    'label' => t('Add user role'),
    'base' => 'rules_action_user_add_role',
  );
  $items['user_remove_role'] = $defaults + array(
    'label' => t('Remove user role'),
    'base' => 'rules_action_user_remove_role',
  );
  $defaults = array(
   'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'save' => TRUE,
      ),
    ),
    'group' => t('User'),
    'access callback' => 'rules_user_integration_access',
  );
  $items['user_block'] = $defaults + array(
    'label' => t('Block a user'),
    'base' => 'rules_action_user_block',
  );
  $items['user_unblock'] = $defaults + array(
    'label' => t('Unblock a user'),
    'base' => 'rules_action_user_unblock',
  );
  return $items;
}

/**
 * User integration role actions access callback.
 */
function rules_user_role_change_access() {
  return entity_metadata_user_roles() && user_access('administer permissions');
}

/**
 * Options list callback for user roles.
 */
function rules_user_roles_options_list($element) {
  return entity_metadata_user_roles('roles', array(), $element instanceof RulesConditionInterface ? 'view' : 'edit');
}

/**
 * @}
 */
