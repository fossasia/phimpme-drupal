<?php
/**
 * @file
 *
 * Plugin to provide a user_edit_form context
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('User edit form'),
  'description' => t('A user edit form.'),
  'context' => 'ctools_context_create_user_edit_form',
  'edit form' => 'ctools_context_user_edit_form_settings_form',
  'defaults' => array('uid' => ''),
  'keyword' => 'user_edit',
  'context name' => 'user_edit_form',
  'convert list' => 'ctools_context_user_edit_convert_list',
  'convert' => 'ctools_context_user_edit_convert',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' => t('Enter the user ID of a user for this argument:'),
  ),
);

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function ctools_context_create_user_edit_form($empty, $user = NULL, $conf = FALSE) {
  static $created;
  $context = new ctools_context(array('form', 'user_edit', 'user_form', 'user_edit_form', 'user', 'entity:user'));
  $context->plugin = 'user_edit_form';

  if ($empty || (isset($created) && $created)) {
    return $context;
  }
  $created = TRUE;

  if ($conf) {
    // In this case, $user is actually our $conf array.
    $uid = is_array($user) && isset($user['uid']) ? $user['uid'] : (is_object($user) ? $user->uid : 0);

    if (module_exists('translation')) {
      if ($translation = module_invoke('translation', 'user_uid', $uid, $GLOBALS['language']->language)) {
        $uid = $translation;
        $reload = TRUE;
      }
    }

    if (is_array($user) || !empty($reload)) {
      $user = user_load($uid);
    }
  }

  if (!empty($user)) {
    $form_id = 'user_profile_form';

    $form_state = array('want form' => TRUE, 'build_info' => array('args' => array($user)));

    $file = drupal_get_path('module', 'user') . '/user.pages.inc';
    require_once DRUPAL_ROOT . '/' . $file;
    // This piece of information can let other modules know that more files
    // need to be included if this form is loaded from cache:
    $form_state['build_info']['files'] = array($file);

    $form = drupal_build_form($form_id, $form_state);

    // Fill in the 'node' portion of the context
    $context->data     = $user;
    $context->title    = isset($user->name) ? $user->name : '';
    $context->argument = $user->uid;

    $context->form       = $form;
    $context->form_state = &$form_state;
    $context->form_id    = $form_id;
    $context->form_title = isset($user->name) ? $user->name : '';
    $context->restrictions['form'] = array('form');
    return $context;
  }
}

function ctools_context_user_edit_form_settings_form($form, &$form_state) {
  $conf = &$form_state['conf'];

  $form['user'] = array(
    '#title' => t('Enter the name or UID of a node'),
    '#type' => 'textfield',
    '#maxlength' => 512,
    '#autocomplete_path' => 'ctools/autocomplete/user',
    '#weight' => -10,
  );

  if (!empty($conf['uid'])) {
    $info = db_query('SELECT * FROM {user} WHERE uid = :uid', array(':uid' => $conf['uid']))->fetchObject();
    if ($info) {
      $link = l(t("'%name' [user id %uid]", array('%name' => $info->name, '%uid' => $info->uid)), "user/$info->uid", array('attributes' => array('target' => '_blank', 'title' => t('Open in new window')), 'html' => TRUE));
      $form['user']['#description'] = t('Currently set to !link', array('!link' => $link));
    }
  }

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $conf['uid'],
  );

  $form['set_identifier'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
    '#title' => t('Reset identifier to user name'),
    '#description' => t('If checked, the identifier will be reset to the user name of the selected user.'),
  );

  return $form;
}

/**
 * Validate a node.
 */
function ctools_context_user_edit_form_settings_form_validate($form, &$form_state) {
  // Validate the autocomplete
  if (empty($form_state['values']['uid']) && empty($form_state['values']['user'])) {
    form_error($form['user'], t('You must select a user.'));
    return;
  }

  if (empty($form_state['values']['user'])) {
    return;
  }

  $uid          = $form_state['values']['user'];
  $preg_matches = array();
  $match        = preg_match('/\[id: (\d+)\]/', $uid, $preg_matches);
  if (!$match) {
    $match = preg_match('/^id: (\d+)/', $uid, $preg_matches);
  }

  if ($match) {
    $uid = $preg_matches[1];
  }
  if (is_numeric($uid)) {
    $user = db_query('SELECT uid FROM {user} WHEREuid = :uid', array(':uid' => $uid))->fetchObject();
  }
  else {
    $user = db_query('SELECT uid FROM {user} WHERE LOWER(name) = LOWER(:name)', array(':name' => $uid))->fetchObject();
  }

  form_set_value($form['uid'], $user->uid, $form_state);
}
function ctools_context_user_edit_form_settings_form_submit($form, &$form_state) {
  if ($form_state['values']['set_identifier']) {
    $user = user_load($form_state['values']['uid']);
    $form_state['values']['identifier'] = $user->name;
  }

  // This will either be the value set previously or a value set by the
  // validator.
  $form_state['conf']['uid'] = $form_state['values']['uid'];
}

/**
 * Provide a list of ways that this context can be converted to a string.
 */
function ctools_context_user_edit_convert_list() {
  // Pass through to the "node" context convert list.
  $plugin = ctools_get_context('user');
  return ctools_context_user_convert_list();
}

/**
 * Convert a context into a string.
 */
function ctools_context_user_edit_convert($context, $type) {
  // Pass through to the "node" context convert list.
  $plugin = ctools_get_context('user');
  return ctools_context_user_convert($context, $type);
}
