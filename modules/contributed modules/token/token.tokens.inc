<?php

/**
 * @file
 * Token callbacks for the token module.
 */

/**
 * Implements hook_token_info_alter().
 */
function token_token_info_alter(&$info) {
  // Force 'date' type tokens to require input and add a 'current-date' type.
  // @todo Remove when http://drupal.org/node/943028 is fixed.
  $info['types']['date']['needs-data'] = 'date';
  $info['types']['current-date'] = array(
    'name' => t('Current date'),
    'description' => t('Tokens related to the current date and time.'),
    'type' => 'date',
  );

  // Add a 'dynamic' key to any tokens that have chained but dynamic tokens.
  $info['tokens']['date']['custom']['dynamic'] = TRUE;

  // The [file:size] may not always return in kilobytes.
  // @todo Remove when http://drupal.org/node/1193044 is fixed.
  $info['tokens']['file']['size']['description'] = t('The size of the file.');

  // Remove deprecated tokens from being listed.
  unset($info['tokens']['node']['tnid']);
  unset($info['tokens']['node']['type']);
  unset($info['tokens']['node']['type-name']);

  // Support 'url' type tokens for core tokens.
  if (isset($info['tokens']['comment']['url']) && module_exists('comment')) {
    $info['tokens']['comment']['url']['type'] = 'url';
  }
  $info['tokens']['node']['url']['type'] = 'url';
  if (isset($info['tokens']['term']['url']) && module_exists('taxonomy')) {
    $info['tokens']['term']['url']['type'] = 'url';
  }
  $info['tokens']['user']['url']['type'] = 'url';

  // Add [token:url] tokens for any URI-able entities.
  $entities = entity_get_info();
  foreach ($entities as $entity => $entity_info) {
    if (!isset($entity_info['token type'])) {
      continue;
    }

    $token_type = $entity_info['token type'];
    if (!isset($info['types'][$token_type]) || !isset($info['tokens'][$token_type])) {
      continue;
    }

    // Add [entity:url] tokens if they do not already exist.
    // @todo Support entity:label
    if (!isset($info['tokens'][$token_type]['url']) && !empty($entity_info['uri callback'])) {
      $info['tokens'][$token_type]['url'] = array(
        'name' => t('URL'),
        'description' => t('The URL of the @entity.', array('@entity' => drupal_strtolower($entity_info['label']))),
        'module' => 'token',
        'type' => 'url',
      );
    }
  }

  // Add support for custom date formats.
  // @todo Remove when http://drupal.org/node/1173706 is fixed.
  $date_format_types = system_get_date_types();
  foreach ($date_format_types as $date_format_type => $date_format_type_info) {
    if (!isset($info['tokens']['date'][$date_format_type])) {
      $info['tokens']['date'][$date_format_type] = array(
        'name' => $date_format_type_info['title'],
        'description' => t("A date in '@type' format. (%date)", array('@type' => $date_format_type, '%date' => format_date(REQUEST_TIME, $date_format_type))),
        'module' => 'token',
      );
    }
  }
}

/**
 * Implements hook_token_info().
 */
