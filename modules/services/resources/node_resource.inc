<?php

function _node_resource_definition() {
  $node_resource = array(
    'node' => array(
      'retrieve' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_retrieve',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The nid of the node to get',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'create' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_create',
        'args' => array(
          array(
            'name' => 'node',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The node data to create',
            'type' => 'array',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_update',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The nid of the node to get',
          ),
          array(
            'name' => 'node',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The node data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_delete',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_index',
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
            'description' => 'Parameters array',
            'default value' => array(),
            'source' => array('param' => 'parameters'),
          ),
          array(
            'name' => 'pagesize',
            'optional' => TRUE,
            'type' => 'init',
            'description' => 'Number of records to get per page.',
            'default value' => variable_get('services_node_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access arguments' => array('access content'),
      ),
      'relationships' => array(
        'files' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
          'help'   => t('This method returns files associated with a node.'),
          'access callback' => '_node_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => '_node_resource_load_node_files',
          'args'     => array(
            array(
              'name' => 'nid',
              'optional' => FALSE,
              'source' => array('path' => 0),
              'type' => 'int',
              'description' => 'The nid of the node whose files we are getting',
            ),
            array(
              'name' => 'file_contents',
              'type' => 'int',
              'description'  => t('To return file contents or not.'),
              'source' => array('path' => 2),
              'optional' => TRUE,
              'default value' => TRUE,
            ),
            array(
              'name'         => 'image_styles',
              'type'         => 'int',
              'description'  => t('To return image styles or not.'),
              'source'       => array('path' => 3),
              'default value' => FALSE,
            ),
          ),
        ),
      ),
    ),
  );
  if (module_exists('comment')) {
    $comments = array(
      'file'                    => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
      'help'                    => t('This method returns the number of new comments on a given node.'),
      'access callback'         => 'user_access',
      'access arguments'        => array('access comments'),
      'access arguments append' => FALSE,
      'callback'                => '_node_resource_load_node_comments',
      'args'                    => array(
        array(
          'name'         => 'nid',
          'type'         => 'int',
          'description'  => t('The node id to load comments for.'),
          'source'       => array('path' => 0),
          'optional'     => FALSE,
        ),
        array(
          'name'         => 'count',
          'type'         => 'int',
          'description'  => t('Number of comments to load.'),
          'source'       => array('param' => 'count'),
          'optional'     => TRUE,
        ),
        array(
          'name'         => 'offset',
          'type'         => 'int',
          'description'  => t('If count is set to non-zero value, you can pass also non-zero value for start. For example to get comments from 5 to 15, pass count=10 and start=5.'),
          'source'       => array('param' => 'offset'),
          'optional'     => TRUE,
        ),
      ),
    );
    $node_resource['node']['relationships']['comments'] =  $comments;
  }
  return $node_resource;
}

/**
 * Returns the results of a node_load() for the specified node.
 *
 * This returned node may optionally take content_permissions settings into
 * account, based on a configuration setting.
 *
 * @param $nid
 *   NID of the node we want to return.
 * @return
 *   Node object or FALSE if not found.
 *
 * @see node_load()
 */
function _node_resource_retrieve($nid) {
  $node = node_load($nid);

  if ($node) {
    $uri = entity_uri('node', $node);
    $node->path = url($uri['path'], array('absolute' => TRUE));
    // Unset uri as it has complete entity and this
    // cause never ending recursion in rendering.
    unset($node->uri);
  }
  return $node;
}

/**
 * Creates a new node based on submitted values.
 *
 * Note that this function uses drupal_execute() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   An associative array contained the new node's nid and, if applicable,
 *   the fully qualified URI to this resource.
 *
 * @see drupal_execute()
 */
function _node_resource_create($node) {
  // Adds backwards compatability with regression fixed in #1083242
  $node = _services_arg_value($node, 'node');

  if (!isset($node['name'])) {
    // Assign username to the node from $user created at auth step.
    global $user;
    $node['name'] = $user->name;
  }

  // Validate the node. If there is validation error Exception will be thrown
  // so code below won't be executed.
  _node_resource_validate_type($node);

  // Load the required includes for drupal_execute
  module_load_include('inc', 'node', 'node.pages');

  $node_type = $node['type'];

  // Setup form_state
  $form_state = array();
  $form_state['values'] = $node;
  $form_state['values']['op'] = variable_get('services_node_save_button_' . $node_type . '_resource_create', t('Save'));

  drupal_form_submit($node_type . '_node_form', $form_state, (object)$node);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
  }
  // Fetch $nid out of $form_state
  $nid = $form_state['nid'];
  // Only add the URI for servers that support it.
  $node = array('nid' => $nid);
  if ($uri = services_resource_uri(array('node', $nid))) {
    $node['uri'] = $uri;
  }
  return $node;
}

