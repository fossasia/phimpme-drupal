<?php

if (module_exists('comment')) {
  /**
   * Plugins are described by creating a $plugin array which will be used
   * by the system that includes this file.
   */
  $plugin = array(
    'single' => TRUE,
    'icon' => 'icon_node_form.png',
    'title' => t('Node form comment settings'),
    'description' => t('Comment settings on the Node form.'),
    'required context' => new ctools_context_required(t('Form'), 'node_form'),
    'category' => t('Form'),
  );
}

function ctools_node_form_comment_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = t('Comment options');
  $block->delta = 'comment-options';

  if (isset($context->form)) {
    if (isset($context->form['comment_settings'])) {
      $block->content['comment_settings'] = $context->form['comment_settings'];
      unset($block->content['comment_settings']['#pre_render']);
      unset($block->content['comment_settings']['#theme_wrappers']);
      $block->content['comment_settings']['#type'] = '';

      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['comment_settings']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Comment options.');
  }
  return $block;
}

function ctools_node_form_comment_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form comment settings', array('@s' => $context->identifier));
}

function ctools_node_form_comment_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
