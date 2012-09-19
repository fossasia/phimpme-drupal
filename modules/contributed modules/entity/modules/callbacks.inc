<?php

/**
 * @file
 * Provides various callbacks for the whole core module integration.
 */

/**
 * Callback for getting properties of an entity.
 */
function entity_metadata_entity_get_properties($entity, array $options, $name, $entity_type) {
  if ($name == 'url') {
    $return = entity_uri($entity_type, $entity);
    return url($return['path'], $return['options'] + $options);
  }
}

/**
 * Callback for getting book node properties.
 * @see entity_metadata_book_entity_info_alter()
 */
function entity_metadata_book_get_properties($node, array $options, $name, $entity_type) {
  if (!isset($node->book['bid'])) {
    throw new EntityMetadataWrapperException('This node is no book page.');
  }
  return $node->book['bid'];
}

/**
 * Callback for getting comment properties.
 * @see entity_metadata_comment_entity_info_alter()
 */
function entity_metadata_comment_get_properties($comment, array $options, $name) {
  switch ($name) {
    case 'name':
      return $name = ($comment->uid == 0) ? variable_get('anonymous', t('Anonymous')) : $comment->name;

    case 'mail':
      if ($comment->uid != 0) {
        $account = user_load($comment->uid);
        return $account->mail;
      }
      return $comment->mail;

    case 'edit_url':
      return url('comment/edit/' . $comment->cid, $options);

    case 'parent':
      if (!empty($comment->pid)) {
        return $comment->pid;
      }
      // There is no parent comment.
      return NULL;
  }
}

/**
 * Callback for setting comment properties.
 * @see entity_metadata_comment_entity_info_alter()
 */
function entity_metadata_comment_setter($comment, $name, $value) {
  switch ($name) {
    case 'node':
      $comment->nid = $value;
      // Also set the bundle name.
      $node = node_load($value);
      $comment->node_type = 'comment_node_' . $node->type;
      break;
  }
}

/**
 * Callback for getting comment related node properties.
 * @see entity_metadata_comment_entity_info_alter()
 */
function entity_metadata_comment_get_node_properties($node, array $options, $name, $entity_type) {
  switch ($name) {
    case 'comment_count':
      return isset($node->comment_count) ? $node->comment_count : 0;

    case 'comment_count_new':
      return comment_num_new($node->nid);
  }
}

/**
 * Return the options lists for the node and comment status property.
 */
function entity_metadata_status_options_list() {
  return array(
    0 => t('Published'),
    1 => t('Unpublished'),
  );
}

/**
 * Callback for getting node properties.
 *
 * @see entity_metadata_node_entity_info_alter()
 */
function entity_metadata_node_get_properties($node, array $options, $name, $entity_type) {
  switch ($name) {
    case 'is_new':
      return empty($node->nid) || !empty($node->is_new);

    case 'source':
      if (!empty($node->tnid) && $source = node_load($node->tnid)) {
        return $source;
      }
      return NULL;

    case 'edit_url':
      return url('node/' . $node->nid . '/edit', $options);
  }
}

/**
 * Callback for determing access for node revision related properties.
 */
function entity_metadata_node_revision_access($op, $name, $entity = NULL, $account = NULL) {
  return $op == 'view' ? user_access('view revisions', $account) : user_access('administer nodes', $account);
}

/**
 * Callback for getting poll properties.
 * @see entity_metadata_poll_entity_info_alter()
 */
function entity_metadata_poll_node_get_properties($node, array $options, $name) {
  $total_votes = $highest_votes = 0;
  foreach ($node->choice as $choice) {
    if ($choice['chvotes'] > $highest_votes) {
      $winner = $choice;
      $highest_votes = $choice['chvotes'];
    }
    $total_votes = $total_votes + $choice['chvotes'];
  }

  if ($name == 'poll_duration') {
    return $node->runtime;
  }
  elseif ($name == 'poll_votes') {
    return $total_votes;
  }
  elseif (!isset($winner)) {
    // There is no poll winner yet.
    return NULL;
  }
  switch ($name) {
    case 'poll_winner_votes':
        return $winner['chvotes'];

    case 'poll_winner':
        return $winner['chtext'];

    case 'poll_winner_percent':
        return ($winner['chvotes'] / $total_votes) * 100;
  }
}