/*
 * Helper function to validate node type information.
 *
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 */
function _node_resource_validate_type($node) {
  if (!isset($node['type'])) {
    return services_error(t('Missing node type'), 406);
  }
  // Wanted to return a graceful error instead of a blank nid, this should
  // allow for that.
  $types = node_type_get_types();
  $node_type = $node['type'];
  if (!isset($types[$node_type])) {
    return services_error(t('Node type @type does not exist.', array('@type' => $node_type)), 406);
  }
  $allowed_node_types = variable_get('services_allowed_create_content_types', $types);
  if (!isset($allowed_node_types[$node_type])) {
    return services_error(t("This node type @type can't be processed via services", array('@type' => $node_type)), 406);
  }
}

/**
 * Updates a new node based on submitted values.
 *
 * Note that this function uses drupal_execute() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $nid
 *   Node ID of the node we're editing.
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   The node's nid.
 *
 * @see drupal_execute()
 */
function _node_resource_update($nid, $node) {
  // Adds backwards compatability with regression fixed in #1083242
  $node = _services_arg_value($node, 'node');

  $node['nid'] = $nid;

  $old_node = node_load($nid);
  node_object_prepare($old_node);

  // Validate the node. If there is validation error Exception will be thrown
  // so code below won't be executed.
  _node_resource_validate_type($node);

  if ($old_node->nid) {
    // Node types cannot be changed once they are created.
    if (isset($node['type']) && $node['type'] != $old_node->type) {
      return services_error(t('Node type cannot be changed'), 406);
    }

    // Load the required includes for drupal_execute
    module_load_include('inc', 'node', 'node.pages');

    $node_type = $node['type'];

    // Setup form_state.
    $form_state = array();
    $form_state['values'] = $node;
    $form_state['values']['op'] = variable_get('services_node_save_button_' . $node_type . '_resource_update', t('Save'));
    $form_state['node'] = $old_node;
    drupal_form_submit($node_type . '_node_form', $form_state, $old_node);

    if ($errors = form_get_errors()) {
      return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
    }
  }
  else {
    return services_error(t('Node @nid not found', array('@nid' => $old_node->nid)), 404);
  }

  $node = array('nid' => $nid);
  if ($uri = services_resource_uri(array('node', $nid))) {
    $node['uri'] = $uri;
  }
  return $node;
}

/**
 * Delete a node given its nid.
 *
 * @param $nid
 *   Node ID of the node we're deleting.
 * @return
 *   The node's nid.
 */
function _node_resource_delete($nid) {
  node_delete($nid);
  return TRUE;
}

/**
 * Return an array of optionally paged nids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/node?fields=nid,vid&parameters[nid]=7&parameters[uid]=1
 *
 * This would return an array of objects with only nid and vid defined, where
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
 *   An array of node objects.
 *
 * @todo
 *   Evaluate the functionality here in general. Particularly around
 *     - Do we need fields at all? Should this just return full nodes?
 *     - Is there an easier syntax we can define which can make the urls
 *       for index requests more straightforward?
 */
