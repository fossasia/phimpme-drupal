<?php

/**
 * @file
 * Hooks provided by the File Entity module.
 */

/**
 * Define file types.
 *
 * @return
 *   An array whose keys are file type names and whose values are arrays
 *   describing the file type, with the following key/value pairs:
 *   - label: The human-readable name of the file type.
 *   - claim callback: The name of the function that returns if a given file is
 *     of this type. See hook_file_type_TYPE_claim() for details.
 *   - default view callback: (optional) The name of the function that returns a
 *     drupal_render() array for displaying the file. Used when there are no
 *     administrator configured file formatters, or none of the configured ones
 *     return a display. See hook_file_type_TYPE_default_view() for details.
 *   - description: (optional) A short description of the file type.
 *   - weight: (optional) A number defining the order in which the 'claim
 *     callback' function for this type is called relative to the claim
 *     callbacks of other defined types, when the type of a file needs to be
 *     determined. The type with the lowest weighted claim callback to return
 *     TRUE is assigned to the file. Also, on administrative pages listing file
 *     types, the types are ordered by weight.
 *   - admin: (optional) An array of information, to be added to the
 *     ['bundles'][TYPE]['admin'] entry for the 'file' entity type, thereby
 *     controlling the path at which Field UI pages are attached for this file
 *     type, and which users may access them. Defaults to attaching the Field UI
 *     pages to the admin/config/media/file-types/manage/TYPE path and requiring
 *     'administer site configuration' permission. See hook_entity_info() for
 *     details about this array. This value can also be set to NULL to suppress
 *     Field UI pages from attaching at all for this file type.
 *
 * @see hook_file_type_info_alter()
 */
function hook_file_type_info() {
  return array(
    'image' => array(
      'label' => t('Image'),
    ),
  );
}

/**
 * Perform alterations on file types.
 *
 * @param $info
 *   Array of information on file types exposed by hook_file_type_info()
 *   implementations.
 */
function hook_file_type_info_alter(&$info) {
  // @todo Add example.
}

/**
 * @todo Add documentation.
 *
 * Note: This is not really a hook. The function name is manually specified via
 * 'claim callback' in hook_file_type_info(), with this recommended
 * callback name pattern.
 */
function hook_file_type_TYPE_claim($file, $type) {
}

/**
 * @todo Add documentation.
 *
 * Note: This is not really a hook. The function name is manually specified via
 * 'default view callback' in hook_file_type_info(), with this recommended
 * callback name pattern.
 */
function hook_file_type_TYPE_default_view($file, $view_mode, $langcode) {
}

/**
 * Define file formatters.
 *
 * @return
 *   An array whose keys are file formatter names and whose values are arrays
 *   describing the formatter.
 *
 * @todo Document key/value pairs that comprise a formatter.
 *
 * @see hook_file_formatter_info_alter()
 */
function hook_file_formatter_info() {
  // @todo Add example.
}

/**
 * Perform alterations on file formatters.
 *
 * @param $info
 *   Array of information on file formatters exposed by
 *   hook_file_formatter_info() implementations.
 */
function hook_file_formatter_info_alter(&$info) {
  // @todo Add example.
}

/**
 * @todo Add documentation.
 *
 * Note: This is not really a hook. The function name is manually specified via
 * 'view callback' in hook_file_formatter_info(), with this recommended callback
 * name pattern.
 */
function hook_file_formatter_FORMATTER_view($file, $display, $langcode) {
}

/**
 * @todo Add documentation.
 *
 * Note: This is not really a hook. The function name is manually specified via
 * 'settings callback' in hook_file_formatter_info(), with this recommended
 * callback name pattern.
 */
function hook_file_formatter_FORMATTER_settings($form, &$form_state, $settings) {
}

/**
 * @todo Add documentation.
 */
function hook_file_displays_alter($displays, $file, $view_mode) {
}

/**
 * @todo Add documentation.
 */
function hook_file_view($file, $view_mode, $langcode) {
}

/**
 * @todo Add documentation.
 */
function hook_file_view_alter($build, $type) {
}
