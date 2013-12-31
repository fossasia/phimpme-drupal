<?php

/**
 * @file
 * Plugin to handle the 'page_actions' content type which allows the local
 * actions template variables to be embedded into a panel.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Actions'),
  'single' => TRUE,
  'icon' => 'icon_page.png',
  'description' => t('Add the action links (local tasks) as content.'),
  'category' => t('Page elements'),
  'render last' => TRUE,
);

/**
 * Output function for the 'page_actions' content type.
 *
 * Outputs the actions (local tasks) of the current page.
 */
function ctools_page_actions_content_type_render($subtype, $conf, $panel_args) {
  $block = new stdClass();
  $block->content = menu_local_actions();

  return $block;
}
