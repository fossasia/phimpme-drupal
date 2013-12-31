<?php

/**
 * @file
 *   This file provides a CTools content type containing the author pane.
 */

/**
 * Implements hook_ctools_content_types().
 */
function statuses_statuses_stream_ctools_content_types() {
  return array(
    'single' => TRUE,
    'title' => t('Statuses Stream'),
    'icon' => 'icon_user.png',
    'description' => t('A stream of Statuses.'),
    'required context' => new ctools_context_required(t('Miscellaneous'), 'misc'), //@todo: Is this needed?
    'category' => t('Miscellaneous'),
  );
}

/**
 * Implements hook_content_type_render().
 */
function statuses_statuses_stream_content_type_render($subtype, $conf, $panel_args, $context) {
  $account = isset($context->data) ? clone $context->data : NULL;
  $block = new stdClass();
  $block->content = t('User information is currently unavailable.');
  if ($account) {
    $block->title = t("Stream");
    $block->content = theme('statuses_form_display');
  }
  return $block;
}

/**
 * Implements hook_content_type_admin_title().
 */
function statuses_statuses_stream_content_type_admin_title($subtype, $conf, $context) {
  return t('Stream');
}
