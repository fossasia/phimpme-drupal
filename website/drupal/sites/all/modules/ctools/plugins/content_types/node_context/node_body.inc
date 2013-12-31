<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('Node body'),
  'icon' => 'icon_node.png',
  'description' => t('The body of the referenced node.'),
  'required context' => new ctools_context_required(t('Node'), 'node'),
  'category' => t('Node'),
  'no ui' => TRUE,
);

/**
 * Render the custom content type.
 */
function ctools_node_body_content_type_render($subtype, $conf, $panel_args, $context) {
  $plugin = ctools_get_content_type('entity_field');
  $conf['formatter'] = 'text_default';
  $conf['formatter_settings'] = array();
  return $plugin['render callback']('node:body', $conf, $panel_args, $context);
}

/**
 * Returns an edit form for custom type settings.
 */
function ctools_node_body_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}

/**
 * Returns the administrative title for a type.
 */
function ctools_node_body_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" body', array('@s' => $context->identifier));
}
