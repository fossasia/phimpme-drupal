<?php

class menu_token_user_user_defined implements menu_token_handler {
  function form_options($options) {
    $output['menu_token_user_user_defined'] = array(
      '#title' => t('User ID'),
      '#description' => t('The user ID of the user that this token handler should load.'),
      '#type' => 'textfield',
      '#default_value' => isset($options['uid']) ? $options : '',
    );

    return $output;
  }
  
  function form_submit($form, &$form_state) {
    $options['uid'] = $form_state['input']['menu_token_user_user_defined'];
    return $options;
  }

  function form_validate($form, &$form_state) {
    $uid = $form_state['input']['menu_token_user_user_defined'];
    if (!is_numeric($uid)) {
      form_set_error('menu_token_user_user_defined', t('User ID should be numeric.'));
      return;
    }
    if (!user_load($uid)) {
      form_set_error('menu_token_user_user_defined', t('User should exist.'));
      return;
    }
  }

  function form_alter(&$form, &$form_state) {
    // Nothing to do here.
  }

  function object_load($options) {
    if (is_numeric($options['uid'])) {
      return user_load($options['uid']);
    }

    return FALSE;
  }
}
