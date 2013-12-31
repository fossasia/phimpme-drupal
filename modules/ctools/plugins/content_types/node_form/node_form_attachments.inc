<?php

if (module_exists('upload')) {
  /**
   * Plugins are described by creating a $plugin array which will be used
   * by the system that includes this file.
   */
  $plugin = array(
    'single' => TRUE,
    'icon' => 'icon_node_form.png',
    'title' => t('Node form file attachments'),
    'description' => t('File attachments on the Node form.'),
    'required context' => new ctools_context_required(t('Form'), 'node_form'),
    'category' => t('Form'),
  );
}

function ctools_node_form_attachments_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = t('Attach files');
  $block->delta = 'url-path-options';

  if (isset($context->form)) {
    if (isset($context->form['attachments'])) {
      $block->content = $context->form['attachments'];
      if (isset($block->content['attachments']['#group'])) {
        unset($block->content['attachments']['#pre_render']);
        unset($block->content['attachments']['#theme_wrappers']);
        $block->content['attachments']['#type'] = '';
      }
      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['attachments']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Attach files.');
  }
  return $block;
}

function ctools_node_form_attachments_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form attach files', array('@s' => $context->identifier));
}

function ctools_node_form_attachments_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
