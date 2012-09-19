<?php

/**
 * @file
 *  Generate content, taxonomy, menu, and users via drush framework.
 */

/**
 * Implementation of hook_drush_command().
 */
function devel_generate_drush_command() {
  $items['generate-users'] = array(
    'description' => 'Create users.',
    'arguments' => array(
      'number_users' => 'Number of users to generate.',
    ),
    'options' => array(
      'kill' => 'Delete all users before generating new ones.',
      'roles' => 'A comma delimited list of role IDs which should be granted to the new users. No need to specify authenticated user role.',
    ),
    'aliases' => array('genu'),
  );
  $items['generate-terms'] = array(
    'description' => 'Create terms in specified vocabulary.',
    'arguments' => array(
      'machine_name' => 'Vocabulary machine name into which new terms will be inserted.',
      'number_terms' => 'Number of terms to insert. Defaults to 10.',
    ),
    'options' => array(
      'kill' => 'Delete all terms in specified vocabulary before generating.',
      'feedback' => 'An integer representing interval for insertion rate logging. Defaults to 500',
    ),
    'aliases' => array('gent'),

  );
  $items['generate-vocabs'] = array(
    'description' => 'Create vocabularies.',
    'arguments' => array(
      'num_vocabs' => 'Number of vocabularies to create. Defaults to 1.',
    ),
    'options' => array(
      'kill' => 'Delete all vocabularies before generating.',
    ),
    'aliases' => array('genv'),
  );
  $items['generate-content'] = array(
    'description' => 'Create content.',
    'drupal dependencies' => array('devel_generate'),
    'arguments' => array(
      'number_nodes' => 'Number of nodes to generate.',
      'maximum_comments' => 'Maximum number of comments to generate.',
    ),
    'options' => array(
      'kill' => 'Delete all content before generating new content.',
      'types' => 'A comma delimited list of content types to create. Defaults to page,article.',
      'feedback' => 'An integer representing interval for insertion rate logging. Defaults to 500',
      'skip-fields' => 'A comma delimited list of fields to omit when generating random values',
      'languages' => 'A comma-separated list of language codes',
    ),
    'aliases' => array('genc'),
  );
  $items['generate-menus'] = array(
    'description' => 'Create menus and menu items.',
    'drupal dependencies' => array('devel_generate'), // Remove these once devel.module is moved down a directory. http://drupal.org/node/925246
    'arguments' => array(
      'number_menus' => 'Number of menus to generate. Defaults to 2.',
      'number_links' => 'Number of links to generate. Defaults to 50.',
      'max_depth' => 'Max link depth. Defaults to 3',
      'max_width' => 'Max width of first level of links. Defaults to 8.',
    ),
    'options' => array(
      'kill' => 'Delete all previously generated menus and links before generating new menus and links.',
    ),
    'aliases' => array('genm'),
  );
  return $items;
}


/**
 * Command callback. Generate a number of users.
 */
function drush_devel_generate_users($num_users = NULL) {
  if (drush_generate_is_number($num_users) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', t('Invalid number of users.'));
  }
  drush_generate_include_devel();
  $roles = drush_get_option('roles') ? explode(',', drush_get_option('roles')) : array();
  devel_create_users($num_users, drush_get_option('kill'), 0, $roles);
  drush_log(t('Generated @number users.', array('@number' => $num_users)), 'success');
}

/**
 * Command callback. Generate a number of terms in given vocabs.
 */
function drush_devel_generate_terms($vname = NULL, $num_terms = 10) {
  // Try to convert machine name to a vocab ID
  if (!$vocab = taxonomy_vocabulary_machine_name_load($vname)) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid vocabulary name: !name', array('!name' => $vname)));
  }
  if (drush_generate_is_number($num_terms) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of terms: !num', array('!num' => $num_terms)));
  }

  drush_generate_include_devel();
  $vocabs[$vocab->vid] = $vocab;
  devel_generate_term_data($vocabs, $num_terms, '12', drush_get_option('kill'));
  drush_log(dt('Generated @num_terms terms.', array('@num_terms' => $num_terms)), 'success');
}

