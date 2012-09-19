<?php

/**
 * @file
 *
 * Plugin to provide an argument handler for a Taxonomy term
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("User edit form: User ID"),
  // keyword to use for %substitution
  'keyword' => 'user',
  'description' => t('Creates a user edit form context from a user ID argument.'),
  'context' => 'ctools_user_edit_context',
  'placeholder form' => array(
    '#type' => 'textfield',
    '#description' => t('Enter the user ID for this argument.'),
  ),
);

/**
 * Discover if this argument gives us the term we crave.
 */
function ctools_user_edit_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('user_edit_form');
  }
  if(is_object($arg)){
    return ctools_context_create('user_edit_form', $arg);
  }
  if (!is_numeric($arg)) {
    return FALSE;
  }

  $account= user_load($arg);
  if (!$account) {
    return NULL;
  }

  // This will perform a node_access check, so we don't have to.
  return ctools_context_create('user_edit_form', $account);
  return NULL;
}