/**
 * Callback for getting statistics properties.
 * @see entity_metadata_statistics_entity_info_alter()
 */
function entity_metadata_statistics_node_get_properties($node, array $options, $name) {
  $statistics = (array) statistics_get($node->nid);
  $statistics += array('totalcount' => 0, 'daycount' => 0, 'timestamp' => NULL);

  switch ($name) {
    case 'views':
      return $statistics['totalcount'];

    case 'day_views':
      return $statistics['daycount'];

    case 'last_view':
      return $statistics['timestamp'];
  }
}

/**
 * Callback for getting site-wide properties.
 * @see entity_metadata_system_entity_info_alter()
 */
function entity_metadata_system_get_properties($data = FALSE, array $options, $name) {
  switch ($name) {
    case 'name':
      return variable_get('site_name', 'Drupal');

    case 'url':
      return url('<front>', $options);

    case 'login_url':
      return url('user', $options);

    case 'current_user':
      return $GLOBALS['user']->uid ? $GLOBALS['user']->uid : drupal_anonymous_user();

    case 'current_date':
      return time();

    case 'current_page':
      // Subsequent getters of the struct retrieve the actual values.
      return array();

    default:
      return variable_get('site_' . $name, '');
  }
}

/**
 * Callback for getting properties for the current page request.
 * @see entity_metadata_system_entity_info_alter()
 */
function entity_metadata_system_get_page_properties($data = array(), array $options, $name) {
  switch ($name) {
    case 'url':
      return $GLOBALS['base_root'] . request_uri();
  }
}

/**
 * Callback for getting file properties.
 * @see entity_metadata_system_entity_info_alter()
 */
function entity_metadata_system_get_file_properties($file, array $options, $name) {
  switch ($name) {
    case 'name':
      return $file->filename;

    case 'mime':
      return $file->filemime;

    case 'size':
      return $file->filesize;

    case 'url':
      return url(file_create_url($file->uri), $options);

    case 'owner':
      return $file->uid;
  }
}

/**
 * Callback for getting term properties.
 *
 * @see entity_metadata_taxonomy_entity_info_alter()
 */
function entity_metadata_taxonomy_term_get_properties($term, array $options, $name) {
  switch ($name) {
    case 'node_count':
      return count(taxonomy_select_nodes($term->tid));

    case 'description':
      return check_markup($term->description, isset($term->format) ? $term->format : NULL, '', TRUE);

    case 'parent':
      if (isset($term->parent[0]) && !is_array(isset($term->parent[0]))) {
        return $term->parent;
      }
      return array_keys(taxonomy_get_parents($term->tid));

    case 'parents_all':
      // We have to return an array of ids.
      $tids = array();
      foreach (taxonomy_get_parents_all($term->tid) as $parent) {
        $tids[] = $parent->tid;
      }
      return $tids;
  }
}

/**
 * Callback for setting term properties.
 *
 * @see entity_metadata_taxonomy_entity_info_alter()
 */
function entity_metadata_taxonomy_term_setter($term, $name, $value) {
  switch ($name) {
    case 'vocabulary':
      // Make sure to also update the taxonomy bundle key.
      $vocabulary = taxonomy_vocabulary_load($value);
      $term->vocabulary_machine_name = $vocabulary->machine_name;
      return $term->vid = $value;
    case 'parent':
      return $term->parent = $value;
  }
}

/**
 * Callback for getting vocabulary properties.
 * @see entity_metadata_taxonomy_entity_info_alter()
 */
function entity_metadata_taxonomy_vocabulary_get_properties($vocabulary, array $options, $name) {
  switch ($name) {
    case 'term_count':
      $sql = "SELECT COUNT (1) FROM {taxonomy_term_data} td WHERE td.vid = :vid";
      return db_query($sql, array(':vid' => $vocabulary->vid))->fetchField();
  }
}

/**
 * Callback for getting user properties.
 * @see entity_metadata_user_entity_info_alter()
 */
function entity_metadata_user_get_properties($account, array $options, $name, $entity_type) {
  switch ($name) {
    case 'last_access':
      // In case there was no access the value is 0, but we have to return NULL.
      return empty($account->access) ? NULL : $account->access;

    case 'last_login':
      return empty($account->login) ? NULL : $account->login;

    case 'name':
      return empty($account->uid) ? variable_get('anonymous', t('Anonymous')) : $account->name;

    case 'url':
      if (empty($account->uid)) {
        return NULL;
      }
      $return = entity_uri('user', $account);
      return $return ? url($return['path'], $return['options'] + $options) : '';

    case 'edit_url':
      return empty($account->uid) ? NULL : url("user/$account->uid/edit", $options);

    case 'roles':
      return isset($account->roles) ? array_keys($account->roles) : array();
  }
}

