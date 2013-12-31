<?php

class menu_token_term_random implements menu_token_handler {
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
    $tid = db_select('taxonomy_term_data', 't')
        ->fields('t', array('tid'))
        ->orderRandom()
        ->range(0, 1)
        ->execute()
        ->fetchField(0);

    if ($tid) {
      return taxonomy_term_load($tid);
    }

    return FALSE;
  }
}