function token_token_info() {
  // Node tokens.
  $info['tokens']['node']['source'] = array(
    'name' => t('Translation source node'),
    'description' => t("The source node for this current node's translation set."),
    'type' => 'node',
  );
  $info['tokens']['node']['log'] = array(
    'name' => t('Revision log message'),
    'description' => t('The explanation of the most recent changes made to the node.'),
  );
  $info['tokens']['node']['content-type'] = array(
    'name' => t('Content type'),
    'description' => t('The content type of the node.'),
    'type' => 'content-type',
  );

  // Content type tokens.
  $info['types']['content-type'] = array(
    'name' => t('Content types'),
    'description' => t('Tokens related to content types.'),
    'needs-data' => 'node_type',
  );
  $info['tokens']['content-type']['name'] = array(
    'name' => t('Name'),
    'description' => t('The name of the content type.'),
  );
  $info['tokens']['content-type']['machine-name'] = array(
    'name' => t('Machine-readable name'),
    'description' => t('The unique machine-readable name of the content type.'),
  );
  $info['tokens']['content-type']['description'] = array(
    'name' => t('Description'),
    'description' => t('The optional description of the content type.'),
  );
  $info['tokens']['content-type']['node-count'] = array(
    'name' => t('Node count'),
    'description' => t('The number of nodes belonging to the content type.'),
  );
  $info['tokens']['content-type']['edit-url'] = array(
    'name' => t('Edit URL'),
    'description' => t("The URL of the content type's edit page."),
    //'type' => 'url',
  );

  // Taxonomy term and vocabulary tokens.
  if (module_exists('taxonomy')) {
    $info['tokens']['term']['edit-url'] = array(
      'name' => t('Edit URL'),
      'description' => t("The URL of the taxonomy term's edit page."),
      //'type' => 'url',
    );
    $info['tokens']['term']['parents'] = array(
      'name' => t('Parents'),
      'description' => t("An array of all the term's parents, starting with the root."),
      'type' => 'array',
    );
    $info['tokens']['term']['root'] = array(
      'name' => t('Root term'),
      'description' => t("The root term of the taxonomy term."),
      'type' => 'term',
    );

    $info['tokens']['vocabulary']['machine-name'] = array(
      'name' => t('Machine-readable name'),
      'description' => t('The unique machine-readable name of the vocabulary.'),
    );
    $info['tokens']['vocabulary']['edit-url'] = array(
      'name' => t('Edit URL'),
      'description' => t("The URL of the vocabulary's edit page."),
      //'type' => 'url',
    );
  }

  // File tokens.
  $info['tokens']['file']['extension'] = array(
    'name' => t('Extension'),
    'description' => t('The extension of the file.'),
  );
  $info['tokens']['file']['size-raw'] = array(
    'name' => t('File byte size'),
    'description' => t('The size of the file, in bytes.'),
  );

  // User tokens.
  // Add information on the restricted user tokens.
  $info['tokens']['user']['cancel-url'] = array(
    'name' => t('Account cancellation URL'),
    'description' => t('The URL of the confirm delete page for the user account.'),
    'restricted' => TRUE,
    //'type' => 'url',
  );
  $info['tokens']['user']['one-time-login-url'] = array(
    'name' => t('One-time login URL'),
    'description' => t('The URL of the one-time login page for the user account.'),
    'restricted' => TRUE,
    //'type' => 'url',
  );
  if (variable_get('user_pictures', 0)) {
    $info['tokens']['user']['picture'] = array(
      'name' => t('Picture'),
      'description' => t('The picture of the user.'),
      'type' => 'file',
    );
  }
  $info['tokens']['user']['roles'] = array(
    'name' => t('Roles'),
    'description' => t('The user roles associated with the user account.'),
    'type' => 'array',
  );

  // Current user tokens.
  $info['tokens']['current-user']['ip-address'] = array(
    'name' => t('IP address'),
    'description' => 'The IP address of the current user.',
  );

  // Menu link tokens (work regardless if menu module is enabled or not).
  $info['types']['menu-link'] = array(
    'name' => t('Menu links'),
    'description' => t('Tokens related to menu links.'),
    'needs-data' => 'menu-link',
  );
  $info['tokens']['menu-link']['mlid'] = array(
    'name' => t('Link ID'),
    'description' => t('The unique ID of the menu link.'),
  );
  $info['tokens']['menu-link']['title'] = array(
    'name' => t('Title'),
    'description' => t('The title of the menu link.'),
  );
  $info['tokens']['menu-link']['url'] = array(
    'name' => t('URL'),
    'description' => t('The URL of the menu link.'),
    'type' => 'url',
  );
  $info['tokens']['menu-link']['parent'] = array(
    'name' => t('Parent'),
    'description' => t("The menu link's parent."),
    'type' => 'menu-link',
  );
  $info['tokens']['menu-link']['parents'] = array(
    'name' => t('Parents'),
    'description' => t("An array of all the menu link's parents, starting with the root."),
    'type' => 'array',
  );
  $info['tokens']['menu-link']['root'] = array(
    'name' => t('Root'),
    'description' => t("The menu link's root."),
    'type' => 'menu-link',
  );

  // Current page tokens.
  $info['types']['current-page'] = array(
    'name' => t('Current page'),
    'description' => t('Tokens related to the current page request.'),
  );
  $info['tokens']['current-page']['title'] = array(
    'name' => t('Title'),
    'description' => t('The title of the current page.'),
  );
  $info['tokens']['current-page']['url'] = array(
    'name' => t('URL'),
    'description' => t('The URL of the current page.'),
    'type' => 'url',
  );
  $info['tokens']['current-page']['page-number'] = array(
    'name' => t('Page number'),
    'description' => t('The page number of the current page when viewing paged lists.'),
  );
  $info['tokens']['current-page']['query'] = array(
    'name' => t('Query string value'),
    'description' => t('The value of a specific query string field of the current page.'),
    'dynamic' => TRUE,
  );

  // URL tokens.
  $info['types']['url'] = array(
    'name' => t('URL'),
    'description' => t('Tokens related to URLs.'),
    'needs-data' => 'path',
  );
  $info['tokens']['url']['path'] = array(
    'name' => t('Path'),
    'description' => t('The path component of the URL.'),
  );
  $info['tokens']['url']['relative'] = array(
    'name' => t('Relative URL'),
    'description' => t('The relative URL.'),
  );
  $info['tokens']['url']['absolute'] = array(
    'name' => t('Absolute URL'),
    'description' => t('The absolute URL.'),
  );
  $info['tokens']['url']['brief'] = array(
    'name' => t('Brief URL'),
    'description' => t('The URL without the protocol and trailing backslash.'),
  );
  $info['tokens']['url']['unaliased'] = array(
    'name' => t('Unaliased URL'),
    'description' => t('The unaliased URL.'),
    'type' => 'url',
  );
  $info['tokens']['url']['args'] = array(
    'name' => t('Arguments'),
    'description' => t("The specific argument of the current page (e.g. 'arg:1' on the page 'node/1' returns '1')."),
    'type' => 'array',
  );

  // Array tokens.
  $info['types']['array'] = array(
    'name' => t('Array'),
    'description' => t('Tokens related to arrays of strings.'),
    'needs-data' => 'array',
  );
  $info['tokens']['array']['first'] = array(
    'name' => t('First'),
    'description' => t('The first element of the array.'),
  );
  $info['tokens']['array']['last'] = array(
    'name' => t('Last'),
    'description' => t('The last element of the array.'),
  );
  $info['tokens']['array']['count'] = array(
    'name' => t('Count'),
    'description' => t('The number of elements in the array.'),
  );
  $info['tokens']['array']['reversed'] = array(
    'name' => t('Reversed'),
    'description' => t('The array reversed.'),
    'type' => 'array',
  );
  $info['tokens']['array']['keys'] = array(
    'name' => t('Keys'),
    'description' => t('The array of keys of the array.'),
    'type' => 'array',
  );
  $info['tokens']['array']['join'] = array(
    'name' => t('Imploded'),
    'description' => t('The values of the array joined together with a custom string in-between each value.'),
    'dynamic' => TRUE,
  );
  $info['tokens']['array']['value'] = array(
    'name' => t('Value'),
    'description' => t('The specific value of the array.'),
    'dynamic' => TRUE,
  );

  // Random tokens.
  $info['types']['random'] = array(
    'name' => t('Random'),
    'description' => ('Tokens related to random data.'),
  );
  $info['tokens']['random']['number'] = array(
    'name' => t('Number'),
    'description' => t('A random number from 0 to @max.', array('@max' => mt_getrandmax())),
  );
  $info['tokens']['random']['hash'] = array(
    'name' => t('Hash'),
    'description' => t('A random hash. The possible hashing algorithms are: @hash-algos.', array('@hash-algos' => implode(', ', hash_algos()))),
    'dynamic' => TRUE,
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function token_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }

  $sanitize = !empty($options['sanitize']);

  // Date tokens.
  if ($type == 'date') {
    $date = !empty($data['date']) ? $data['date'] : REQUEST_TIME;

    // @todo Remove when http://drupal.org/node/1173706 is fixed.
    $date_format_types = system_get_date_types();
    foreach ($tokens as $name => $original) {
      if (isset($date_format_types[$name]) && _token_module('date', $name) == 'token') {
        $replacements[$original] = format_date($date, $name, '', NULL, $language_code);
      }
    }
  }

  // Current date tokens.
  // @todo Remove when http://drupal.org/node/943028 is fixed.
  if ($type == 'current-date') {
    $replacements += token_generate('date', $tokens, array('date' => REQUEST_TIME), $options);
  }

  // Comment tokens.
  if ($type == 'comment' && !empty($data['comment'])) {
    $comment = $data['comment'];

    // Chained token relationships.
    if (($url_tokens = token_find_with_prefix($tokens, 'url'))) {
      $replacements += token_generate('url', $url_tokens, entity_uri('comment', $comment), $options);
    }
  }

  // Node tokens.
  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'source':
          if (!empty($node->tnid) && $source_node = node_load($node->tnid)) {
            $title = $source_node->title;
            $replacements[$original] = $sanitize ? filter_xss($title) : $title;
          }
          break;
        case 'log':
          $replacements[$original] = $sanitize ? filter_xss($node->log) : $node->log;
          break;
        case 'content-type':
          $type_name = node_type_get_name($node);
          $replacements[$original] = $sanitize ? check_plain($type_name) : $type_name;
          break;
      }
    }

    // Chained token relationships.
    if (!empty($node->tnid) && ($source_tokens = token_find_with_prefix($tokens, 'source')) && $source_node = node_load($node->tnid)) {
      $replacements += token_generate('node', $source_tokens, array('node' => $source_node), $options);
    }
    if (($node_type_tokens = token_find_with_prefix($tokens, 'content-type')) && $node_type = node_type_load($node->type)) {
      $replacements += token_generate('content-type', $node_type_tokens, array('node_type' => $node_type), $options);
    }
    if (($url_tokens = token_find_with_prefix($tokens, 'url'))) {
      $replacements += token_generate('url', $url_tokens, entity_uri('node', $node), $options);
    }
  }

  // Content type tokens.
  if ($type == 'content-type' && !empty($data['node_type'])) {
    $node_type = $data['node_type'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'name':
          $replacements[$original] = $sanitize ? check_plain($node_type->name) : $node_type->name;
          break;
        case 'machine-name':
          // This is a machine name so does not ever need to be sanitized.
          $replacements[$original] = $node_type->type;
          break;
        case 'description':
          $replacements[$original] = $sanitize ? filter_xss($node_type->description) : $node_type->description;
          break;
        case 'node-count':
          $query = db_select('node');
          $query->condition('type', $node_type->type);
          $query->addTag('node_type_node_count');
          $count = $query->countQuery()->execute()->fetchField();
          $replacements[$original] = (int) $count;
          break;
        case 'edit-url':
          $replacements[$original] = url("admin/structure/types/manage/{$node_type->type}", $url_options);
          break;
      }
    }
  }

  // Taxonomy term tokens.
  if ($type == 'term' && !empty($data['term'])) {
    $term = $data['term'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'edit-url':
          $replacements[$original] = url("taxonomy/term/{$term->tid}/edit", $url_options);
          break;

        case 'parents':
          if ($parents = token_taxonomy_term_load_all_parents($term->tid)) {
            $replacements[$original] = token_render_array($parents, $options);
          }
          break;

        case 'root':
          $parents = taxonomy_get_parents_all($term->tid);
          $root_term = end($parents);
          if ($root_term->tid != $term->tid) {
            $replacements[$original] = $sanitize ? check_plain($root_term->name) : $root_term->name;
          }
          break;
      }
    }

    // Chained token relationships.
    if (($url_tokens = token_find_with_prefix($tokens, 'url'))) {
      $replacements += token_generate('url', $url_tokens, entity_uri('taxonomy_term', $term), $options);
    }
    // [term:parents:*] chained tokens.
    if ($parents_tokens = token_find_with_prefix($tokens, 'parents')) {
      if ($parents = token_taxonomy_term_load_all_parents($term->tid)) {
        $replacements += token_generate('array', $parents_tokens, array('array' => $parents), $options);
      }
    }
    if ($root_tokens = token_find_with_prefix($tokens, 'root')) {
      $parents = taxonomy_get_parents_all($term->tid);
      $root_term = end($parents);
      if ($root_term->tid != $term->tid) {
        $replacements += token_generate('term', $root_tokens, array('term' => $root_term), $options);
      }
    }
  }

  // Vocabulary tokens.
  if ($type == 'vocabulary' && !empty($data['vocabulary'])) {
    $vocabulary = $data['vocabulary'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'machine-name':
          // This is a machine name so does not ever need to be sanitized.
          $replacements[$original] = $vocabulary->machine_name;
          break;
        case 'edit-url':
          $replacements[$original] = url("admin/structure/taxonomy/{$vocabulary->machine_name}/edit", $url_options);
          break;
      }
    }
  }

  // File tokens.
  if ($type == 'file' && !empty($data['file'])) {
    $file = $data['file'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'extension':
          $extension = pathinfo($file->filename, PATHINFO_EXTENSION);
          $replacements[$original] = $sanitize ? check_plain($extension) : $extension;
          break;
        case 'size-raw':
          $replacements[$original] = (int) $file->filesize;
          break;
      }
    }
  }

  // User tokens.
  if ($type == 'user' && !empty($data['user'])) {
    $account = $data['user'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'picture':
          if (variable_get('user_pictures', 0)) {
            $replacements[$original] = theme('user_picture', array('account' => $account));
          }
          break;
        case 'roles':
          // The roles array may be set from checkbox values so ensure it always
          // has 'proper' data with the role names.
          $roles = array_intersect_key(user_roles(), $account->roles);
          $replacements[$original] = token_render_array($roles, $options);
          break;
      }
    }

    // Chained token relationships.
    if (variable_get('user_pictures', 0) && !empty($account->picture) && ($picture_tokens = token_find_with_prefix($tokens, 'picture'))) {
      // @todo Remove when core bug http://drupal.org/node/978028 is fixed.
      $account->picture->description = '';
      $replacements += token_generate('file', $picture_tokens, array('file' => $account->picture), $options);
    }
    if ($url_tokens = token_find_with_prefix($tokens, 'url')) {
      $replacements += token_generate('url', $url_tokens, entity_uri('user', $account), $options);
    }
    if ($role_tokens = token_find_with_prefix($tokens, 'roles')) {
      // The roles array may be set from checkbox values so ensure it always
      // has 'proper' data with the role names.
      $roles = array_intersect_key(user_roles(), $account->roles);
      $replacements += token_generate('array', $role_tokens, array('array' => $roles), $options);
    }
  }

  // Current user tokens.
  if ($type == 'current-user') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'ip-address':
          $ip = ip_address();
          $replacements[$original] = $sanitize ? check_plain($ip) : $ip;
          break;
      }
    }
  }

  // Menu link tokens.
  if ($type == 'menu-link' && !empty($data['menu-link'])) {
    $link = (array) $data['menu-link'];

    if (!isset($link['title'])) {
      // Re-load the link if it was not loaded via token_menu_link_load().
      $link = token_menu_link_load($link['mlid']);
    }

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'mlid':
          $replacements[$original] = $link['mlid'];
          break;
        case 'title':
          $replacements[$original] = $sanitize ? check_plain($link['title']) : $link['title'];
          break;
        case 'url':
          $replacements[$original] = url($link['href'], $url_options);
          break;
        case 'parent':
          if (!empty($link['plid']) && $parent = token_menu_link_load($link['plid'])) {
            $replacements[$original] = $sanitize ? check_plain($parent['title']) : $parent['title'];
          }
          break;
        case 'parents':
          if ($parents = token_menu_link_load_all_parents($link['mlid'])) {
            $replacements[$original] = token_render_array($parents, $options);
          }
          break;
        case 'root';
          if (!empty($link['p1']) && $link['p1'] != $link['mlid'] && $root = token_menu_link_load($link['p1'])) {
            $replacements[$original] = $sanitize ? check_plain($root['title']) : $root['title'];
          }
          break;
      }
    }

    // Chained token relationships.
    if (!empty($link['plid']) && ($source_tokens = token_find_with_prefix($tokens, 'parent')) && $parent = token_menu_link_load($link['plid'])) {
      $replacements += token_generate('menu-link', $source_tokens, array('menu-link' => $parent), $options);
    }
    // [menu-link:parents:*] chained tokens.
    if ($parents_tokens = token_find_with_prefix($tokens, 'parents')) {
      if ($parents = token_menu_link_load_all_parents($link['mlid'])) {
        $replacements += token_generate('array', $parents_tokens, array('array' => $parents), $options);
      }
    }
    if (!empty($link['p1']) && $link['p1'] != $link['mlid'] && ($root_tokens = token_find_with_prefix($tokens, 'root')) && $root = token_menu_link_load($link['p1'])) {
      $replacements += token_generate('menu-link', $root_tokens, array('menu-link' => $root), $options);
    }
    if ($url_tokens = token_find_with_prefix($tokens, 'url')) {
      $replacements += token_generate('url', $url_tokens, array('path' => $link['href']), $options);
    }
  }

  // Current page tokens.
  if ($type == 'current-page') {
    $current_path = current_path();

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'title':
          $title = drupal_get_title();
          $replacements[$original] = $sanitize ? $title : decode_entities($title);
          break;
        case 'url':
          $replacements[$original] = url($current_path, $url_options);
          break;
        case 'page-number':
          if ($page = filter_input(INPUT_GET, 'page')) {
            // @see PagerDefault::execute()
            $pager_page_array = explode(',', $page);
            $page = $pager_page_array[0];
          }
          $replacements[$original] = (int) $page + 1;
          break;
      }
    }

    // @deprecated
    // [current-page:arg] dynamic tokens.
    if ($arg_tokens = token_find_with_prefix($tokens, 'arg')) {
      foreach ($arg_tokens as $name => $original) {
        if (is_numeric($name) && ($arg = arg($name)) && isset($arg)) {
          $replacements[$original] = $sanitize ? check_plain($arg) : $arg;
        }
      }
    }

    // [current-page:query] dynamic tokens.
    if ($query_tokens = token_find_with_prefix($tokens, 'query')) {
      foreach ($query_tokens as $name => $original) {
        // @todo Should this use filter_input()?
        if (isset($_GET[$name])) {
          $replacements[$original] = $sanitize ? check_plain($_GET[$name]) : $_GET[$name];
        }
      }
    }

    // Chained token relationships.
    if ($url_tokens = token_find_with_prefix($tokens, 'url')) {
      $replacements += token_generate('url', $url_tokens, array('path' => $current_path), $options);
    }
  }

  // URL tokens.
  if ($type == 'url' && !empty($data['path'])) {
    $path = $data['path'];

    if (isset($data['options'])) {
      // Merge in the URL options if available.
      $url_options = $data['options'] + $url_options;
    }

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'path':
          $value = empty($url_options['alias']) ? drupal_get_path_alias($path, $language_code) : $path;
          $replacements[$original] = $sanitize ? check_plain($value) : $value;
          break;
        case 'alias':
          // @deprecated
          $alias = drupal_get_path_alias($path, $language_code);
          $replacements[$original] = $sanitize ? check_plain($alias) : $alias;
          break;
        case 'absolute':
          $replacements[$original] = url($path, $url_options);
          break;
        case 'relative':
          $replacements[$original] = url($path, array('absolute' => FALSE) + $url_options);
          break;
        case 'brief':
          $replacements[$original] = preg_replace(array('!^https?://!', '!/$!'), '', url($path, $url_options));
          break;
        case 'unaliased':
          $replacements[$original] = url($path, array('alias' => TRUE) + $url_options);
          break;
        case 'args':
          $value = empty($url_options['alias']) ? drupal_get_path_alias($path, $language_code) : $path;
          $replacements[$original] = token_render_array(arg(NULL, $value), $options);
          break;
      }
    }

    // [url:arg:*] chained tokens.
    if ($arg_tokens = token_find_with_prefix($tokens, 'args')) {
      $value = empty($url_options['alias']) ? drupal_get_path_alias($path, $language_code) : $path;
      $replacements += token_generate('array', $arg_tokens, array('array' => arg(NULL, $value)), $options);
    }

    // [url:unaliased:*] chained tokens.
    if ($unaliased_tokens = token_find_with_prefix($tokens, 'unaliased')) {
      $unaliased_token_data['path'] = $path;
      $unaliased_token_data['options'] = isset($data['options']) ? $data['options'] : array();
      $unaliased_token_data['options']['alias'] = TRUE;
      $replacements += token_generate('url', $unaliased_tokens, $unaliased_token_data, $options);
    }
  }

  // Entity tokens.
  if (!empty($data[$type]) && $entity_type = token_get_entity_mapping('token', $type)) {
    $entity = $data[$type];

    // Sometimes taxonomy terms are not properly loaded.
    // @see http://drupal.org/node/870528
    if ($entity_type == 'taxonomy_term' && !isset($entity->vocabulary_machine_name)) {
      $entity->vocabulary_machine_name = db_query("SELECT machine_name FROM {taxonomy_vocabulary} WHERE vid = :vid", array(':vid' => $entity->vid))->fetchField();
    }

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'url':
          if (_token_module($type, 'url') == 'token' && $uri = entity_uri($entity_type, $entity)) {
            $replacements[$original] = url($uri['path'], $uri['options']);
          }
          break;
      }
    }

    // Chained token relationships.
    if (($url_tokens = token_find_with_prefix($tokens, 'url')) && _token_module($type, 'url') == 'token') {
      $replacements += token_generate('url', $url_tokens, entity_uri($entity_type, $entity), $options);
    }

    // Pass through to an generic 'entity' token type generation.
    $entity_data = array(
      'entity_type' => $entity_type,
      'entity' => $entity,
      'token_type' => $type,
    );
    // @todo Investigate passing through more data like everything from entity_extract_ids().
    $replacements += token_generate('entity', $tokens, $entity_data, $options);
  }

  // Array tokens.
  if ($type == 'array' && !empty($data['array']) && is_array($data['array'])) {
    $array = $data['array'];

    $sort = isset($options['array sort']) ? $options['array sort'] : TRUE;
    $keys = element_children($array, $sort);

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'first':
          $value = $array[$keys[0]];
          $value = is_array($value) ? render($value) : (string) $value;
          $replacements[$original] = $sanitize ? check_plain($value) : $value;
          break;
        case 'last':
          $value = $array[$keys[count($keys) - 1]];
          $value = is_array($value) ? render($value) : (string) $value;
          $replacements[$original] = $sanitize ? check_plain($value) : $value;
          break;
        case 'count':
          $replacements[$original] = count($keys);
          break;
        case 'keys':
          $replacements[$original] = token_render_array($keys, $options);
          break;
        case 'reversed':
          $reversed = array_reverse($array, TRUE);
          $replacements[$original] = token_render_array($reversed, $options);
          break;
        case 'join':
          $replacements[$original] = token_render_array($array, array('join' => '') + $options);
          break;
      }
    }

    // [array:value:*] dynamic tokens.
    if ($value_tokens = token_find_with_prefix($tokens, 'value')) {
      foreach ($value_tokens as $key => $original) {
        if ($key[0] !== '#' && isset($array[$key])) {
          $replacements[$original] = token_render_array_value($array[$key], $options);
        }
      }
    }

    // [array:join:*] dynamic tokens.
    if ($join_tokens = token_find_with_prefix($tokens, 'join')) {
      foreach ($join_tokens as $join => $original) {
        $replacements[$original] = token_render_array($array, array('join' => $join) + $options);
      }
    }

    // [array:keys:*] chained tokens.
    if ($key_tokens = token_find_with_prefix($tokens, 'keys')) {
      $replacements += token_generate('array', $key_tokens, array('array' => $keys), $options);
    }

    // [array:reversed:*] chained tokens.
    if ($reversed_tokens = token_find_with_prefix($tokens, 'reversed')) {
      $replacements += token_generate('array', $reversed_tokens, array('array' => array_reverse($array, TRUE)), array('array sort' => FALSE) + $options);
    }

    // @todo Handle if the array values are not strings and could be chained.
  }

  // Random tokens.
  if ($type == 'random') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'number':
          $replacements[$original] = mt_rand();
          break;
      }
    }

    // [custom:hash:*] dynamic token.
    if ($hash_tokens = token_find_with_prefix($tokens, 'hash')) {
      $algos = hash_algos();
      foreach ($hash_tokens as $name => $original) {
        if (in_array($name, $algos)) {
          $replacements[$original] = hash($name, drupal_random_bytes(55));
        }
      }
    }
  }

  // If $type is a token type, $data[$type] is empty but $data[$entity_type] is
  // not, re-run token replacements.
  if (empty($data[$type]) && ($entity_type = token_get_entity_mapping('token', $type)) && $entity_type != $type && !empty($data[$entity_type]) && empty($options['recursive'])) {
    $data[$type] = $data[$entity_type];
    $options['recursive'] = TRUE;
    $replacements += module_invoke_all('tokens', $type, $tokens, $data, $options);
  }

  // If the token type specifics a 'needs-data' value, and the value is not
  // present in $data, then throw an error.
  if (!empty($GLOBALS['drupal_test_info']['test_run_id'])) {
    // Only check when tests are running.
    $type_info = token_get_info($type);
    if (!empty($type_info['needs-data']) && !isset($data[$type_info['needs-data']])) {
      trigger_error(t('Attempting to perform token replacement for token type %type without required data', array('%type' => $type)), E_USER_WARNING);
    }
  }

  return $replacements;
}

