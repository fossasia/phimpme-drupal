<?php

/**
 * @file
 * Hooks for flag actions.
 */

/**
 * Implementation of hook_trigger_info().
 */
function flag_trigger_info() {
  $hooks = array(
    'flag' => array(
      'flag_flag' => array(
        'label' => t('Content has been flagged with any flag'),
      ),
      'flag_unflag' => array(
        'label' => t('Content has been unflagged with any flag')
      ),
    ),
  );

  foreach (flag_get_flags() as $flag) {
    $hooks['flag']['flag_flag_'. $flag->name]['label'] = t('A %type has been flagged with %name', array('%type' => $flag->content_type, '%name' => $flag->name));
    $hooks['flag']['flag_unflag_'. $flag->name]['label'] = t('A %type has been unflagged with %name', array('%type' => $flag->content_type, '%name' => $flag->name));
  }

  return $hooks;
}

/**
 * Implementation of hook_action_info().
 */
function flag_action_info() {
  return array(
    'flag_node_action' => array(
      'type' => 'node',
      'label' => t('Flag (or unflag) a node'),
      'configurable' => TRUE,
      'triggers' => array(
        'node_presave', 'node_insert', 'node_update', 'node_delete', 'node_view',
        'comment_insert', 'comment_update', 'comment_delete', 'comment_view',
      ),
    ),
    'flag_comment_action' => array(
      'type' => 'comment',
      'label' => t('Flag (or unflag) a comment'),
      'configurable' => TRUE,
      'triggers' => array(
        'comment_insert', 'comment_update', 'comment_delete', 'comment_view',
      ),
    ),
    'flag_user_action' => array(
      'type' => 'user',
      'label' => t('Flag (or unflag) a user'),
      'configurable' => TRUE,
      'triggers' => array(
        'user_insert', 'user_update', 'user_delete', 'user_login', 'user_logout', 'user_view',
      ),
    ),
  );
}

/**
 * Implementation of hook_action_info_alter().
 *
 * Enable Flag actions on Node, Comment, and User hooks without
 * the trigger_unlock.module.
 */
function flag_action_info_alter(&$actions) {
  $node_flags = flag_get_flags('node');
  $comment_flags = flag_get_flags('comment');
  $user_flags = flag_get_flags('user');

  foreach ($actions as $name => $action) {
    if (strpos($name, 'node') === 0) {
      $actions[$name]['triggers'][] = 'flag_flag';
      $actions[$name]['triggers'][] = 'flag_unflag';
      foreach ($node_flags as $flag) {
        $actions[$name]['triggers'][] = 'flag_flag_' . $flag->name;
        $actions[$name]['triggers'][] = 'flag_unflag_' . $flag->name;
      }
    }
    if (strpos($name, 'comment') === 0) {
      $actions[$name]['triggers'][] = 'flag_flag';
      $actions[$name]['triggers'][] = 'flag_unflag';
      foreach ($comment_flags as $flag) {
        $actions[$name]['triggers'][] = 'flag_flag_' . $flag->name;
        $actions[$name]['triggers'][] = 'flag_unflag_' . $flag->name;
      }
    }
    if (strpos($name, 'user') === 0) {
      $actions[$name]['triggers'][] = 'flag_flag';
      $actions[$name]['triggers'][] = 'flag_unflag';
      foreach ($user_flags as $flag) {
        $actions[$name]['triggers'][] = 'flag_flag_' . $flag->name;
        $actions[$name]['triggers'][] = 'flag_unflag_' . $flag->name;
      }
    }
  }
}

/**
 * Implementation of a Drupal action. Flags a node.
 *
 * Note the first parameter is "object" because it may be a comment or a node.
 */
function flag_node_action(&$object, $context = array()) {
  if ($flag = flag_get_flag($context['flag_action']['flag'])) {
    $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
    $flag->flag($context['flag_action']['op'], $object->nid, $account, TRUE);
  }
}

/**
 * Form for configuring the Flag node action.
 */
function flag_node_action_form($context = array()) {
  return flag_action_form($context, 'node');
}

/**
 * Submit function for the Flag node action form.
 */
function flag_node_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Implementation of a Drupal action. Flags a comment.
 */
function flag_comment_action(&$comment, $context = array()) {
  if ($flag = flag_get_flag($context['flag_action']['flag'])) {
    $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
    $flag->flag($context['flag_action']['op'], $comment->cid, $account, TRUE);
  }
}

/**
 * Form for configuring the Flag comment action.
 */
function flag_comment_action_form($context) {
  return flag_action_form($context, 'comment');
}

/**
 * Submit function for the Flag comment action form.
 */
function flag_comment_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Implementation of a Drupal action. Flags a user.
 */
function flag_user_action(&$user, $context = array()) {
  if ($flag = flag_get_flag($context['flag_action']['flag'])) {
    $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
    $flag->flag($context['flag_action']['op'], $user->uid, $account, TRUE);
  }
}

/**
 * Form for configuring the Flag user action.
 */
function flag_user_action_form($context) {
  return flag_action_form($context, 'user');
}

/**
 * Submit function for the Flag user action form.
 */
function flag_user_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Generic form for configuring Flag actions.
 *
 * @param $context
 *   The current action context.
 * @param $content_type
 *   The content type applicable to this action, such as "node" or "comment".
 */
function flag_action_form($context, $content_type) {
  $form = array();

  $flags = flag_get_flags($content_type);
  // If this is a flag_action action, do not allow the triggering flag.
  if (isset($context['actions_flag'])) {
    unset($flags[$context['actions_flag']]);
  }
  $options = drupal_map_assoc(array_keys($flags));

  $form['flag_action']['#tree'] = TRUE;
  $form['flag_action']['warning'] = array(
    '#markup' => '<div class="messages status">' . t("Note when setting a flag through actions, the selected flag will be flagged regardless of the user's permissions.") . '</div>',
  );
  $form['flag_action']['flag'] = array(
    '#title' => t('Flag to affect'),
    '#type' => 'radios',
    '#options' => $options,
    '#required' => TRUE,
    '#description' => t('When this action is fired, which flag should be flagged (or unflagged)?'),
    '#default_value' => isset($context['flag_action']['flag']) ? $context['flag_action']['flag'] : reset($options),
  );

  $form['flag_action']['op'] = array(
    '#title' => t('Flag operation'),
    '#type' => 'radios',
    '#options' => array('flag' => t('Flag'), 'unflag' => t('Unflag')),
    '#description' => t('When this action is fired, which operation should be performed on the flag?'),
    '#default_value' => isset($context['flag_action']['op']) ? $context['flag_action']['op'] : 'flag',
  );

  if (empty($options)) {
    $error = t('There are no available %type flags. Before you can create an action of this type, you need to <a href="!url">create a %type flag</a>.', array('%type' => $content_type, '!url' => url(FLAG_ADMIN_PATH . '/add')));
    $form['flag_action']['flag']['#type'] = 'item';
    $form['flag_action']['flag']['#markup'] = $error;
    $form['flag_action']['flag']['#element_validate'][] = 'flag_action_validate_flag';
    $form['flag_action']['flag']['#flag_error'] = $error;
  }

  return $form;
}

/**
 * Generic validation handler for validating Flag action configuration.
 */
function flag_action_validate_flag($element) {
  if (isset($element['#flag_error'])) {
    form_error($element, $element['#flag_error']);
  }
}

/**
 * Generic submission handler for saving Flag action configuration.
 */
function flag_action_submit($form, $form_state) {
  return array(
    'flag_action' => $form_state['values']['flag_action'],
  );
}
