<?php
/**
 * @file
 * Chaos Tools plugin include using a plugin array to declare a plugin.
 */

/**
 * Plugin array plugin definition.
 */
$plugin = array(
  'function' => 'ctools_plugin_test_plugin_array2_not_cached_test',
  'handler' => array(
    'class' => 'ctoolsNotCachedPluginArray2',
  ),
);

/**
 * Plugin array function plugin.
 */
function ctools_plugin_test_plugin_array2_not_cached_test() {}