/**
 * Implements hook_tokens_alter().
 *
 * Fix existing core tokens that do not work correctly.
 */
function token_tokens_alter(array &$replacements, array $context) {
  $options = $context['options'];

  $sanitize = !empty($options['sanitize']);
  $langcode = !empty($options['language']->language) ? $options['language']->language : NULL;

  // Comment token fixes.
  if ($context['type'] == 'comment' && !empty($context['data']['comment'])) {
    $comment = $context['data']['comment'];

    foreach ($context['tokens'] as $name => $original) {
      switch ($name) {
        case 'name':
        case 'author':
          // @todo Remove when http://drupal.org/node/920056 is fixed.
          if (!empty($comment->uid)) {
            $account = user_load($comment->uid);
          }
          else {
            $account = drupal_anonymous_user();
            $account->name = $comment->name;
          }
          $name = format_username($account);
          $replacements[$original] = $sanitize ? check_plain($name) : $name;
          break;
      }
    }
  }

  // Node token fixes.
  if ($context['type'] == 'node' && !empty($context['data']['node'])) {
    $node = $context['data']['node'];

    foreach ($context['tokens'] as $name => $original) {
      switch ($name) {
        case 'author':
          // http://drupal.org/node/1185842 was fixed in core release 7.9.
          if (version_compare(VERSION, '7.9', '<')) {
            $account = user_load($node->uid);
            $name = format_username($account);
            $replacements[$original] = $sanitize ? check_plain($name) : $name;
          }
          break;
      }
    }
  }

  // File token fixes.
  if ($context['type'] == 'file' && !empty($context['data']['file'])) {
    $file = $context['data']['file'];

    foreach ($context['tokens'] as $name => $original) {
      switch ($name) {
        case 'owner':
          // http://drupal.org/node/978028 was fixed in core release 7.7.
          if (version_compare(VERSION, '7.7', '<')) {
            $account = user_load($file->uid);
            $name = format_username($account);
            $replacements[$original] = $sanitize ? check_plain($name) : $name;
          }
          break;
      }
    }
  }
}

