<?php

function _comment_resource_definition() {
  if (!module_exists('comment')) {
    return array();
  }

  return array(
    'comment' => array(
      'create' => array(
        'help' => 'Creates a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_create',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'comment',
            'type' => 'array',
            'description' => 'The comment object',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'retrieve' => array(
        'help' => 'Retrieves a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_retrieve',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The cid of the comment to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),

      'update' => array(
        'help' => 'Updates a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_update',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('edit'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The unique identifier for this comment.',
          ),
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The comment object with updated information',
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),

      'delete' => array(
        'help' => 'Deletes a comment',
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_delete',
        'access callback' => '_comment_resource_access',
        'access arguments' => array('edit'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The id of the comment to delete',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
        'callback' => '_comment_resource_index',
        'args' => array(
          array(
            'name' => 'page',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'The zero-based index of the page to get, defaults to 0.',
            'default value' => 0,
            'source' => array('param' => 'page'),
          ),
          array(
            'name' => 'fields',
            'optional' => TRUE,
            'type' => 'string',
            'description' => 'The fields to get.',
            'default value' => '*',
            'source' => array('param' => 'fields'),
          ),
          array(
            'name' => 'parameters',
            'optional' => TRUE,
            'type' => 'array',
            'description' => 'Parameters',
            'default value' => array(),
            'source' => array('param' => 'parameters'),
          ),
          array(
            'name' => 'pagesize',
            'optional' => TRUE,
            'type' => 'init',
            'description' => 'Number of records to get per page.',
            'default value' => variable_get('services_comment_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access callback' => '_comment_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'actions' => array(
        'countAll' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of comments on a given node.'),
          'access callback' => '_comment_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_comment_resource_count_all',
          'args'     => array(
            array(
              'name'         => 'nid',
              'type'         => 'int',
              'description'  => t('The node id to count all comments.'),
              'source'       => array('data' => 'nid'),
              'optional'     => FALSE,
            ),
          ),
        ),
        'countNew' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/comment_resource'),
          'help'   => t('This method returns the number of new comments on a given node since a given timestamp.'),
          'access callback' => '_comment_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_comment_resource_count_new',
          'args'     => array(
            array(
              'name'          => 'nid',
              'type'          => 'int',
              'description'   => t('The node id to load comments for.'),
              'source'        => array('data' => 'nid'),
              'optional'      => FALSE,
            ),
            array(
              'name'          => 'since',
              'type'          => 'int',
              'optional'      => TRUE,
              'description'   => t('Timestamp to count from (defaults to time of last user acces to node).'),
              'source'        => array('data' => 'since'),
              'optional'      => TRUE,
              'default value' => 0,
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Adds a new comment to a node and returns the cid.
 *
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or errors if there was a problem.
 */

function _comment_resource_create($comment) {
  // Adds backwards compatability with regression fixed in #1083242
  $comment = _services_arg_value($comment, 'comment');

  if (empty($comment['nid'])) {
    return services_error(t('A nid must be provided'));
  }

  $form_state['values'] = $comment;
  $form_state['values']['op'] = variable_get('services_comment_save_button_resource_create', t('Save'));

  $comment_empty = array(
    'nid' => $comment['nid'],
    'cid' => NULL,
  );

  // If a pid is provide use it
  if (!empty($comment['pid'])) {
    $comment_empty['pid'] = $comment['pid'];
  }

  $comment_empty = (object) $comment_empty;

  $ret = drupal_form_submit('comment_form', $form_state, $comment_empty);

  // Error if needed.
  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  $comment = $form_state['comment'];

  return array(
    'cid' => $comment->cid,
    'uri' => services_resource_uri(array('comment', $comment->cid)),
  );
}

/**
 * Returns a specified comment
 *
 * @param $cid
 *   Unique identifier for the specified comment
 * @return
 *   The comment object
 */
function _comment_resource_retrieve($cid) {
  return comment_load($cid);
}

/**
 * Updates a comment and returns the cid.
 *
 * @param $cid
 *   Unique identifier for this comment.
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or FALSE if there was a problem.
 */
function _comment_resource_update($cid, $comment) {
  // Adds backwards compatability with regression fixed in #1083242
  $comment = _services_arg_value($comment, 'data');
  $comment['cid'] = $cid;

  $old_comment = comment_load($cid);
  if (empty($old_comment)) {
    return services_error(t('Comment @cid not found', array('@cid' => $cid), 404));
  }
  // Setup form_state.
  $form_state = array();
  $form_state['values'] = $comment;
  $form_state['values']['op'] = variable_get('services_comment_save_button_resource_update', t('Save'));
  $form_state['comment'] = $old_comment;

  drupal_form_submit('comment_form', $form_state, $old_comment);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }

  return $cid;
}

/**
 * Delete a comment.
 *
 * @param $cid
 *   Unique identifier of the comment to delete.
 * @return
 *   True.
 */
function _comment_resource_delete($cid) {
  // Load in the required includes for comment_delete.
  module_load_include('inc', 'comment', 'comment.admin');

  // The following is from comment_confirm_delete_submit in comment.admin.inc
  $comment = comment_load($cid);
  if (empty($comment)) {
    return services_error(t('There is no comment found with id @cid', array('@cid' => $cid)), 404);
  }

  // Delete comment and its replies.
  comment_delete($cid);

  // Clear the cache so an anonymous user sees that his comment was deleted.
  cache_clear_all();
  return TRUE;
}
/**
 * Return an array of optionally paged cids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/comment?fields=cid,nid&parameters[nid]=7&parameters[uid]=2
 *
 * This would return an array of objects with only cid and nid defined, where
 * nid = 7 and uid = 1.
 *
 * @param $page
 *   Page number of results to return (in pages of 20).
 * @param $fields
 *   The fields you want returned.
 * @param $parameters
 *   An array containing fields and values used to build a sql WHERE clause
 *   indicating items to retrieve.
 * @param $page_size
 *   Integer number of items to be returned.
 * @return
 *   An array of comment objects.
 *
 * @see _node_resource_index() for more notes
 **/
function _comment_resource_index($page, $fields, $parameters, $page_size) {
  $comment_select = db_select('comment', 't')
    ->orderBy('created', 'DESC');

  services_resource_build_index_query($comment_select, $page, $fields, $parameters, $page_size, 'comment');

  if (!user_access('administer comments')) {
    $comment_select->condition('status', COMMENT_PUBLISHED);
  }

  $results = services_resource_execute_index_query($comment_select);

  return services_resource_build_index_list($results, 'comment', 'cid');
}

/**
 * Returns the number of comments on a given node id.
 *
 * @param $nid
 *   Unique identifier for the specified node.
 * @return
 *   Number of comments that node has.
 */
function _comment_resource_count_all($nid) {
  $node = node_load($nid);
  return $node->comment_count;
}

/**
 * Returns the number of new comments on a given node id since timestamp.
 *
 * @param $nid
 *   Unique identifier for the specified node.
 * @param $since
 *   Timestamp to indicate what nodes are new. Defaults to time of last user acces to node.
 * @return
 *   Number of comments that node has.
 */
function _comment_resource_count_new($nid, $since) {
  return comment_num_new($nid, $since);
}

/**
 * Access check callback for comment controllers.
 */
function _comment_resource_access($op = 'view', $args = array()) {
  // Adds backwards compatability with regression fixed in #1083242
  if (isset($args[0])) {
    $args[0] = _services_access_value($args[0], array('comment', 'data'));
  }

  if ($op == 'create') {
    $comment = (object)$args[0];
  }
  else {
    $comment = comment_load($args[0]);
  }
  if(isset($comment->nid)) {
    $node = node_load($comment->nid);
    if($op == 'create' && !$node->nid) {
      return services_error(t('Node nid: @nid does not exist.', array('@nid' => $comment->nid)), 406);
    }
  }
  if (user_access('administer comments')) {
    return TRUE;
  }
  switch ($op) {
    case 'view':
      // Check if the user has access to comments
      return user_access('access comments');
    case 'create':
    case 'delete':
    case 'edit':
      // Check if the user may post comments, node has comments enabled
      // and that the user has access to the node.
      return user_access('post comments') && ($node->comment == COMMENT_NODE_OPEN);
  }
}