/**
 * Callback for setting user properties.
 * @see entity_metadata_user_entity_info_alter()
 */
function entity_metadata_user_set_properties($account, $name, $value) {
  switch ($name) {
    case 'roles':
      $account->roles = array_intersect_key(user_roles(), array_flip($value));
      break;
  }
}

/**
 * Options list callback returning all user roles.
 */
function entity_metadata_user_roles($property_name = 'roles', $info = array(), $op = 'edit') {
  $roles = user_roles();
  if ($op == 'edit') {
    unset($roles[DRUPAL_AUTHENTICATED_RID], $roles[DRUPAL_ANONYMOUS_RID]);
  }
  return $roles;
}

/**
 * Return the options lists for user status property.
 */
function entity_metadata_user_status_options_list() {
  return array(
    0 => t('Blocked'),
    1 => t('Active'),
  );
}

/**
 * Callback defining an options list for language properties.
 */
function entity_metadata_language_list() {
  $list = array();
  $list[LANGUAGE_NONE] = t('Language neutral');
  foreach (language_list() as $language) {
    $list[$language->language] = $language->name;
  }
  return $list;
}

/**
 * Callback for getting field property values.
 */
function entity_metadata_field_property_get($entity, array $options, $name, $entity_type, $info) {
  $field = field_info_field($name);
  $columns = array_keys($field['columns']);
  $langcode = field_language($entity_type, $entity, $name, isset($options['language']) ? $options['language']->language : NULL);
  $values = array();
  if (isset($entity->{$name}[$langcode])) {
    foreach ($entity->{$name}[$langcode] as $delta => $data) {
      $values[$delta] = $data[$columns[0]];
      if ($info['type'] == 'boolean' || $info['type'] == 'list<boolean>') {
        // Ensure that we have a clean boolean data type.
        $values[$delta] = (boolean) $values[$delta];
      }
    }
  }
  // For an empty single-valued field, we have to return NULL.
  return $field['cardinality'] == 1 ? ($values ? reset($values) : NULL) : $values;
}

/**
 * Callback for setting field property values.
 */
function entity_metadata_field_property_set($entity, $name, $value, $langcode, $entity_type) {
  $field = field_info_field($name);
  $columns = array_keys($field['columns']);
  if (!isset($langcode)) {
    // Try to figure out the default language used by the entity.
    // @todo: Update once http://drupal.org/node/1260640 has been fixed.
    $langcode = isset($entity->language) ? $entity->language : LANGUAGE_NONE;
  }
  $values = $field['cardinality'] == 1 ? array($value) : (array) $value;

  $items = array();
  foreach ($values as $delta => $value) {
    if (isset($value)) {
      $items[$delta][$columns[0]] = $value;
    }
  }
  $entity->{$name}[$langcode] = $items;
  // Empty the static field language cache, so the field system picks up any
  // possible new languages.
  drupal_static_reset('field_language');
}

/**
 * Callback returning the options list of a field.
 */
function entity_metadata_field_options_list($name, $info) {
  if (is_numeric($name) && isset($info['parent'])) {
    // The options list is to be returned for a single item of a multiple field.
    $field_property_info = $info['parent']->info();
    $name = $field_property_info['name'];
  }
  if ($field = field_info_field($name)) {
    return (array) module_invoke($field['module'], 'options_list', $field);
  }
}

/**
 * Callback to verbatim get the data structure of a field. Useful for fields
 * that add metadata for their own data structure.
 */
function entity_metadata_field_verbatim_get($entity, array $options, $name, $entity_type, &$context) {
  // Set contextual info useful for getters of any child properties.
  $context['instance'] = field_info_instance($context['parent']->type(), $name, $context['parent']->getBundle());
  $context['field'] = field_info_field($name);

  $langcode = field_language($entity_type, $entity, $name, isset($options['language']) ? $options['language']->language : NULL);
  if ($context['field']['cardinality'] == 1) {
    return isset($entity->{$name}[$langcode][0]) ? $entity->{$name}[$langcode][0] : NULL;
  }
  return isset($entity->{$name}[$langcode]) ? $entity->{$name}[$langcode] : array();
}

