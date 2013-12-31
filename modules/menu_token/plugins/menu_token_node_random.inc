<?php

class menu_token_node_random implements menu_token_handler {
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
    $nid = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->orderRandom()
        ->range(0, 1)
        ->execute()
        ->fetchField(0);

    if ($nid) {
      return node_load($nid);
    }

    return FALSE;
  }
}