/**
 * Implements hook_token_info() on behalf of book.module.
 */
function book_token_info() {
  $info['tokens']['node']['book'] = array(
    'name' => t('Book'),
    'description' => t('The book page associated with the node.'),
    'type' => 'menu-link',
  );
  return $info;
}

/**
 * Implements hook_tokens() on behalf of book.module.
 */
function book_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);

  // Node tokens.
  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    if (!empty($node->book['mlid'])) {
      $link = token_book_link_load($node->book['mlid']);

      foreach ($tokens as $name => $original) {
        switch ($name) {
          case 'book':
            $replacements[$original] = $sanitize ? check_plain($link['title']) : $link['title'];
            break;
        }
      }

      // Chained token relationships.
      if ($book_tokens = token_find_with_prefix($tokens, 'book')) {
        $replacements += token_generate('menu-link', $book_tokens, array('menu-link' => $link), $options);
      }
    }
  }

  return $replacements;
}

/**
 * Implements hook_token_info() on behalf of menu.module.
 */
function menu_token_info() {
  // Menu tokens.
  $info['types']['menu'] = array(
    'name' => t('Menus'),
    'description' => t('Tokens related to menus.'),
    'needs-data' => 'menu',
  );
  $info['tokens']['menu']['name'] = array(
    'name' => t('Name'),
    'description' => t("The name of the menu."),
  );
  $info['tokens']['menu']['machine-name'] = array(
    'name' => t('Machine-readable name'),
    'description' => t("The unique machine-readable name of the menu."),
  );
  $info['tokens']['menu']['description'] = array(
    'name' => t('Description'),
    'description' => t('The optional description of the menu.'),
  );
  $info['tokens']['menu']['menu-link-count'] = array(
    'name' => t('Menu link count'),
    'description' => t('The number of menu links belonging to the menu.'),
  );
  $info['tokens']['menu']['edit-url'] = array(
    'name' => t('Edit URL'),
    'description' => t("The URL of the menu's edit page."),
  );

  $info['tokens']['menu-link']['menu'] = array(
    'name' => t('Menu'),
    'description' => t('The menu of the menu link.'),
    'type' => 'menu',
  );
  $info['tokens']['menu-link']['edit-url'] = array(
    'name' => t('Edit URL'),
    'description' => t("The URL of the menu link's edit page."),
  );
  $info['tokens']['node']['menu-link'] = array(
    'name' => t('Menu link'),
    'description' => t("The menu link for this node."),
    'type' => 'menu-link',
  );

  return $info;
}

