<?php

/**
 * @file
 * This file contains drush integration for the varnish module.
 */

/**
 * Implements hook_drush_command().
 */
function varnish_drush_command() {
  $items = array();
  $items['varnish-purge-all'] = array(
    'description' => 'Purge all pages in varnish.',
    'drupal dependencies' => array('varnish'),
    'aliases' => array('vpa'),
    'callback' => 'varnish_purge_all_pages',
  );
  $items['varnish-purge'] = array(
    'description' => "Purge a list of URLs in varnish.",
    'arguments' => array(
      'paths' => 'A list of paths to purge separated by spaces. Regular expressions allowed.',
    ),
    'aliases' => array('vp'),
    'drupal dependencies' => array('varnish'),
    'callback' => 'varnish_drush_purge'
  );
  return $items;
}

/**
 * Callback for varnish-purge drush command.
 * @param string $paths a space separated list of paths.
 */
function varnish_drush_purge($paths) {
  $paths = explode(' ', $paths);
  varnish_expire_cache($paths);
}
