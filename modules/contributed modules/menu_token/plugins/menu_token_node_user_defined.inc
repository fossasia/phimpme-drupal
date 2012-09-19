<?php

class menu_token_node_user_defined implements menu_token_handler {
  function form_options($options) {
    $output['menu_token_node_user_defined'] = array(
      '#title' => t('Node ID'),
      '#description' => t('The node id of the node that this token handler should load.'),
      '#type' => 'textfield',
      '#default_value' => isset($options['nid']) ? $options : '',
    );

    return $output;
  }

  function form_submit($form, &$form_state) {
    $options['nid'] = $form_state['values']['menu_token_node_user_defined'];
    return $options;
  }

  function form_validate($form, &$form_state) {
    $nid = $form_state['values']['menu_token_node_user_defined'];
    if (!is_numeric($nid)) {
      form_set_error('menu_token_node_user_defined', t('Node ID should be numeric.'));
      return;
    }
    if (!node_load($nid)) {
      form_set_error('menu_token_node_user_defined', t('Node should exist.'));
      return;
    }
  }

  function form_alter(&$form, &$form_state) {
    // Nothing to do here.
  }

  function object_load($options) {
    if (is_numeric($options['nid'])) {
      return node_load($options['nid']);
    }

    return FALSE;
  }
}