/**
 * Implements hook_tokens() on behalf of menu.module.
 */
function menu_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }

  $sanitize = !empty($options['sanitize']);

  // Node tokens.
  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'menu-link':
          if ($link = token_node_menu_link_load($node)) {
            $replacements[$original] = $sanitize ? check_plain($link['title']) : $link['title'];
          }
          break;
      }

      // Chained token relationships.
      if ($menu_tokens = token_find_with_prefix($tokens, 'menu-link')) {
        if ($link = token_node_menu_link_load($node)) {
          $replacements += token_generate('menu-link', $menu_tokens, array('menu-link' => $link), $options);
        }
      }
    }
  }

  // Menu link tokens.
  if ($type == 'menu-link' && !empty($data['menu-link'])) {
    $link = (array) $data['menu-link'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'menu':
          if ($menu = menu_load($link['menu_name'])) {
            $replacements[$original] = $sanitize ? check_plain($menu['title']) : $menu['title'];
          }
          break;
        case 'edit-url':
          $replacements[$original] = url("admin/structure/menu/item/{$link['mlid']}/edit", $url_options);
          break;
      }
    }

    // Chained token relationships.
    if (($menu_tokens = token_find_with_prefix($tokens, 'menu')) && $menu = menu_load($link['menu_name'])) {
      $replacements += token_generate('menu', $menu_tokens, array('menu' => $menu), $options);
    }
  }

  // Menu tokens.
  if ($type == 'menu' && !empty($data['menu'])) {
    $menu = (array) $data['menu'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'name':
          $replacements[$original] = $sanitize ? check_plain($menu['title']) : $menu['title'];
          break;
        case 'machine-name':
          // This is a machine name so does not ever need to be sanitized.
          $replacements[$original] = $menu['menu_name'];
          break;
        case 'description':
          $replacements[$original] = $sanitize ? filter_xss($menu['description']) : $menu['description'];
          break;
        case 'menu-link-count':
          $query = db_select('menu_links');
          $query->condition('menu_name', $menu['menu_name']);
          $query->addTag('menu_menu_link_count');
          $count = $query->countQuery()->execute()->fetchField();
          $replacements[$original] = (int) $count;
          break;
        case 'edit-url':
          $replacements[$original] = url("admin/structure/menu/manage/" . $menu['menu_name'], $url_options);
          break;
      }
    }
  }

  return $replacements;
}