/**
 * Command callback. Generate a number of vocabularies.
 */
function drush_devel_generate_vocabs($num_vocab = 1) {
  if (drush_generate_is_number($num_vocab) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of vocabularies: !num.', array('!num' => $num_vocab)));
  }
  drush_generate_include_devel();
  devel_generate_vocab_data($num_vocab, '12', drush_get_option('kill'));
  drush_log(dt('Generated @num_vocab vocabularies.', array('@num_vocab' => $num_vocab)), 'success');
}

/**
 * Command callback. Generate a number of nodes and comments.
 */
function drush_devel_generate_content($num_nodes = NULL, $max_comments = NULL) {
  if (drush_generate_is_number($num_nodes) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of nodes'));
  }
  if (!empty($max_comments) && drush_generate_is_number($max_comments) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of comments.'));
  }

  $add_language = drush_get_option('languages');
  if (!empty($add_language)) {
    $add_language = explode(',', str_replace(' ', '', $add_language));
    // Intersect with the enabled languages to make sure the language args
    // passed are actually enabled.
    $values['values']['add_language'] = array_intersect($add_language, array_keys(locale_language_list()));
  }

  // Load user 1; is needed for creating *published* comments.
  if ($max_comments) {
    global $user;
    $user_one = user_load(1);
    $user = $user_one;
    drupal_save_session(FALSE);
  }

  $values['values']['kill_content'] = drush_get_option('kill');
  $values['values']['title_length'] = 6;
  $values['values']['num_nodes'] = $num_nodes;
  $values['values']['max_comments'] = $max_comments;
  $values['values']['node_types'] = drupal_map_assoc(explode(',', drush_get_option('types', 'page,article')));
  drush_generate_include_devel();
  devel_generate_content($values);
  drush_log(t('Generated @num_nodes nodes, @max_comments comments (or less) per node.', array('@num_nodes' => (int)$num_nodes, '@max_comments' => (int)$max_comments)), 'success');
}

/**
 * Command callback. Generate a number of menus and menu links.
 */
function drush_devel_generate_menus($number_menus = 2, $number_links = 50, $max_depth = 3, $max_width = 8) {
  if (drush_generate_is_number($number_menus) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of menus'));
  }
  if (drush_generate_is_number($number_links) == FALSE) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid number of links'));
  }
  if (drush_generate_is_number($max_depth) == FALSE || $max_depth > 9 || $max_depth < 1) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid maximum link depth. Use a value between 1 and 9'));
  }
  if (drush_generate_is_number($max_width) == FALSE || $max_width < 1) {
    return drush_set_error('DEVEL_GENERATE_INVALID_INPUT', dt('Invalid maximum menu width. Use a positive numeric value.'));
  }

  global $user;
  $user_one = user_load(1);
  $user = $user_one;
  drupal_save_session(FALSE);

  $kill = drush_get_option('kill');
  drush_generate_include_devel();
  $link_types = drupal_map_assoc(array('node', 'front', 'external'));
  devel_generate_menu_data($number_menus, array(), $number_links, 12, $link_types, $max_depth, $max_width, $kill);
  drush_log(t('Generated @number_menus menus, @number_links links.', array('@number_menus' => (int)$number_menus, '@number_links' => (int)$number_links)), 'success');
}
//////////////////////////////////////////////////////////////////////////////
// Helper functions

// Verify if param is a number.
function drush_generate_is_number($number) {
  if ($number == NULL) return FALSE;
  if (!is_numeric($number)) return FALSE;
  return TRUE;
}

// Include devel_generate.inc.
function drush_generate_include_devel() {
  $path = drupal_get_path('module', 'devel_generate');
  require_once($path .'/devel_generate.inc');
}
