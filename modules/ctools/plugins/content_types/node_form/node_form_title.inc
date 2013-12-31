<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_node_form.png',
  'title' => t('Node form title field'),
  'description' => t('The node title form.'),
  'required context' => new ctools_context_required(t('Form'), 'node_form'),
  'category' => t('Form'),
);

function ctools_node_form_title_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->delta = 'title-options';

  if (isset($context->form)) {
    if (!empty($context->form['title'])) {
      $block->content['title'] = $context->form['title'];
      unset($context->form['title']);
    }
  }
  else {
    $block->content = t('Node title form.');
  }
  return $block;
}

function ctools_node_form_title_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form title field', array('@s' => $context->identifier));
}

function ctools_node_form_title_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