/**
 * Implements hook_token_info() on behalf of profile.module.
 */
function profile_token_info() {
  $info = array();

  foreach (_token_profile_fields() as $field) {
    $info['tokens']['user'][$field->token_name] = array(
      'name' => check_plain($field->title),
      'description' => t('@category @type field.', array('@category' => drupal_ucfirst($field->category), '@type' => $field->type)),
    );

    switch ($field->type) {
      case 'date':
        $info['tokens']['user'][$field->token_name]['type'] = 'date';
        break;
    }
  }

  return $info;
}

/**
 * Implements hook_tokens() on behalf of profile.module.
 */
function profile_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);
  $language_code = isset($options['language']) ? $options['language']->language : NULL;

  if ($type == 'user' && !empty($data['user'])) {
    $account = $data['user'];

    // Load profile fields if this is the global user account.
    // @see http://drupal.org/node/361471
    // @see http://drupal.org/node/967330
    if ($account->uid == $GLOBALS['user']->uid && isset($account->timestamp)) {
      $profile_users = array($account->uid => $account);
      profile_user_load($profile_users);
      $account = $profile_users[$account->uid];
    }

    $profile_fields = _token_profile_fields();
    foreach ($tokens as $name => $original) {
      if (isset($profile_fields[$name]) && !empty($account->{$profile_fields[$name]->name})) {
        $value = $account->{$profile_fields[$name]->name};
        switch ($profile_fields[$name]->type) {
          case 'textarea':
            $replacements[$original] = $sanitize ? check_markup($value, filter_default_format($account), '', TRUE) : $value;
            break;
          case 'date':
            $timestamp = gmmktime(0, 0, 0, $value['month'], $value['day'], $value['year']);
            $replacements[$original] = format_date($timestamp, 'medium', '', NULL, $language_code);
            break;
          case 'url':
            $replacements[$original] = $sanitize ? check_url($value) : $value;
            break;
          case 'checkbox':
            // Checkbox field if checked should return the text.
            $replacements[$original] = $sanitize ? check_plain($profile_fields[$name]->title) : $profile_fields[$name]->title;
            break;
          case 'list':
            $value = preg_split("/[,\n\r]/", $value);
            $value = array_map('trim', $value);
            $value = implode(', ', $value);
            // Intentionally fall through to the default condition.
          default:
            $replacements[$original] = $sanitize ? check_plain($value) : $value;
            break;
        }
      }
    }

    // Chained token relationships.
    foreach ($profile_fields as $field) {
      if ($field->type == 'date' && isset($account->{$field->name}) && $field_tokens = token_find_with_prefix($tokens, $field->token_name)) {
        $date = $account->{$field->name};
        $replacements += token_generate('date', $field_tokens, array('date' => gmmktime(0, 0, 0, $date['month'], $date['day'], $date['year'])), $options);
      }
    }
  }

  return $replacements;
}

