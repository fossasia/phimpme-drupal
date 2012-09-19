<?php
/**
 * @file
 *  Link general taxonomy functionalities to services module.
 */

function _taxonomy_resource_definition() {
  if (!module_exists('taxonomy')) {
    return array();
  }

  return array(
    'taxonomy_term' => array(
      'retrieve' => array(
        'callback' => '_taxonomy_term_resource_retrieve',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The tid of the taxonomy term to get',
          ),
        ),
        'access arguments' => array('access content'),
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'create' => array(
        'callback' => '_taxonomy_term_resource_create',
        'args' => array(
          array(
            'name' => 'term',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy term object to create',
            'type' => 'array',
          ),
        ),
        'access callback' => '_taxonomy_resource_create_access',
        'access arguments append' => TRUE,
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'update' => array(
        'callback' => '_taxonomy_term_resource_update',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'description' => 'The unique identifier for this taxonomy term.',
            'type' => 'int',
          ),
          array(
            'name' => 'term',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy term data to update',
            'type' => 'array',
          ),
        ),
        'access callback' => '_taxonomy_resource_update_access',
        'access arguments append' => TRUE,
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'delete' => array(
        'callback' => '_taxonomy_term_resource_delete',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
          ),
        ),
        'access callback' => '_taxonomy_resource_delete_access',
        'access arguments append' => TRUE,
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
        'callback' => '_taxonomy_term_resource_index',
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
            'default value' => variable_get('services_taxonomy_term_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access arguments' => array('access content'),
      ),
      'actions' => array(
        'selectNodes' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
          'help'   => t('This method returns all nodes with provided taxonomy id.'),
          'access arguments' => array('access content'),
          'callback' => 'taxonomy_service_select_nodes',
          'args'     => array(
            array(
              'name'         => 'tid',
              'type'         => 'string',
              'description'  => t('The vocabulary ids to retrieve, separated by comma.'),
              'source'       => array('data' => 'tid'),
              'optional'     => FALSE,
            ),
            array(
              'name'         => 'pager',
              'type'         => 'int',
              'description'  => t('Whether the nodes are to be used with a pager (the case on most Drupal pages) or not (in an XML feed, for example).'),
              'source'       => array('data' => 'pager'),
              'optional'     => TRUE,
              'default value'=> TRUE,
            ),
            array(
              'name'         => 'limit',
              'type'         => 'int',
              'description'  => t('Maximum number of nodes to find.'),
              'source'       => array('data' => 'limit'),
              'optional'     => TRUE,
              'default value'=> FALSE,
            ),
            array(
              'name'         => 'order',
              'type'         => 'int',
              'description'  => t('The order clause for the query that retrieve the nodes.'),
              'source'       => array('data' => 'order'),
              'optional'     => TRUE,
              'default value'=> array('t.sticky' => 'DESC', 't.created' => 'DESC'),
            ),
          ),
        ),
      ),
    ),
    'taxonomy_vocabulary' => array(
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      'retrieve' => array(
        'callback' => '_taxonomy_vocabulary_resource_retrieve',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The vid of the taxonomy vocabulary to get',
          ),
        ),
        'access arguments' => array('access content'),
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'create' => array(
        'callback' => '_taxonomy_vocabulary_resource_create',
        'args' => array(
          array(
            'name' => 'vocabulary',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy vocabulary object to create',
            'type' => 'array',
          ),
        ),
        'access arguments' => array('administer taxonomy'),
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'update' => array(
        'callback' => '_taxonomy_vocabulary_resource_update',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'description' => 'The unique identifier for this taxonomy vocabulary.',
            'type' => 'int',
          ),
          array(
            'name' => 'vocabulary',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy vocabulary data to update',
            'type' => 'array',
          ),
        ),
        'access arguments' => array('administer taxonomy'),
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'delete' => array(
        'callback' => '_taxonomy_vocabulary_resource_delete',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
          ),
        ),
        'access arguments' => array('administer taxonomy'),
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
        'callback' => '_taxonomy_vocabulary_resource_index',
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
            'default value' => variable_get('services_taxonomy_vocabulary_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access arguments' => array('access content'),
      ),
      'actions' => array(
        'getTree' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
          'help'   => t('This method returns a full list of taxonomy terms.'),
          'access arguments' => array('access content'),
          'callback' => 'taxonomy_service_get_tree',
          'args'     => array(
            array(
              'name'         => 'vid',
              'type'         => 'int',
              'description'  => t('The vocabulary id to retrieve.'),
              'source'       => array('data' => 'vid'),
              'optional'     => FALSE,
            ),
            array(
              'name'         => 'parent',
              'type'         => 'int',
              'description'  => t('The term ID under which to generate the tree. If 0, generate the tree for the entire vocabulary.'),
              'source'       => array('data' => 'parent'),
              'default value' => 0,
              'optional'     => TRUE,
            ),
            array(
              'name'         => 'maxdepth',
              'type'         => 'int',
              'description'  => t('The number of levels of the tree to return. Leave NULL to return all levels.'),
              'source'       => array('data' => 'maxdepth'),
              'default value' => NULL,
              'optional'     => TRUE,
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Return the results of taxonomy_get_term() for a specified term id.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   A term object.
 *
 * @see taxonomy_get_term()
 */
function _taxonomy_term_resource_retrieve($tid) {
  return taxonomy_term_load($tid);
}

/**
 * Create a new taxonomy term based on submitted values.
 *
 * @param $term
 *   Array of values for the taxonomy term.
 * @return
 *   Status constant indicating if term was inserted or updated.
 *
 * @see taxonomy_save_term()
 */
function _taxonomy_term_resource_create($term) {
  // Adds backwards compatability with regression fixed in #1083242
  $term = _services_arg_value($term, 'term');

  $term = (object)$term;
  return taxonomy_term_save($term);
}

/**
 * Update a taxonomy term based on submitted values.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to update.
 * @param $term
 *   Array of values for the taxonomy term.
 * @return
 *   Status constant indicating if term was inserted or updated.
 *
 * @see taxonomy_save_term()
 */
function _taxonomy_term_resource_update($tid, $term) {
  // Adds backwards compatability with regression fixed in #1083242
  $term = _services_arg_value($term, 'term');

  $term = (object) $term;
  $term->tid = $tid;
  return taxonomy_term_save($term);
}

/**
 * Delete a taxonomy term.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to delete.
 * @return
 *   Status constant indicating deletion.
 *
 * @see taxonomy_del_term()
 */
function _taxonomy_term_resource_delete($tid) {
  return taxonomy_term_delete($tid);
}


/**
 * Return the results of taxonomy_vocabulary_load() for a specified vocabulary id.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   A vocabulary object or FALSE.
 *
 * @see taxonomy_vocabulary_load()
 */
function _taxonomy_vocabulary_resource_retrieve($vid) {
  return taxonomy_vocabulary_load($vid);
}

/**
 * Create a new taxonomy vocabulary based on submitted values.
 *
 * Here is a sample vocabulary array, taken from
 * http://drupaldeveloper.in/article/programmatically-create-vocabulary
 *
 * @code
 * $vocabulary = array(
 *   'name'  => t("Name"). // Human readable name of the vocabulary
 *   'description' => t("Description"), // extended description of the vocabulary
 *   'help' => t("help"), // help text
 *   'tags' => 0, // 1 to make this vocabulary free tagging
 *   'multiple' => 0, // 1 to allow multiple selection
 *   'required' => 0, // 1 to make the terms mandatory to be selected
 *   'hierarchy' => 0, // 1 to allow and create hierarchy of the terms within the vocabulary
 *   'relations' => 0, // 1 to set and allow relation amongst multiple terms
 *   'module' => 'mymodule', // provide the module name in which the vocabulary is defined and which is calling this function
 *   'node' => array('story' => 1), // content types to which this vocabulary will be attached to
 *   'weight' => -9, // set the weight to display the vocabulary in the list
 * );
 * @endcode
 *
 * @param $vocabulary
 *   Array of values for the taxonomy vocabulary.
 * @return
 *   Status constant indicating if vocabulary was inserted or updated.
 *
 * @see taxonomy_vocabulary_save()
 */
function _taxonomy_vocabulary_resource_create($vocabulary) {
  // Adds backwards compatability with regression fixed in #1083242
  $vocabulary = _services_arg_value($vocabulary, 'vocabulary');

  $vocabulary = (object) $vocabulary;
  return taxonomy_vocabulary_save($vocabulary);
}

/**
 * Update a taxonomy vocabulary based on submitted values.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @param $vocabulary
 *   Array of values for the taxonomy vocabulary.
 * @return
 *   Status constant indicating if vocabulary was inserted or updated.
 *
 * @see taxonomy_vocabulary_save()
 */
function _taxonomy_vocabulary_resource_update($vid, $vocabulary) {
  // Adds backwards compatability with regression fixed in #1083242
  $vocabulary = _services_arg_value($vocabulary, 'vocabulary');

  $vocabulary = (object) $vocabulary;
  $vocabulary->vid = $vid;
  return taxonomy_vocabulary_save($vocabulary);
}

/**
 * Delete a taxonomy vocabulary.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   Status constant indicating deletion.
 *
 * @see taxonomy_del_vocabulary()
 */
function _taxonomy_vocabulary_resource_delete($vid) {
  return taxonomy_vocabulary_delete($vid);
}



/**
 * Services interface to taxonomy_get_tree().
 *
 * @see taxonomy_get_tree()
 */
function taxonomy_service_get_tree($vid, $parent = 0, $max_depth = NULL) {
  $terms = taxonomy_get_tree($vid, $parent, $max_depth);
  if (empty($terms)) {
    return services_error(t('No vocabulary with id @vid found.', array('@vid' => $vid)), 404);
  }
  return $terms;
}

/**
 * Services interface to taxonomy_select_nodes().
 *
 * Note that where taxonomy_select_nodes() returns the results
 * of a db_query(), this function returns an array of node objects.
 *
 * @see taxonomy_select_nodes()
 * @return
 *   An array of node objects.
 */
function taxonomy_service_select_nodes($tid = '', $pager, $limit,  $order) {
  $result = taxonomy_select_nodes($tid, (bool)$pager, $limit, $order);
  foreach ($result as $nid) {
    $node = node_load($nid);
    if ($uri = services_resource_uri(array('node', $nid))) {
      $node->uri = $uri;
    }
    $nodes[] = $node;
  }
  if (empty($nodes)) {
    return services_error(t('No nodes were found with tid @tid', array('@tid' => $tid)), 404);
  }
  return $nodes;
}

/**
 * Access callback for term updating.
 *
 * @param $term
 *   An taxonomy term object
 * @return
 *   Boolean whether or not the user has access to create or edit the term.
 */
function _taxonomy_resource_update_access($tid, $term) {
  // Adds backwards compatability with regression fixed in #1083242
  $term = _services_arg_value($term, 'term');
  
  $term = (object) $term;
  if (!isset($term->vid)) {
    return services_error(t('Term object needs vid property.'), 406);
  }
  return user_access('edit terms in ' . $term->vid) || user_access('administer taxonomy');
}

/**
 * Access callback for term creating.
 *
 * @param $term
 *   An taxonomy term object
 * @return
 *   Boolean whether or not the user has access to create or edit the term.
 */
function _taxonomy_resource_create_access($term) {
  // Adds backwards compatability with regression fixed in #1083242
  $term = _services_arg_value($term, 'term');

  $term = (object) $term;
  return user_access('edit terms in ' . $term->vid) || user_access('administer taxonomy');
}

/**
 * Access callback for term deleting.
 *
 * @param $term
 *   An taxonomy term object
 * @return
 *   Boolean whether or not the user has access to delete the term.
 */
function _taxonomy_resource_delete_access($tid) {
  $term = taxonomy_term_load($tid);
  if (!$term) {
    return services_error(t('There is no term with id @tid.', array('@tid' => $tid)), 406);
  }
  return user_access('delete terms in ' . $term->vid) || user_access('administer taxonomy');
}

/**
 * Return an array of optionally paged tids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/taxonomy_term?fields=tid,name&parameters[tid]=7&parameters[vid]=1
 *
 * This would return an array of objects with only tid and name defined, where
 * tid = 7 and vid = 1.
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
 *   An array of term objects.
 *
 * @see _node_resource_index() for more notes
 **/
function _taxonomy_term_resource_index($page, $fields, $parameters, $page_size) {
  $taxonomy_select = db_select('taxonomy_term_data', 't')
    ->orderBy('vid', 'DESC')
    ->orderBy('weight', 'DESC')
    ->orderBy('name', 'DESC');

  services_resource_build_index_query($taxonomy_select, $page, $fields, $parameters, $page_size, 'taxonomy_term');

  $results = services_resource_execute_index_query($taxonomy_select);

  return services_resource_build_index_list($results, 'taxonomy_term', 'tid');
}

/**
 * Return an array of optionally paged vids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/taxonomy_vocabulary?fields=vid,name&parameters[vid]=2
 *
 * This would return an array of objects with only vid and name defined, where
 * vid = 2.
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
 *   An array of vocabulary objects.
 *
 * @todo
 *   Support node types as parameters.
 *
 * @see _node_resource_index() for more notes
 **/
function _taxonomy_vocabulary_resource_index($page, $fields, $parameters, $page_size) {
  $taxonomy_select = db_select('taxonomy_vocabulary', 't')
    ->orderBy('weight', 'DESC')
    ->orderBy('name', 'DESC');

  services_resource_build_index_query($taxonomy_select, $page, $fields, $parameters, $page_size, 'taxonomy_vocabulary');

  $results = $taxonomy_select->execute();

  return services_resource_build_index_list($results, 'taxonomy_vocabulary', 'vid');
}