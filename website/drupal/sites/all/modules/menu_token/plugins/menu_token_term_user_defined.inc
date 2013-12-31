<?php

class menu_token_term_user_defined implements menu_token_handler {
  function form_options($options) {
    $output['menu_token_term_user_defined'] = array(
      '#title' => t('Taxonomy term'),
      '#description' => t('The taxonomy term ID that this token handler should load.'),
      '#type' => 'textfield',
      '#default_value' => isset($options['tid']) ? $options : '',
    );

    return $output;
  }
  
  function form_submit($form, &$form_state) {
    $options['tid'] = $form_state['input']['menu_token_term_user_defined'];
    return $options;
  }

  function form_validate($form, &$form_state) {
    $tid = $form_state['input']['menu_token_term_user_defined'];
    if (!is_numeric($tid)) {
      form_set_error('menu_token_term_user_defined', t('Taxonomy term ID should be numeric.'));
      return;
    }
    if (!taxonomy_term_load($tid)) {
      form_set_error('menu_token_term_user_defined', t('Taxonomy term should exist.'));
      return;
    }
  }

  function form_alter(&$form, &$form_state) {
    // Nothing to do here.
  }

  function object_load($options) {
    if (is_numeric($options['tid'])) {
      return taxonomy_term_load($options['tid']);
    }

    return FALSE;
  }
}