/**
 * Fetch an array of profile field objects, keyed by token name.
 */
function _token_profile_fields() {
  $fields = &drupal_static(__FUNCTION__);

  if (!isset($fields)) {
    $fields = array();
    $results = db_query("SELECT name, title, category, type FROM {profile_field}");
    foreach ($results as $field) {
      $field->token_name = token_clean_token_name($field->name);
      $fields[$field->token_name] = $field;
    }
  }

  return $fields;
}

/**
 * Fetch an array of field data used for tokens.
 */
function _token_field_info($field_name = NULL) {
  $info = &drupal_static(__FUNCTION__);

  if (!isset($fields)) {
    if ($cached = cache_get('field:info', 'cache_token')) {
      $info = $cached->data;
    }
    else {
      $info = array();

      $fields = field_info_fields();
      $instances = field_info_instances();
      $type_info = field_info_field_types();
      $entity_info = entity_get_info();

      foreach ($fields as $field) {
        $key = $field['field_name'];
        if (!empty($field['bundles'])) {
          foreach (array_keys($field['bundles']) as $entity) {
            // Make sure a token type exists for this entity.
            $token_type = token_get_entity_mapping('entity', $entity);
            if (empty($token_type)) {
              continue;
            }

            $info[$key]['token types'][] = $token_type;
            $info[$key] += array('labels' => array(), 'bundles' => array());

            // Find which label is most commonly used.
            foreach ($field['bundles'][$entity] as $bundle) {
              // Field information will included fields attached to disabled
              // bundles, so check that the bundle exists before provided a
              // token for it.
              // @see http://drupal.org/node/1252566
              if (!isset($entity_info[$entity]['bundles'][$bundle])) {
                continue;
              }

              $info[$key]['labels'][] = $instances[$entity][$bundle][$key]['label'];
              $info[$key]['bundles'][$token_type][$bundle] = $entity_info[$entity]['bundles'][$bundle]['label'];
            }
          }
        }

        if (isset($info[$key])) {
          $labels = array_count_values($info[$key]['labels']);
          arsort($labels);
          $info[$key]['label'] = key($labels);

          // Generate a description for the token.
          $info[$key]['description'] = t('@type field.', array('@type' => $type_info[$field['type']]['label']));
          if ($also_known_as = array_unique(array_diff($info[$key]['labels'], array($info[$key]['label'])))) {
            $info[$key]['description'] .= ' ' . t('Also known as %labels.', array('%labels' => implode(', ', $also_known_as)));
          }
        }
      }

      drupal_alter('token_field_info', $info);
      cache_set('field:info', $info, 'cache_token');
    }
  }

  if (isset($field_name)) {
    return isset($info[$field_name]) ? $info[$field_name] : FALSE;
  }

  return $info;
}

