<?php

class menu_token_user_context implements menu_token_handler {
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
    if (arg(0) == 'user' && is_numeric(arg(1))) {
      return user_load(arg(1));
    }

    return FALSE;
  }
}