/**
 * Writes the passed field items in the object. Useful as field level setter
 * to set the whole data structure at once.
 */
function entity_metadata_field_verbatim_set($entity, $name, $items, $langcode) {
  $field = field_info_field($name);
  if (!isset($langcode)) {
    // Try to figure out the default language used by the entity.
    // @todo: Update once http://drupal.org/node/1260640 has been fixed.
    $langcode = isset($entity->language) ? $entity->language : LANGUAGE_NONE;
  }
  $value = $field['cardinality'] == 1 ? array($items) : (array) $items;
  // Filter out any items set to NULL.
  $entity->{$name}[$langcode] = array_filter($value);

  // Empty the static field language cache, so the field system picks up any
  // possible new languages.
  drupal_static_reset('field_language');
}

/**
 * Callback for getting the sanitized text of 'text_formatted' properties.
 * This callback is used for both the 'value' and the 'summary'.
 */
function entity_metadata_field_text_get($item, array $options, $name, $type, $context) {
  // $name is either 'value' or 'summary'.
  if (!isset($item['safe_' . $name])) {
    // Apply input formats.
    $langcode = isset($options['language']) ? $options['language']->language : '';
    $format = isset($item['format']) ? $item['format'] : filter_default_format();
    $item['safe_' . $name] = check_markup($item[$name], $format, $langcode);
    // To speed up subsequent calls, update $item with the 'safe_value'.
    $context['parent']->set($item);
  }
  return $item['safe_' . $name];
}

/**
 * Defines the list of all available text formats.
 */
function entity_metadata_field_text_formats() {
  foreach (filter_formats() as $key => $format) {
    $formats[$key] = $format->name;
  }
  return $formats;
}

/**
 * Callback for getting the file entity of file fields.
 */
function entity_metadata_field_file_get($item) {
  return $item['fid'];
}

/**
 * Callback for setting the file entity of file fields.
 */
function entity_metadata_field_file_set(&$item, $property_name, $value) {
  $item['fid'] = $value;
}

/**
 * Callback for auto-creating file field $items.
 */
function entity_metadata_field_file_create_item($property_name, $context) {
  // 'fid' is required, so 'file' has to be set as initial property.
  return array('display' => $context['field']['settings']['display_default']);
}

/**
 * Callback for validating file field $items.
 */
