<?php

/**
 * @file
 * Flag module tokens support.
 */

/**
 * Implements of hook_token_info().
 */
function flag_token_info() {
  $types = array();
  $tokens = array();

  // Flag tokens.
  $types['flag'] = array(
    'name' => t('Flags'),
    'description' => t('Tokens related to flag data.'),
    'needs-data' => 'flag',
  );
  $tokens['flag']['name'] = array(
    'name' => t('Flag name'),
    'description' => t('The flag machine-readable name.'),
  );
  $tokens['flag']['title'] = array(
    'name' => t('Flag title'),
    'description' => t('The human-readable flag title.'),
  );

  // Flage action tokens.
  $types['flag-action'] = array(
    'name' => t('Flag actions'),
    'description' => t('Tokens available in response to a flag action being executed by a user.'),
    'needs-data' => 'flag-action',
  );
  $tokens['flag-action']['action'] = array(
    'name' => t('Flag action'),
    'description' => t('The flagging action taking place, either "flag" or "unflag".'),
  );
  $tokens['flag-action']['content-url'] = array(
    'name' => t('Flag content URL'),
    'description' => t('The URL of the content being flagged.'),
  );
  $tokens['flag-action']['content-title'] = array(
    'name' => t('Flag content title'),
    'description' => t('The title of the content being flagged.'),
  );
  $tokens['flag-action']['content-type'] = array(
    'name' => t('Flag content type'),
    'description' => t('The type of content being flagged, such as <em>node</em> or <em>comment</em>.'),
  );
  $tokens['flag-action']['content-id'] = array(
    'name' => t('Flag content ID'),
    'description' => t('The ID of content being flagged, may be a nid or cid.'),
  );
  $tokens['flag-action']['count'] = array(
    'name' => t('Flag count'),
    'description' => t('The current count total for this flag.'),
  );

  // Add tokens for the flag count available at the node/comment/user level.
  foreach (flag_get_types() as $flag_type) {
    $flags = flag_get_flags($flag_type);
    foreach ($flags as $flag) {
      $tokens[$flag_type]['flag-'. str_replace('_', '-', $flag->name) .'-count'] = array(
        'name' => t('@flag flag count', array('@flag' => $flag->get_title())),
        'description' => t('Total flag count for flag @flag', array('@flag' => $flag->get_title())),
      );
      $tokens[$flag_type]['flag-'. str_replace('_', '-', $flag->name) .'-link'] = array(
        'name' => t('@flag flag link', array('@flag' => $flag->get_title())),
        'description' => t('Flag/unflag link for @flag', array('@flag' => $flag->get_title())),
      );
    }
  }

  return array(
    'types' => $types,
    'tokens' => $tokens,
  );
}

/**
 * Implements hook_tokens().
 */
function flag_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);

  if ($type == 'flag' && !empty($data['flag'])) {
    $flag = $data['flag'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'name':
          $replacements[$original] = $sanitize ? check_plain($flag->name) : $flag->name;
          break;
        case 'title':
          $replacements[$original] = $sanitize ? check_plain($flag->get_title()) : $flag->get_title();
          break;
      }
    }
  }
  elseif ($type == 'flag-action' && !empty($data['flag-action'])) {
    $action = $data['flag-action'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'action':
          $replacements[$original] = $action->action;
          break;
        case 'content-url':
          $replacements[$original] = $sanitize ? check_url($action->content_url) : $action->content_url;
          break;
        case 'content-title':
          $replacements[$original] = $sanitize ? check_plain($action->content_title) : $action->content_title;
          break;
        case 'content-type':
          $replacements[$original] = $action->content_type;
          break;
        case 'content-id':
          $replacements[$original] = $action->content_id;
          break;
        case 'count':
          $replacements[$original] = $action->count;
          break;
      }
    }
  }

  if (isset($data[$type]) && in_array($type, flag_get_types())) {
    $flags = flag_get_flags($type);
    $object = $data[$type];
    foreach ($flags as $flag) {
      foreach ($tokens as $name => $original) {
        $flag_count_token = 'flag-'. str_replace('_', '-', $flag->name) .'-count';
        $flag_link_token = 'flag-'. str_replace('_', '-', $flag->name) .'-link';
        if ($name == $flag_count_token) {
          $replacements[$original] = $flag->get_count($flag->get_content_id($object));
        }
        elseif ($name == $flag_link_token) {
          $replacements[$original] = flag_create_link($flag->name, $flag->get_content_id($object));
        }
      }
    }
  }
  return $replacements;
}

/**
 * Returns HTML for a tokens browser.
 *
 * @param $variables
 *   An associative array containing:
 *   - types: An array naming the types of tokens to show.
 *   - global_types: Whether to show global tokens.
 */
function theme_flag_tokens_browser($variables) {
  $types = $variables['types'];
  $global_types = $variables['global_types'];

  if (module_exists('token')) {
    return theme('token_tree', array('token_types' => $types, 'global_types' => $global_types));
  }
  else {
    return '<p><em>' . t("Note: You don't have the <a href='@token-url'>Token</a> module installed, so the list of available tokens isn't shown here. You don't have to install <a href='@token-url'>Token</a> to be able to use tokens, but if you have it installed, and enabled, you'll be able to enjoy an interactive tokens browser.", array('@token-url' => 'http://drupal.org/project/token')) . '</em></p>';
  }
}
