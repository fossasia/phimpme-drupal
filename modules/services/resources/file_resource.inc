<?php

/**
 * @file
 * File resource.
 */

/**
 * THERE SHOULD BE NO UPDATE!!!
 * Drupal doesn't allow updating or replacing a file in the files table.
 * If you need to, create a new file and remove the old file.
 */
function _file_resource_definition() {
  return array(
    'file' => array(
      'create' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),
        'help' => 'Creates a file with base64 encoded data',
        'callback' => '_file_resource_create',
        'access callback' => '_file_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'file',
            'type' => 'array',
            'description'    => t('An array representing a file.'),
            'source' => 'data',
            'optional' => FALSE,
          ),
        ),
      ),
      'retrieve' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),
        'help' => 'Retrieves a file',
        'callback' => '_file_resource_retrieve',
        'access callback' => '_file_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'fid',
            'type' => 'int',
            'description' => 'The fid of the file to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
          array(
            'name'         => 'file_contents',
            'type'         => 'int',
            'description'  => t('To return file contents or not.'),
            'source'       => array('param' => 'file_contents'),
            'default value' => TRUE,
          ),
          array(
            'name'         => 'image_styles',
            'type'         => 'int',
            'description'  => t('To return image styles or not.'),
            'source'       => array('param' => 'image_styles'),
            'default value' => FALSE,
          ),
        ),
      ),
      'delete' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),
        'help' => 'Deletes a file',
        'callback' => '_file_resource_delete',
        'access callback' => '_file_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'cid',
            'type' => 'int',
            'description' => 'The id of the file to delete',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),
        'callback' => '_file_resource_index',
        'help' => 'Lists all files',
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
            'default value' => variable_get('services_file_index_page_size', 20),
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access callback' => '_file_resource_access',
        'access arguments' => array('index'),
        'access arguments append' => TRUE,
      ),
      'actions' => array(
        'create_raw' => array(
          'help' => 'Creates a file with raw data.',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/file_resource'),
          'callback' => '_file_resource_create_raw',
          'access callback' => '_file_resource_access',
          'access arguments' => array('create_raw'),
          'access arguments append' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Adds a new file and returns the fid.
 *
 * @param $file
 *   An object as representing the file with a base64 encoded $file->file
 * @return
 *   Unique identifier for the file (fid) or errors if there was a problem.
 */
function _file_resource_create($file) {
  // Adds backwards compatability with regression fixed in #1083242
  // $file['file'] can be base64 encoded file so we check whether it is
  // file array or file data.
  $file = _services_arg_value($file, 'file');

  global $user;
  $file = (object) $file;

  // If the file data is empty then bail
  if (!isset($file->file)) {
    return FALSE;
  }

  // Make sure we create new file.
  $file->fid = NULL;

  // Get the directory name for the location of the file:
  if (isset($file->filename) && !isset($file->filepath)) {
    $file->filepath = file_default_scheme() . '://' . $file->filename;
  }
  $dir = file_default_scheme() . '://';
  // Build the destination folder tree if it doesn't already exists.
  if (!file_prepare_directory($dir, FILE_CREATE_DIRECTORY)) {
    return services_error(t("Could not create destination directory for file."), 500);
  }

  $file->filemime = file_get_mimetype($file->filename);

  // Rename potentially executable files, to help prevent exploits.
  if (preg_match('/\.(php|pl|py|cgi|asp|js)$/i', $file->filename) && (drupal_substr($file->filename, -4) != '.txt')) {
    $file->filemime = 'text/plain';
    $file->filepath .= '.txt';
    $file->filename .= '.txt';
  }

  // Update the timestamp to the current time, otherwise the file could
  // get deleted on the next cron run if its status is set to 0.
  $file->timestamp = time();

  // Write the file
  if (!$file = file_save_data(base64_decode($file->file), $file->filepath)) {
    return services_error(t("Could not write file to destination"), 500);
  }

  // Required to be able to reference this file.
  file_usage_add($file, 'services', 'files', $file->fid);

  return array(
    'fid' => $file->fid,
    'uri' => services_resource_uri(array('file', $file->fid)),
  );
}
/**
 * Adds new files and returns the files array.
 *
 * @return
 *   Array of file objects with URIS to access them
 */
function _file_resource_create_raw() {
  $validators = array(
    'file_validate_extensions' => array(),
    'file_validate_size' => array(),
  );
  $files = array();
  foreach ($_FILES['files']['name'] as $field_name => $file_name) {
    $file = file_save_upload($field_name, $validators, file_default_scheme() . "://");
    // Required to be able to reference this file.
    if ($file->fid) {
      file_usage_add($file, 'services', 'files', $file->fid);
      $files[] = array(
        'fid' => $file->fid,
        'uri' => services_resource_uri(array('file', $file->fid)),
      );
    }
    else {
      return services_error(t('An unknown error occured', 500));
    }
  }
  return $files;
}
/**
 * Get a given file
 *
 * @param $fid
 *   Number. File ID
 * @param $include_file_contents
 *   Bool Whether or not to include the base64_encoded version of the file.
 * @param $get_image_style
 *   Bool Whether or not to provide image style paths.
 * @return
 *   The file
 */
function _file_resource_retrieve($fid, $include_file_contents, $get_image_style) {
  if ($file = file_load($fid)) {
    $filepath = $file->uri;

    // Convert the uri to the external url path provided by the stream wrapper.
    $file->uri_full = file_create_url($file->uri);

    // Provide a path in the form sample/test.txt.
    $file->target_uri = file_uri_target($file->uri);

    if ($include_file_contents) {
      $file->file = base64_encode(file_get_contents(drupal_realpath($filepath)));
    }

    $file->image_styles = array();
    // Add image style information if available.
    if ($get_image_style) {
      foreach (image_styles() as $style) {
        $style_name = $style['name'];
        $file->image_styles[$style_name] = image_style_url($style_name, $file->uri);
      }
    }
    return $file;
  }
}

/**
 * Delete a file.
 *
 * @param $fid
 *   Unique identifier of the file to delete.
 * @return bool
 *   Whether or not the delete was successful.
 */
function _file_resource_delete($fid) {
  if ($file = file_load($fid)) {
    file_usage_delete($file, 'services');
    return file_delete($file);
  }
  return FALSE;
}

/**
 * Return an array of optionally paged fids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/file?fields=fid,filename&parameters[fid]=7&parameters[uid]=1
 *
 * This would return an array of objects with only fid and filename defined, where
 * fid = 7 and uid = 1.
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
 *   An array of file objects.
 *
 * @see _node_resource_index() for more notes
 **/
function _file_resource_index($page, $fields, $parameters, $page_size) {
  $file_select = db_select('file_managed', 't')
    ->orderBy('timestamp', 'DESC');

  services_resource_build_index_query($file_select, $page, $fields, $parameters, $page_size, 'file');

  $results = services_resource_execute_index_query($file_select);

  // Put together array of matching files to return.
  return services_resource_build_index_list($results, 'file', 'fid');
}

/**
 * Access check callback for file controllers.
 */
function _file_resource_access($op = 'view', $args = array()) {
  // Adds backwards compatability with regression fixed in #1083242
  if (isset($args[0])) {
    $args[0] = _services_access_value($args[0], 'file'); 
  }

  global $user;

  if (($op != 'create' && $op != 'create_raw') && $op != 'index') {
    $file = file_load($args[0]);
  } else if ($op == 'create' && $op != 'create_raw') {
    $file = (object)$args[0];
  }
  if (empty($file) && $op != 'index' && ($op != 'create' && $op != 'create_raw')) {
    return services_error(t('There is no file with ID @fid', array('@fid' => $args[0])), 406);
  }
  switch ($op) {
    case 'view':
      if (user_access('get any binary files')) {
        return TRUE;
      }
      return $file->uid == $user->uid && user_access('get own binary files');
      break;
    case 'create':
    case 'create_raw':
      return user_access('save file information');
    case 'delete':
      return $file->uid == $user->uid && user_access('save file information');
      break;
    case 'index':
      if (user_access('get any binary files')) {
        return TRUE;
      }
  }

  return FALSE;
}

function _file_resource_node_access($op = 'view', $args = array()) {
  global $user;
  if (user_access('get any binary files')) {
    return TRUE;
  }
  elseif ($node = node_load($args[0])) {
    return $node->uid == $user->uid && user_access('get own binary files');
  }
  return FALSE;
}

