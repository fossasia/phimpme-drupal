<?php

function _user_resource_definition() {
  $definition = array(
    'user' => array(
      'retrieve' => array(
        'help' => 'Retrieves a user',
        'callback' => '_user_resource_retrieve',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        'access callback' => '_user_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'uid',
            'type' => 'int',
            'description' => 'The uid of the user to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),

      'create' => array(
        'help' => 'Creates a user',
        'callback' => '_user_resource_create',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        'access callback' => '_user_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => FALSE,
        'args' => array(
          array(
            'name' => 'account',
            'type' => 'array',
            'description' => 'The user object',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'update' => array(
        'help' => 'Updates a user',
        'callback' => '_user_resource_update',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        'access callback' => '_user_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'uid',
            'type' => 'int',
            'description' => 'Unique identifier for this user',
            'source' => array('path' => 0),
            'optional' => FALSE,
          ),
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The user object with updated information',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'delete' => array(
        'help' => 'Deletes a user',
        'callback' => '_user_resource_delete',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        'access callback' => '_user_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'nid',
            'type' => 'int',
            'description' => 'The id of the note to delete',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        'callback' => '_user_resource_index',
        'args' => array(
          array(
            'name' => 'page',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'The zero-based index of the page to get, defaults to 0.',
            'default value' => 0,
            'source' => array('param' => 'page'),
          ),
          array(
            'name' => 'fields',
            'optional' => TRUE,
            'type' => 'string',
            'description' => 'The fields to get.',
            'default value' => '*',
            'source' => array('param' => 'fields'),
          ),
          array(
            'name' => 'parameters',
            'optional' => TRUE,
            'type' => 'array',
            'description' => 'Parameters',
            'default value' => array(),
            'source' => array('param' => 'parameters'),
          ),
          array(
            'name' => 'pagesize',
            'optional' => TRUE,
            'type' => 'init',
            'description' => 'Number of records to get per page.',
            'default value' => variable_get('services_user_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access arguments' => array('access user profiles'),
        'access arguments append' => FALSE,
      ),
      'actions' => array(
        'login' => array(
          'help' => 'Login a user for a new session',
          'callback' => '_user_resource_login',
          'args' => array(
            array(
              'name' => 'username',
              'type' => 'string',
              'description' => 'A valid username',
              'source' => array('data' => 'username'),
              'optional' => FALSE,
            ),
            array(
              'name' => 'password',
              'type' => 'string',
              'description' => 'A valid password',
              'source' => array('data' => 'password'),
              'optional' => FALSE,
            ),
          ),
          'access callback' => 'services_access_menu',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
        ),

        'logout' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/user_resource'),
          'help' => 'Logout a user session',
          'callback' => '_user_resource_logout',
          'access callback' => 'services_access_menu',
        ),
      ),
    ),
  );

  $definition['user']['actions']['register'] = array_merge($definition['user']['create'], array(
    'help' => 'Register a user',
  ));

  return $definition;
}

/**
 * Get user details.
 *
 * @param $uid
 *   UID of the user to be loaded.
 *
 * @return
 *   A user object.
 *
 * @see user_load()
 */
function _user_resource_retrieve($uid) {
  $account = user_load($uid);
  if (empty($account)) {
    return services_error(t('There is no user with ID @uid.', array('@uid' => $uid)), 404);
  }

  services_remove_user_data($account);

  // Everything went right.
  return $account;
}

/**
 * Create a new user.
 *
 * This function uses drupal_form_submit() and as such expects all input to match
 * the submitting form in question.
 *
 * @param $account
 *   A object containing account information. The $account object should
 *   contain, at minimum, the following properties:
 *     - name (user name)
 *     - mail (email address)
 *     - pass (plain text unencrypted password)
 *
 *   These properties can be passed but are optional
 *     - status (0 for blocked, otherwise will be active by default)
 *     - notify (1 to notify user of new account, will not notify by default)
 *
 *  Roles can be passed in a roles property which is an associative
 *  array formatted with '<role id>' => '<role id>', not including
 *  the authenticated user role, which is given by default.
 *
 * @return
 *   The user object of the newly created user.
 */
function _user_resource_create($account) {
  // Adds backwards compatability with regression fixed in #1083242
  $account = _services_arg_value($account, 'account');

  // Load the required includes for saving profile information
  // with drupal_form_submit().
  module_load_include('inc', 'user', 'user.pages');

  // register a new user
  $form_state['values'] = $account;
  $form_state['values']['pass'] = array(
    'pass1' => $account['pass'],
    'pass2' => $account['pass'],
  );
  $form_state['values']['op'] = variable_get('services_user_create_button_resource_create', t('Create new account'));

  // execute the register form
  drupal_form_submit('user_register_form', $form_state);
  // find and store the new user into the form_state
  if(isset($form_state['values']['uid'])) {
    $form_state['user'] = user_load($form_state['values']['uid']);
  }

  // Error if needed.
  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }
  else {
    $user = array('uid' => $form_state['user']->uid);
    if ($uri = services_resource_uri(array('user', $user['uid']))) {
      $user['uri'] = $uri;
    }
    return $user;
  }
}

/**
 * Update an existing user.
 *
 * This function uses drupal_form_submit() and as such expects all input to match
 * the submitting form in question.
 *
 * @param $uid
 *   Unique identifier for this user
 * @param $account
 *   Fields to modify for this user.
 *
 * @return
 *   The modified user object.
 */
function _user_resource_update($uid, $account) {
  // Adds backwards compatability with regression fixed in #1083242
  $account = _services_arg_value($account, 'data');

  $account['uid'] = $uid;

  $account_loaded = user_load($uid);

  // Load the required includes for saving profile information
  // with drupal_form_submit().
  module_load_include('inc', 'user', 'user.pages');

  // If a profile category was passed in, use it. Otherwise default
  // to 'account' (for saving core user data.)
  $category = 'account';
  if (isset($account['category'])) {
    $category = $account['category'];
    unset($account['category']);
  }

  // Drop any passed in values into the $account var. Anything
  // unused by the form just gets ignored. We handle roles and
  // password separately.
  foreach ($account as $key => $value) {
    if ($key != 'pass' && $key != 'roles') {
      $form_state['values'][$key] = $value;
    }
  }

  // Prepare values of roles.
  if (!isset($account['roles'])) {
    $account['roles'] = $account_loaded->roles;
  }
  foreach ($account['roles'] as $key => $value) {
    if (!empty($value)) {
      $form_state['values']['roles'][$key] = $key;
    }
  }
  unset($form_state['values']['roles'][2]);

  // Prepare values for password.
  if (isset($account['pass'])) {
    $form_state['values']['pass']['pass1'] = $account['pass'];
    $form_state['values']['pass']['pass2'] = $account['pass'];
  }

  $form_state['values']['op'] = variable_get('services_user_save_button_resource_update', t('Save'));
  $form_state['values']['#user_category'] = $category;
  $form_state['values']['#account'] = $account_loaded;

  $ret = drupal_form_submit('user_profile_form', $form_state, $account_loaded, $category);

  // Error if needed.
  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }
  else {
    services_remove_user_data($account);
    return $account;
  }
}

/**
 * Delete a user.
 *
 * @param $uid
 *   UID of the user to be deleted.
 *
 * @see user_delete()
 */
function _user_resource_delete($uid) {
  $account = user_load($uid);
  if (empty($account)) {
    return services_error(t('There is no user with ID @uid.', array('@uid' => $uid)), 404);
  }
  user_delete($uid);

  // Everything went right.
  return TRUE;
}

/**
 * Login a user using the specified credentials.
 *
 * Note this will transfer a plaintext password.
 *
 * @param $username
 *   Username to be logged in.
 * @param $password
 *   Password, must be plain text and not hashed.
 *
 * @return
 *   A valid session object.
 */
function _user_resource_login($username, $password) {
  global $user;

  if ($user->uid) {
    // user is already logged in
    return services_error(t('Already logged in as @user.', array('@user' => $user->name)), 406);
  }

  $uid = user_authenticate($username, $password);

  if ($uid) {
    $user = user_load($uid);
    if ($user->uid) {
      user_login_finalize();

      $return = new stdClass();
      $return->sessid = session_id();
      $return->session_name = session_name();

      services_remove_user_data($user);

      $return->user = $user;

      return $return;
    }
  }
  watchdog('user', 'Invalid login attempt for %username.', array('%username' => $username));
  return services_error(t('Wrong username or password.'), 401);
}

/**
 * Logout the current user.
 */
function _user_resource_logout() {
  global $user;

  if (!$user->uid) {
    // User is not logged in
    return services_error(t('User is not logged in.'), 406);
  }

  watchdog('user', 'Session closed for %name.', array('%name' => drupal_placeholder($user->name)));

  // Destroy the current session:
  session_destroy();
  module_invoke_all('user_logout', $user);

  // Load the anonymous user
  $user = drupal_anonymous_user();

  return TRUE;
}

/**
 * Return an array of optionally paged uids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/user?fields=uid,name,mail&parameters[uid]=1
 *
 * This would return an array of objects with only uid, name and mail defined,
 * where uid = 1.
 *
 * @param $page
 *   Page number of results to return (in pages of 20).
 * @param $fields
 *   The fields you want returned.
 * @param $parameters
 *   An array containing fields and values used to build a sql WHERE clause
 *   indicating items to retrieve.
 * @param $page_size
 *   Integer number of items to be returned.
 * @return
 *   An array of user objects.
 *
 * @see _node_resource_index() for more notes
 */
function _user_resource_index($page, $fields, $parameters, $page_size) {
  $user_select = db_select('users', 't')
    ->orderBy('created', 'DESC');

  services_resource_build_index_query($user_select, $page, $fields, $parameters, $page_size, 'user');

  $results = services_resource_execute_index_query($user_select);

  return services_resource_build_index_list($results, 'user', 'uid');
}

/**
 * Access check callback for user resource.
 */
function _user_resource_access($op = 'view', $args = array()) {
  // Adds backwards compatability with regression fixed in #1083242
  if (isset($args[0])) {
    $args[0] = _services_access_value($args[0], array('account', 'data'));
  }

  // Check if the user exists if appropriate.
  if ($op != 'create' && $op != 'register' ) {
    $account = user_load($args[0]);
    if (!$account) {
      return services_error(t('There is no user with ID @uid.', array('@uid' => $args[0])), 406);
    }
  }

  global $user;
  switch ($op) {
    case 'view':
      return user_view_access($account);
    case 'update':
      return ($user->uid == $account->uid || user_access('administer users'));
    case 'create':
    case 'register':
      if (!$user->uid && variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) != USER_REGISTER_ADMINISTRATORS_ONLY) {
        return TRUE;
      }
      else {
        return user_access('administer users');
      }
    case 'delete':
      return user_access('administer users');
  }
}