function entity_metadata_field_file_validate_item($items, $context) {
  // Stream-line $items for multiple vs non-multiple fields.
  $items = !entity_property_list_extract_type($context['type']) ? array($items) : (array) $items;

  foreach ($items as $item) {
    // File-field items require a valid file.
    if (!isset($item['fid']) || !file_load($item['fid'])) {
      return FALSE;
    }
    if (isset($context['property info']['display']) && !isset($item['display'])) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Access callback for the node entity.
 *
 * This function does not implement hook_node_access(), thus it may not be
 * called entity_metadata_node_access().
 */
function entity_metadata_no_hook_node_access($op, $node = NULL, $account = NULL) {
  if (isset($node)) {
    return node_access($op, $node, $account);
  }
  // Is access to all nodes allowed?
  if (!user_access('access content', $account)) {
    return FALSE;
  }
  if (user_access('bypass node access', $account) || (!isset($account) && $op == 'view' && node_access_view_all_nodes())) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for the user entity.
 */
function entity_metadata_user_access($op, $entity = NULL, $account = NULL, $entity_type) {
  $account = isset($account) ? $account : $GLOBALS['user'];
  // Grant access to the users own user account and to the anonymous one.
  if (isset($entity) && $op != 'delete' && (($entity->uid == $account->uid && $entity->uid) || (!$entity->uid && $op == 'view'))) {
    return TRUE;
  }
  if (user_access('administer users', $account) || user_access('access user profiles', $account) && $op == 'view') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for restricted user properties.
 */
function entity_metadata_user_properties_access($op, $property, $entity = NULL, $account = NULL) {
  if (user_access('administer users', $account)) {
    return TRUE;
  }
  $account = isset($account) ? $account : $GLOBALS['user'];
  // Flag to indicate if this user entity is the own user account.
  $is_own_account = isset($entity) && $account->uid == $entity->uid;
  switch ($property) {
    case 'name':
      // Allow view access to anyone with access to the entity.
      if ($op == 'view') {
        return TRUE;
      }
      // Allow edit access for own user name if the permission is satisfied.
      return $is_own_account && user_access('change own username', $account);
    case 'mail':
      // Allow access to own mail address.
      return $is_own_account;
    case 'roles':
      // Allow view access for own roles.
      return ($op == 'view' && $is_own_account);
  }
  return FALSE;
}

/**
 * Access callback for the comment entity.
 */
function entity_metadata_comment_access($op, $entity = NULL, $account = NULL) {
  if (isset($entity) && !isset($account) && comment_access($op, $entity)) {
    return TRUE;
  }
  if (user_access('administer comments', $account) || user_access('access comments', $account) && $op == 'view') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for the taxonomy entities.
 */
function entity_metadata_taxonomy_access($op, $entity = NULL, $account = NULL, $entity_type) {
  if ($entity_type == 'taxonomy_vocabulary') {
    return user_access('administer taxonomy', $account);
  }
  if (isset($entity) && $op == 'edit' && !isset($account) && taxonomy_term_edit_access($entity)) {
    return TRUE;
  }
  if (user_access('administer taxonomy', $account) || user_access('access content', $account) && $op == 'view') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Callback to determine access for properties which are fields.
 */
function entity_metadata_field_access_callback($op, $name, $entity = NULL, $account = NULL, $entity_type) {
  $field = field_info_field($name);
  return field_access($op, $field, $entity_type, $entity, $account);
}

/**
 * Callback to create entity objects.
 */
function entity_metadata_create_object($values = array(), $entity_type) {
  $info = entity_get_info($entity_type);
  // Make sure at least the bundle and label properties are set.
  if (isset($info['entity keys']['bundle']) && $key = $info['entity keys']['bundle']) {
    $values += array($key => NULL);
  }
  if (isset($info['entity keys']['label']) && $key = $info['entity keys']['label']) {
    $values += array($key => NULL);
  }
  $entity = (object) $values;
  $entity->is_new = TRUE;
  return $entity;
}

/**
 * Callback to create a new comment.
 */
function entity_metadata_create_comment($values = array()) {
  $comment = (object) ($values + array(
    'status' => COMMENT_PUBLISHED,
    'pid' => 0,
    'subject' => '',
    'uid' => 0,
    'language' => LANGUAGE_NONE,
    'node_type' => NULL,
    'is_new' => TRUE,
  ));
  $comment->cid = FALSE;
  return $comment;
}

/**
 * Callback to create a new node.
 */
function entity_metadata_create_node($values = array()) {
  $node = (object) array(
    'type' => $values['type'],
    'language' => LANGUAGE_NONE,
    'is_new' => TRUE,
  );
  // Set some defaults.
  $node_options = variable_get('node_options_' . $node->type, array('status', 'promote'));
  foreach (array('status', 'promote', 'sticky') as $key) {
    $node->$key = (int) in_array($key, $node_options);
  }
  if (module_exists('comment') && !isset($node->comment)) {
    $node->comment = variable_get("comment_$node->type", COMMENT_NODE_OPEN);
  }
  // Apply the given values.
  foreach ($values as $key => $value) {
    $node->$key = $value;
  }
  return $node;
}

/**
 * Callback to save a user account.
 */
function entity_metadata_user_save($account) {
  $edit = (array) $account;
  // Don't save the hashed password as password.
  unset($edit['pass']);
  user_save($account, $edit);
}

/**
 * Callback to delete a file.
 * Watch out to not accidentilly implement hook_file_delete().
 */
function entity_metadata_delete_file($fid) {
  file_delete(file_load($fid), TRUE);
}

/**
 * Callback to view nodes.
 */
function entity_metadata_view_node($entities, $view_mode = 'full', $langcode = NULL) {
  $result = node_view_multiple($entities, $view_mode, 0, $langcode);
  // Make sure to key the result with 'node' instead of 'nodes'.
  return array('node' => reset($result));
}

/**
 * Callback to view comments.
 */
function entity_metadata_view_comment($entities, $view_mode = 'full', $langcode = NULL) {
  $build = array();
  $nodes = array();
  // The comments, indexed by nid and then by cid.
  $nid_comments = array();
  foreach ($entities as $cid => $comment) {
    $nid = $comment->nid;
    $nodes[$nid] = $nid;
    $nid_comments[$nid][$cid] = $comment;
  }
  $nodes = node_load_multiple(array_keys($nodes));
  foreach ($nid_comments as $nid => $comments) {
    $node = isset($nodes[$nid]) ? $nodes[$nid] : NULL;
    $build += comment_view_multiple($comments, $node, $view_mode, 0, $langcode);
  }
  return array('comment' => $build);
}

/**
 * Callback to view an entity, for which just ENTITYTYPE_view() is available.
 */
function entity_metadata_view_single($entities, $view_mode = 'full', $langcode = NULL, $entity_type) {
  $function = $entity_type . '_view';
  $build = array();
  foreach ($entities as $key => $entity) {
    $build[$entity_type][$key] = $function($entity, $view_mode, $langcode);
  }
  return $build;
}

/**
 * Callback to get the form of a node.
 */
function entity_metadata_form_node($node) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($node);
  form_load_include($form_state, 'inc', 'node', 'node.pages');
  return drupal_build_form($node->type . '_node_form', $form_state);
}

/**
 * Callback to get the form of a comment.
 */
function entity_metadata_form_comment($comment) {
  if (!isset($comment->node_type)) {
    $node = node_load($comment->nid);
    $comment->node_type = 'comment_node_' . $node->type;
  }
  return drupal_get_form($comment->node_type . '_form', $comment);
}

/**
 * Callback to get the form of a user account.
 */
function entity_metadata_form_user($account) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($account);
  form_load_include($form_state, 'inc', 'user', 'user.pages');
  return drupal_build_form('user_profile_form', $form_state);
}

/**
 * Callback to get the form of a term.
 */
function entity_metadata_form_taxonomy_term($term) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($term);
  form_load_include($form_state, 'inc', 'taxonomy', 'taxonomy.admin');
  return drupal_build_form('taxonomy_form_term', $form_state);
}

/**
 * Callback to get the form of a vocabulary.
 */
function entity_metadata_form_taxonomy_vocabulary($term) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($term);
  form_load_include($form_state, 'inc', 'taxonomy', 'taxonomy.admin');
  return drupal_build_form('taxonomy_form_term', $form_state);
}

/**
 * Callback to get the form for entities using the entity API admin ui.
 */
function entity_metadata_form_entity_ui($entity, $entity_type) {
  $info = entity_get_info($entity_type);
  $form_state = array();
  // Add in the include file as the form API does else with the include file
  // specified for the active menu item.
  if (!empty($info['admin ui']['file'])) {
    $path = isset($info['admin ui']['file path']) ? $info['admin ui']['file path'] : drupal_get_path('module', $info['module']);
    $form_state['build_info']['files']['entity_ui'] = $path . '/' . $info['admin ui']['file'];
    // Also load the include file.
    if (file_exists($form_state['build_info']['files']['entity_ui'])) {
      require_once DRUPAL_ROOT . '/' . $form_state['build_info']['files']['entity_ui'];
    }
  }
  return entity_ui_get_form($entity_type, $entity, $op = 'edit', $form_state);
}

/**
 * Callback for querying entity properties having their values stored in the
 * entities main db table.
 */
function entity_metadata_table_query($entity_type, $property, $value, $limit) {
  $properties = entity_get_all_property_info($entity_type);
  $info = $properties[$property] + array('schema field' => $property);

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type, '=')
        ->propertyCondition($info['schema field'], $value, is_array($value) ? 'IN' : '=')
        ->range(0, $limit);

  $result = $query->execute();
  return !empty($result[$entity_type]) ? array_keys($result[$entity_type]) : array();
}

/**
 * Callback for querying entities by field values. This function just queries
 * for the value of the first specified column. Also it is only suitable for
 * fields that don't process the data, so it's stored the same way as returned.
 */
function entity_metadata_field_query($entity_type, $property, $value, $limit) {
  $query = new EntityFieldQuery();
  $field = field_info_field($property);
  $columns = array_keys($field['columns']);

  $query->entityCondition('entity_type', $entity_type, '=')
        ->fieldCondition($field, $columns[0], $value, is_array($value) ? 'IN' : '=')
        ->range(0, $limit);

  $result = $query->execute();
  return !empty($result[$entity_type]) ? array_keys($result[$entity_type]) : array();
}