/**
 * Implements hook_token_info_alter() on behalf of field.module.
 *
 * We use hook_token_info_alter() rather than hook_token_info() as other
 * modules may already have defined some field tokens.
 */
function field_token_info_alter(&$info) {
  $fields = _token_field_info();

  // Attach field tokens to their respecitve entity tokens.
  foreach ($fields as $field_name => $field) {
    foreach (array_keys($field['bundles']) as $token_type) {
      // If a token already exists for this field, then don't add it.
      if (isset($info['tokens'][$token_type][$field_name])) {
        continue;
      }

      // Ensure the tokens exist.
      if (!isset($info['types'][$token_type]) || !isset($info['tokens'][$token_type])) {
        continue;
      }

      if ($token_type == 'comment' && $field_name == 'comment_body') {
        // Core provides the comment field as [comment:body].
        continue;
      }

      $info['tokens'][$token_type][$field_name] = array(
        'name' => $field['label'],
        'description' => $field['description'],
        'module' => 'token',
      );
    }
  }
}

/**
 * Implements hook_tokens() on behalf of field.module.
 */
function field_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);
  $langcode = isset($options['language']) ? $options['language']->language : NULL;

  // Entity tokens.
  if ($type == 'entity' && !empty($data['entity_type']) && !empty($data['entity']) && !empty($data['token_type'])) {
    $entity_type = $data['entity_type'];
    // The field API does weird stuff to the entity, so let's clone it.
    $entity = clone $data['entity'];

    list(, , $bundle) = entity_extract_ids($entity_type, $entity);
    $fields = field_info_instances($entity_type, $bundle);

    foreach (array_keys($fields) as $field_name) {
      // Do not continue if the field is empty.
      if (empty($entity->{$field_name})) {
        continue;
      }

      // Replace the [entity:field-name] token.
      if (isset($tokens[$field_name])) {
        $original = $tokens[$field_name];

        // Assert that this field was added by token.module.
        $token_info = token_get_info($data['token_type'], $field_name);
        if (!isset($token_info['module']) || $token_info['module'] != 'token') {
          continue;
        }

        $field_output = field_view_field($entity_type, $entity, $field_name, 'token', $langcode);
        $field_output['#token_options'] = $options;
        $field_output['#pre_render'][] = 'token_pre_render_field_token';
        $replacements[$original] = drupal_render($field_output);
      }
    }

    // Remove the cloned object from memory.
    unset($entity);
  }

  return $replacements;
}

/**
 * Pre-render callback for field output used with tokens.
 */
function token_pre_render_field_token(&$elements) {
  // Remove the field theme hook, attachments, and JavaScript states.
  unset($elements['#theme']);
  unset($elements['#states']);
  unset($elements['#attached']);

  // Prevent multi-value fields from appearing smooshed together by appending
  // a join suffix to all but the last value.
  $deltas = element_get_visible_children($elements);
  $count = count($deltas);
  if ($count > 1) {
    $join = isset($elements['#token_options']['join']) ? $elements['#token_options']['join'] : ", ";
    foreach ($deltas as $index => $delta) {
      // Do not add a suffix to the last item.
      if ($index < ($count - 1)) {
        $elements[$delta] += array('#suffix' => $join);
      }
    }
  }
  return $elements;
}
