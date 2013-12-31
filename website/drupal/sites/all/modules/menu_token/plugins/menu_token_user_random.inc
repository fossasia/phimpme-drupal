<?php

class menu_token_user_random implements menu_token_handler {
  function form_options($options) {
    // Nothing to do here.
  }

  function form_submit($form, &$form_state) {
    // Nothing to do here.
  }

  function form_validate($form, &$form_state) {
    // Nothing to do here.
  }

  function form_alter(&$form, &$form_state) {
    // Nothing to do here.
  }

  function object_load($options) {
    $uid = db_select('users', 'u')
        ->fields('u', array('uid'))
        ->condition('uid', 0, '!=')
        ->orderRandom()
        ->range(0, 1)
        ->execute()
        ->fetchField(0);

    if ($uid) {
      return user_load($uid);
    }

    return FALSE;
  }
}
