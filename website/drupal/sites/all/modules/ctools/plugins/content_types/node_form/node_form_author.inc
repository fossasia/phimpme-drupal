<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_node_form.png',
  'title' => t('Node form author information'),
  'description' => t('Author information on the Node form.'),
  'required context' => new ctools_context_required(t('Form'), 'node_form'),
  'category' => t('Form'),
);

function ctools_node_form_author_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = t('Authoring information');
  $block->delta = 'author-options';

  if (isset($context->form)) {
    if (!empty($context->form['author'])) {
      $block->content['author'] = $context->form['author'];
      if (isset($block->content['author']['#group'])) {
        unset($block->content['author']['#pre_render']);
        unset($block->content['author']['#theme_wrappers']);
        $block->content['author']['#type'] = '';
        $block->content['author']['name']['#size'] /= 2;
        $block->content['author']['date']['#size'] /= 2;
      }

      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['author']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Authoring information.');
  }
  return $block;
}

function ctools_node_form_author_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" node form publishing options', array('@s' => $context->identifier));
}

function ctools_node_form_author_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
