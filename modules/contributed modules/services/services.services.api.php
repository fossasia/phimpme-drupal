<?php

/**
 * @file
 * Hooks provided by Services for the definition of new services.
 */

/**
 * @addtogroup hooks
 */

 /**
  * Defines function signatures for resources available to services.
  *
  * Functionally this is very similar to the way hook_menu() works, and in many
  * ways Services can be seen as an abstraction layer on top of hook_menu().
  *
  * @return
  *   An associative array which defines available resources.
  *
  *   The associative array which defines services has eight possible top
  *   level keys:
  *
  *     - create
  *     - retrieve
  *     - update
  *     - delete
  *     - index
  *     - actions
  *     - targeted_actions
  *     - relationships
  *
  *   The CRUD functions are pretty self-explanatory. Index is an extra CRUD-
  *   type function that allows you to create pageable lists.
  *
  *   Actions are performed directly on the resource type, not a individual
  *   resource. The following example is hypothetical (but plausible). Say
  *   that you want to expose a API for the apachesolr module. One of the
  *   things that could be exposed is the functionality to reindex the whole
  *   site at apachesolr/reindex.
  *
  *   Targeted actions acts on a individual resource. A good, but again -
  *   hypothetical, example would be the publishing and unpublishing of nodes
  *   at node/123/publish.
  *
  *   Relationship requests are convenience methods (sugar) to get something
  *   thats related to a individual resource. A real example would be the
  *   ability to get the files for a node at node/123/files.
  *
  *   The first five (the CRUD functions + index) define the indvidual service
  *   callbacks for each function. However 'actions', 'targeted_actions',
  *   and 'relationships' can contain multiple callbacks.
  *
  *   For those familiar with Services 2.x, these callbacks are created
  *   similarly, but the keys have changed around a bit. The following keys
  *   are used to describe a callback.
  *
  *   - help: Text describing what this callback does.
  *   - callback: The name of a function to call when this resource is
  *     requested.
  *   - file: an array describing the file which contains the callback
  *     function
  *   - access callback: The name of a function to call to check whether
  *     the requesting user has permission to access this resource. If not
  *     specified, this defaults to 'user_access'.
  *   - access callback file: an array describing the file which contains the
  *     access callback function.  This attribute only needs to be supplied if
  *     the method callback and the access callback are defined in different
  *     files, for example when a method callback is overridden using
  *     hook_services_resources_alter but the access callback is not
  *   - access arguments: The arguments to pass to the access callback.
  *   - access arguments append: A boolean indicating whether the resource's
  *     arguments should be appended to the access arguments. This can be useful
  *     in situations where an access callback is specific to the particular
  *     item ('edit all nodes' vs 'edit my nodes'). Defaults to FALSE.
  *   - args: an array describing the arguments which should be passed to this
  *     resource when it is called. Each element in the array is an associative
  *     array containing the following keys:
  *
  *     - name: The name of this argument.
  *     - type: The data type of this argument (int, string, array)
  *     - description: Text describing this argument's usage.
  *     - optional: A boolean indicating whether or not this argument is optional.
  *     - source: Where this argument should be retrieved from. This can be
  *       'data' (indicating the POST data), 'param' (indicating the query
  *       string) or 'path' (indicating the url path). In the case of path,
  *       an additional parameter must be passed indicating the index to be used.
  *     - default value: this is a value that will be passed to the method for
  *       this particular argument if no argument value is passed
  *
  *   A detailed example of creating a new resource can be found at
  *   http://drupal.org/node/783460 and more information about how
  *   REST resources are managed can be found at http://drupal.org/node/783254.
  */
function hook_services_resources() {
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
        'access callback file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
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
