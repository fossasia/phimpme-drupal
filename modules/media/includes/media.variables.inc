<?php

/**
 * @file
 * Contains the variables used by media and their defaults.
 */

/* ***************************************** */
/* CONSTANTS                                 */
/* ***************************************** */

/**
 * @TODO: iz_cruft?
 * @var unknown_type
 */
define('MEDIA_RESOURCE_URI_DEFAULT', 'public://');
define('MEDIA_TYPES_DEFAULT', '*');

/**
 * Define constants.
 */
define('MEDIA_VARIABLE_NAMESPACE', 'media__');

/**
 *  Wrapper for variable_get() that uses the Media variable registry.
 *
 *  @param string $name
 *    The variable name to retrieve. Note that it will be namespaced by
 *    pre-pending MEDIA_VARIABLE_NAMESPACE, as to avoid variable collisions with
 *    other modules.
 *  @param unknown $default
 *    An optional default variable to return if the variable hasn't been set
 *    yet. Note that within this module, all variables should already be set
 *    in the media_variable_default() function.
 *  @return unknown
 *    Returns the stored variable or its default.
 *
 *  @see media_variable_set()
 *  @see media_variable_del()
 *  @see media_variable_default()
 */
function media_variable_get($name, $default = NULL) {
  // Allow for an override of the default.
  // Useful when a variable is required (like $path), but namespacing still desired.
  if (!isset($default)) {
    $default = media_variable_default($name);
  }
  // Namespace all variables
  $variable_name = MEDIA_VARIABLE_NAMESPACE . $name;
  return variable_get($variable_name, $default);
}

/**
 *  Wrapper for variable_set() that uses the Media variable registry.
 *
 *  @param string $name
 *    The variable name to set. Note that it will be namespaced by
 *    pre-pending MEDIA_VARIABLE_NAMESPACE, as to avoid variable collisions with
 *    other modules.
 *  @param unknown $value
 *    The value for which to set the variable.
 *  @return unknown
 *    Returns the stored variable after setting.
 *
 *  @see media_variable_get()
 *  @see media_variable_del()
 *  @see media_variable_default()
 */
function media_variable_set($name, $value) {
  $variable_name = MEDIA_VARIABLE_NAMESPACE . $name;
  return variable_set($variable_name, $value);
}

/**
 *  Wrapper for variable_del() that uses the Media variable registry.
 *
 *  @param string $name
 *    The variable name to delete. Note that it will be namespaced by
 *    pre-pending MEDIA_VARIABLE_NAMESPACE, as to avoid variable collisions with
 *    other modules.
 *
 *  @see media_variable_get()
 *  @see media_variable_set()
 *  @see media_variable_default()
 */
function media_variable_del($name) {
  $variable_name = MEDIA_VARIABLE_NAMESPACE . $name;
  variable_del($variable_name);
}

/**
 * Returns the final machine name of a Media namespaced variable.
 */
function media_variable_name($name) {
  return MEDIA_VARIABLE_NAMESPACE . $name;
}

/**
 *  The default variables within the Media namespace.
 *
 *  @param string $name
 *    Optional variable name to retrieve the default. Note that it has not yet
 *    been pre-pended with the MEDIA_VARIABLE_NAMESPACE namespace at this time.
 *  @return unknown
 *    The default value of this variable, if it's been set, or NULL, unless
 *    $name is NULL, in which case we return an array of all default values.
 *
 *  @see media_variable_get()
 *  @see media_variable_set()
 *  @see media_variable_del()
 */
function media_variable_default($name = NULL) {
  static $defaults;

  if (!isset($defaults)) {
    $defaults = array(
      'wysiwyg_title' => 'Media browser',
      'wysiwyg_icon_title' => 'Add media',
      //@todo: We should do this per type actually.  For "other" it should be a link.
      'wysiwyg_default_view_mode' => 'media_large',
      // Types which can be selected when embedding media vs wysiwyg.
      'wysiwyg_allowed_types' => array('audio', 'image', 'video'),
      // Attributes which can be modified via the wysiwyg and persist.
      'wysiwyg_allowed_attributes' => array('height', 'width', 'hspace', 'vspace', 'border', 'align', 'style', 'alt', 'title', 'class', 'id'),

      'field_select_media_text' => 'Select media',
      'field_remove_media_text' => 'Remove media',

      // Name of the theme to use in media popup dialogs, defaults to admin_theme
      'dialog_theme' => '',
      // @TODO: Make a configuration form with this.
      'file_extensions' => 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp mp3 mov m4v mp4 mpeg avi ogg oga ogv wmv ico',
      'max_filesize' => '',
      'debug' => FALSE,

      'file_list_size' => 10,

      // Used in media.xml.inc: how long to cache retrieved remote data.
      'xml_cache_expire' => 3600,

      // Browser defaults in media.browser.inc.
      'browser_viewtype_default' => 'thumbnails',
      'browser_pager_limit' => 40,
      'browser_library_empty_message' => 'There are currently no media in this library to select.',

      'import_batch_size' => 20,
      'fromurl_supported_schemes' => array('http', 'https', 'ftp', 'smb', 'ftps'),
      'type_icon_directory' => drupal_get_path('module', 'media') . '/images/types',
      'icon_base_directory' => drupal_get_path('module', 'media') . '/images/icons',
      'icon_set' => 'default',

       // This is set in media_enable().  It will show a persistant dsm on every page
       // until the user runs the batch operation provided by media_admin_rebuild_types_form().
      'show_file_type_rebuild_nag' => FALSE,
    );
  }

  if (!isset($name)) {
    return $defaults;
  }

  if (isset($defaults[$name])) {
    return $defaults[$name];
  }
}