function _node_resource_index($page, $fields, $parameters, $page_size) {
  module_load_include('inc', 'services', 'services.module');
  $node_select = db_select('node', 't')
    ->addTag('node_access')
    ->orderBy('sticky', 'DESC')
    ->orderBy('created', 'DESC');

  services_resource_build_index_query($node_select, $page, $fields, $parameters, $page_size, 'node');

  if (!user_access('administer nodes')) {
    $node_select->condition('status', 1);
  }

  $results = services_resource_execute_index_query($node_select);

  return services_resource_build_index_list($results, 'node', 'nid');
}

/**
 * Determine whether the current user can access a node resource.
 *
 * @param $op
 *   One of view, update, create, delete per node_access().
 * @param $args
 *   Resource arguments passed through from the original request.
 * @return bool
 *
 * @see node_access()
 */
function _node_resource_access($op = 'view', $args = array()) {
  // Adds backwards compatability with regression fixed in #1083242
  if (isset($args[0])) {
    $args[0] = _services_access_value($args[0], 'node');
  }

  // Make sure we have an object or this all fails, some servers can
  // mess up the types.
  if (is_array($args[0])) {
    $args[0] = (object) $args[0];
  }
  elseif (!is_array($args[0]) && !is_object($args[0])) {  //This is to determine if it is just a string happens on node/%NID
    $args[0] = (object)array('nid' => $args[0]);
  }

  if ($op != 'create' && !empty($args)) {
    $node = node_load($args[0]->nid);
  }
  elseif ($op == 'create') {
    if (isset($args[0]->type)) {
      $node = $args[0]->type;
      return node_access($op, $node);
    }
    else {
      return services_error(t('Node type is required'), 406);
    }
  }
  if (isset($node->nid) && $node->nid) {
    return node_access($op, $node);
  }
  else {
    return services_error(t('Node @nid could not be found', array('@nid' => $args[0]->nid)), 404);
  }
}

/**
 * Generates an array of base64 encoded files attached to a node
 *
 * @param $nid
 *   Number. Node ID
 * @param $include_file_contents
 *   Bool Whether or not to include the base64_encoded version of the file.
  * @param $get_image_style
 *   Bool Whether or not to provide image style paths.
 * @return
 *   Array. A list of all files from the given node
 */
function _node_resource_load_node_files($nid, $include_file_contents, $get_image_style) {
  module_load_include('inc', 'services', 'resources/file_resource');
  $node = node_load($nid);

  // Hopefully theres another way to get a nodes fields that are a file, but this was the only way I could do it.
  $fields = field_info_fields();
  $files = array();

  // Loop through all of the fields on the site
  foreach ($fields as $key => $field) {
    //if we are a field type of file
    if ($field['type'] == 'image' || $field['type'] == 'file') {
      // If this field exists on our current node..
      if (isset($node->{$field['field_name']})) {
        // If there are items in the field...
        if (isset($node->{$field['field_name']}[LANGUAGE_NONE])) {
          // Grab the items given and attach them to the array.
          $node_file_field_items = $node->{$field['field_name']}[LANGUAGE_NONE];
          foreach ($node_file_field_items as $file) {
            $files[] = _file_resource_retrieve($file['fid'], $include_file_contents, $get_image_style);
          }
        }
      }
    }
  }

  return $files;
}

/**
 * Returns the comments of a specified node.
 *
 * @param $nid
 *   Unique identifier for the node.
 * @param $count
 *   Number of comments to return.
 * @param $start
 *   Which comment to start with. If present, $start and $count are used together
 *   to create a LIMIT clause for selecting comments. This could be used to do paging.
 * @return
 *   An array of comment objects.
 */
function _node_resource_load_node_comments($nid, $count = 0, $start = 0) {
  $query = db_select('comment', 'c');
  $query->innerJoin('node', 'n', 'n.nid = c.nid');
  $query->addTag('node_access');
  $query->fields('c', array('cid'))
    ->condition('c.nid', $nid);

  if ($count) {
    $query->range($start, $count);
  }

  $result = $query->execute()
    ->fetchAll();

  foreach ($result as $record) {
    $cids[] = $record->cid;
  }

  return !empty($cids) ? comment_load_multiple($cids) : array();
}
