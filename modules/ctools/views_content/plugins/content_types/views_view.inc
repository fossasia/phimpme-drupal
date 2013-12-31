<?php

/**
 * @file
 * Allow a view context to be displayed as whole.
 */

$plugin = array(
  'title' => t('Entire view'),
  'category' => t('View context'),
  'icon' => 'icon_views_page.png',
  'description' => t('Display the entire view.'),
  'required context' => new ctools_context_required(t('View'), 'view'),
);

/**
 * Render the views view content type.
 */
function views_content_views_view_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return;
  }

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'views_view';
  $block->delta   = $context->argument;
  $block->title   = '';
  $block->content = '';

  $output = views_content_context_get_output($context);
  $output = $output['view']->preview();
  $block->content = $output;

  return $block;
}

function views_content_views_view_content_type_edit_form($form, &$form_state) {
  // This form does nothing; it exists to let the main form select the view context.
  return $form;
}

function views_content_views_view_content_type_edit_form_submit(&$form, &$form_state) {
  // Kept so we guarantee we have a submit handler.
}

/**
 * Returns the administrative title for a type.
 */
function views_content_views_view_content_type_admin_title($subtype, $conf, $context) {
  return t('"@context" entire view', array('@context' => $context->identifier));
}
