<?php

/**
 * @file
 * Publishing options form for the node. This contains the basic settings
 * like published, moderated, node revision, etc.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('Node form publishing options'),
  'icon' => 'icon_node_form.png',
  'description' => t('Publishing options on the Node form.'),
  'required context' => new ctools_context_required(t('Form'), 'node_form'),
  'category' => t('Form'),
);

function ctools_node_form_publishing_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();

  $block->title = t('Publishing options');
  $block->module = t('node_form');
  $block->delta = 'publishing-options';

  if (isset($context->form)) {
    if (isset($context->form['options'])) {
      $block->content['options'] = $context->form['options'];
      unset($block->content['options']['#pre_render']);
      unset($block->content['options']['#theme_wrappers']);
      $block->content['options']['#type'] = '';

      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['options']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Publishing options.');
  }
  return $block;
}

function ctools_node_form_publishing_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form author information', array('@s' => $context->identifier));
}

function ctools_node_form_publishing_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
