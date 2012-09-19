<?php

class menu_token_term_context implements menu_token_handler {
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
    if (arg(0) == 'taxonomy' && arg(1) == 'term' && is_numeric(arg(2))) {
      return taxonomy_term_load(arg(2));
    }

    return FALSE;
  }
}
