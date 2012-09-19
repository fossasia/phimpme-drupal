<?php

/**
 * @file
 * CTools Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function ctools_drush_command() {
  $items = array();

  $items['ctools-export'] = array(
    'aliases' => array('ctex'),
    'description' => 'Export multiple CTools exportable objects directly to code.',
    'arguments' => array(
      'module'    => 'Name of your module.',
    ),
    'options' => array(
      'subdir' => 'The name of the sub directory to create the module in. Defaults to ctools_export which will be placed into sites/all/modules.',
      'remove' => 'Remove existing files before writing, except the .module file.',
    ),
    'drupal dependencies' => array('bulk_export'),
    'examples' => array(
      'drush ctex export_module' => 'Export CTools exportables to a module called "export_module".',
      'drush ctex export_module --subdir=exports' => 'Same as above, but into the sites/all/modules/exports directory.',
      'drush ctex export_module --subdir=exports --remove' => 'Same as above, but automatically removing all files, except for the .module file.',
    ),
  );

  $items['ctools-export-info'] = array(
    'aliases' => array('ctei'),
    'description' => 'Show available CTools exportable objects.',
    'arguments' => array(),
    'options' => array(
      'format' => 'Display exportables info in a different format such as print_r, json, export. The default is to show in a tabular format.',
      'tables-only' => 'Only show list of exportable types/table names and not available objects.',
      'enabled' => 'Only show exportables that are currently enabled.',
      'disabled' => 'Only show exportables that are currently disabled.',
      'overridden' => 'Only show exportables that have been overridden in the database.',
      'database' => 'Only show exportables defined in the database (and not in code).',
    ),
    'examples' => array(
      'drush ctools-export-info' => 'View export info on all exportables.',
      'drush ctools-export-info views_view variable' => 'View export info for views_view and variable exportable types only.',
    ),
  );

  $items['ctools-export-view'] = array(
    'aliases' => array('ctev'),
    'callback' => 'drush_ctools_export_op_command',
    'description' => 'View CTools exportable object code output.',
    'arguments' => array(
      'table name' => 'Base table of the exportable you want to view.',
      'machine names' => 'Space separated list of exportables you want to view.',
    ),
    'options' => array(
      'indent' => 'The string to use for indentation when dispalying the exportable export code. Defaults to \'\'.',
      'no-colour' => 'Remove any colour formatting from export string output. Ideal if you are sending the output of this command to a file.',
    ),
    'examples' => array(
      'drush ctools-export-view views_view' => 'View all views exportable objects.',
      'drush ctools-export-view views_view archive' => 'View default views archive view.',
    ),
  );

  $items['ctools-export-revert'] = array(
    'aliases' => array('cter'),
    'callback' => 'drush_ctools_export_op_command',
    'description' => 'Revert CTools exportables from changes overridden in the database.',
    'arguments' => array(
      'table name' => 'Base table of the exportable you want to revert.',
      'machine names' => 'Space separated list of exportables you want to revert.',
    ),
    'options' => array(),
    'examples' => array(
      'drush ctools-export-revert views_view' => 'Revert all overridden views exportable objects.',
      'drush ctools-export-revert views_view archive' => 'Revert overridden default views archive view.',
    ),
  );

  $items['ctools-export-enable'] = array(
    'aliases' => array('ctee'),
    'callback' => 'drush_ctools_export_op_command',
    'description' => 'Enable CTools exportables.',
    'arguments' => array(
      'table name' => 'Base table of the exportable you want to enable.',
      'machine names' => 'Space separated list of exportables you want to enable.',
    ),
    'options' => array(),
    'examples' => array(
      'drush ctools-export-enable views_view' => 'Enable all overridden views exportable objects.',
      'drush ctools-export-enable views_view archive' => 'Enable overridden default views archive view.',
    ),
  );

  $items['ctools-export-disable'] = array(
    'aliases' => array('cted'),
    'callback' => 'drush_ctools_export_op_command',
    'description' => 'Disable CTools exportables.',
    'arguments' => array(
      'table name' => 'Base table of the exportable you want to disable.',
      'machine names' => 'Space separated list of exportables you want to disable.',
    ),
    'options' => array(),
    'examples' => array(
      'drush ctools-export-disable views_view' => 'Disable all overridden views exportable objects.',
      'drush ctools-export-disable views_view archive' => 'Disable overridden default views archive view.',
    ),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function ctools_drush_help($section) {
  switch ($section) {
    case 'meta:ctools:title':
      return dt('CTools commands');
    case 'meta:entity:summary':
      return dt('CTools drush commands.');
  }
}

/**
 * Drush callback: export
 */
function drush_ctools_export($module = 'foo') {
  $error = FALSE;
  if (preg_match('@[^a-z_]+@', $module)) {
    $error = dt('The name of the module must contain only lowercase letters and underscores') . '.';
    drush_log($error, 'error');
    return;
  }

  // Selection.
  $options = array('all' => dt('Export everything'), 'select' => dt('Make selection'));
  $selection = drush_choice($options, dt('Select to proceed'));

  if (!$selection) {
    return;
  }

  // Present the selection screens.
  if ($selection == 'select') {
    $selections = _drush_ctools_selection_screen();
  }
  else {
    $selections = _drush_ctools_export_info();
  }

  // Subdirectory.
  $dest_exists = FALSE;
  $subdir = drush_get_option('subdir', 'ctools_export');
  $dest = 'sites/all/modules/' . $subdir . '/' . $module;

  // Overwriting files can be set with 'remove' argument.
  $remove = drush_get_option('remove', FALSE);

  // Check if folder exists.
  if (file_exists($dest)) {
    $dest_exists = TRUE;
    if ($remove) {
      if (drush_confirm(dt('All files except for the .info and .module files in !module will be removed. You can choose later if you want to overwrite these files as well. Are you sure you want to proceed ?', array('!module' => $module)))) {
        $remove = TRUE;
        drush_log(dt('Files will be removed'), 'success');
      }
      else {
        drush_log(dt('Export aborted.'), 'success');
        return;
      }
    }
  }

  // Remove files (except for the .module file) if the destination folder exists.
  if ($remove && $dest_exists) {
    _drush_ctools_file_delete($dest);
  }

  // Create new dir if needed.
  if (!$dest_exists) {
    if (!file_exists('sites/all/modules/' . $subdir)) {
      drush_mkdir('sites/all/modules/' . $subdir);
    }
  }

  // Create destination directory.
  drush_mkdir($dest);

  // Create options and call Bulk export function.
  // We create an array, because maybe in the future we can pass in more
  // options to the export function (pre-selected modules and/or exportables).
  $options = array(
    'name' => $module,
    'selections' => $selections,
  );
  $files = bulk_export_export(TRUE, $options);

  // Start writing.
  if (is_array($files)) {
    foreach ($files as $base_file => $data) {
      $filename = $dest . '/' . $base_file;
      // Extra check for .module file.
      if ($base_file == ($module . '.module' || $module . '.info') && file_exists($filename)) {
        if (!drush_confirm(dt('Do you want to overwrite !module_file', array('!module_file' => $base_file)))) {
          drush_log(dt('Writing of !filename skipped. This is the code that was supposed to be written:', array('!filename' => $filename)), 'success');
          drush_print('---------');
          drush_print(shellColours::getColouredOutput("\n$data", 'light_green'));
          drush_print('---------');
          continue;
        }
      }
      if (file_put_contents($filename, $data)) {
        drush_log(dt('Succesfully written !filename', array('!filename' => $filename)), 'success');
      }
      else {
        drush_log(dt('Error writing !filename', array('!filename' => $filename)), 'error');
      }
    }
    drush_log("\n" . dt('No penguins were harmed in the generation of this code.') . "\n", 'success');
  }
  else {
    drush_log(dt('No files were found to be written.'), 'error');
  }
}

/**
 * Helper function to select the exportables. By default, all exportables
 * will be selected, so it will be easier to deselect them.
 */
function _drush_ctools_selection_screen() {
  $selections = $build = array();
  $files = system_rebuild_module_data();

  $selection_number = 0;

  $exportables = _drush_ctools_export_info();
  $export_tables = array();

  foreach (array_keys($exportables) as $table) {
    natcasesort($exportables[$table]);
    $export_tables[$table] = $files[$schema['module']]->info['name'] . ' (' . $table . ')';
  }

  foreach ($export_tables as $table => $table_title) {
    if (!empty($exportables[$table])) {
      $table_count = count($exportables[$table]);
      $selection_number += $table_count;
      foreach ($exportables[$table] as $key => $title) {
        $build[$table]['title'] = $table_title;
        $build[$table]['items'][$key] = $title;
        $build[$table]['count'] = $table_count;
        $selections[$table][$key] = $key;
      }
    }
  }

  drush_print(dt('Number of exportables selected: !number', array('!number' => $selection_number)));
  drush_print(dt('By default all exportables are selected. Select a table to deselect exportables. Select "cancel" to start writing the files.'));

  // Let's go into a loop.
  $return = FALSE;
  while (!$return) {

    // Present the tables choice.
    $table_rows = array();
    foreach ($build as $table => $info) {
      $table_rows[$table] = $info['title'] . ' (' . dt($info['count']) . ')';
    }
    $table_choice = drush_choice($table_rows, dt('Select a table. Select cancel to start writing files.'));

    // Bail out.
    if (!$table_choice) {
      drush_log(dt('Selection mode done, starting to write the files.'), 'notice');
      $return = TRUE;
      return $selections;
    }

    // Present the exportables choice, using the drush_choice_multiple.
    $max = count($build[$table_choice]['items']);
    $exportable_rows = array();
    foreach ($build[$table_choice]['items'] as $key => $title) {
      $exportable_rows[$key] = $title;
    }
    drush_print(dt('Exportables from !table', array('!table' => $build[$table_choice]['title'])));
    $multi_select = drush_choice_multiple($exportable_rows, $selections[$table_choice], dt('Select exportables.'), '!value', '!value (selected)', 0, $max);

    // Update selections.
    if (is_array($multi_select)) {
      $build[$table_choice]['count'] = count($multi_select);
      $selections[$table_choice] = array();
      foreach ($multi_select as $key) {
         $selections[$table_choice][$key] = $key;
      }
    }
  }
}

/**
 * Delete files in a directory, keeping the .module  and .info files.
 *
 * @param $path
 *   Path to directory in which to remove files.
 */
function _drush_ctools_file_delete($path) {
  if (is_dir($path)) {
    $files = new DirectoryIterator($path);
    foreach ($files as $fileInfo) {
      if (!$fileInfo->isDot() && !in_array($fileInfo->getExtension(), array('module', 'info'))) {
        unlink($fileInfo->getPathname());
      }
    }
  }
}

/**
 * Drush callback: Export info.
 *
 * @params $table_names
 *  Each argument will be taken as a CTools exportable table name.
 */
function drush_ctools_export_info() {
  // Collect array of table names from args.
  $table_names = func_get_args();

  // Get format option to allow for alternative output.
  $format = drush_get_option('format', FALSE);
  $tables_only = drush_get_option('tables-only', FALSE);
  $show_overridden = drush_get_option('overridden', FALSE);
  $show_enabled = drush_get_option('enabled', FALSE);
  $show_disabled = drush_get_option('disabled', FALSE);
  $show_database_only = drush_get_option('database', FALSE);

  // Only load exportable objects for each type fully if we need to.
  $load = ($show_overridden || $show_enabled || $show_disabled || $show_database_only) ? TRUE : FALSE;

  // Get info on these tables, or all if none specified.
  $exportables = _drush_ctools_export_info($table_names, $load);

  if (empty($exportables)) {
    drush_log(dt('There are no exportables available.'), 'warning');
    return;
  }

  // The order of these conditionals set a hierarchy for options if there are mulitple.
  // Show enabled exportables only.
  if ($show_enabled) {
    foreach ($exportables as $table => $objects) {
      foreach ($objects as $key => $object) {
        if (_ctools_drush_object_is_disabled($object)) {
          unset($exportables[$table][$key]);
        }
      }
    }
  }
  // Show disabled exportables only.
  elseif ($show_disabled) {
    foreach ($exportables as $table => $objects) {
      foreach ($objects as $key => $object) {
        if (!_ctools_drush_object_is_disabled($object)) {
          unset($exportables[$table][$key]);
        }
      }
    }
  }
  // Show overridden exportables only.
  elseif ($show_overridden) {
    foreach ($exportables as $table => $objects) {
      foreach ($objects as $key => $object) {
        if (!_ctools_drush_object_is_overridden($object)) {
          unset($exportables[$table][$key]);
        }
      }
    }
  }
  // Show database only exportables.
  elseif ($show_database_only) {
    foreach ($exportables as $table => $objects) {
      foreach ($objects as $key => $object) {
        if (!_ctools_drush_object_is_db_only($object)) {
          unset($exportables[$table][$key]);
        }
      }
    }
  }

  $exportables = array_filter($exportables);

  // Only use array keys if --tables-only option is set.
  if ($tables_only) {
    $exportables = array_keys($exportables);
  }

  // Use format from --format option if it's present, and send to drush_format.
  if ($format) {
    drush_print(drush_format($exportables, NULL, $format));
  }
  // Build a tabular output as default.
  else {
    $header = $tables_only ? array() : array(dt('Base table'), dt('Exportables'));
    $rows = array();
    foreach ($exportables as $table => $info) {
      if (is_array($info)) {
        $row = array(
          $table,
          // Machine name is better for this?
          shellColours::getColouredOutput(implode("\n", array_keys($info)), 'light_green') . "\n",
        );
        $rows[] = $row;
      }
      else {
        $rows[] = array($info);
      }
    }
    if (!empty($rows)) {
      array_unshift($rows, $header);
      drush_print_table($rows, TRUE);
    }
    else {
      drush_log(dt('There are no exportables matching this criteria.'), 'notice');
    }
  }
}

/**
 * Drush callback: Acts as the hub for all op commands to keep all arg handling etc in one place.
 */
function drush_ctools_export_op_command() {
  // Get all info for the current drush command.
  $command = drush_get_command();
  $op = '';

  switch ($command['command']) {
    case 'ctools-export-view':
      $op = 'view';
    break;
    case 'ctools-export-revert':
      // Revert is same as deleting. As any objects in the db are deleted.
      $op = 'delete';
    break;
    case 'ctools-export-enable':
      $op = 'enable';
    break;
    case 'ctools-export-disable':
      $op = 'disable';
    break;
  }

  if (!$op) {
    return;
  }

  $args = func_get_args();
  // Table name should always be first arg...
  $table_name = array_shift($args);
  // Any additional args are assumed to be exportable names.
  $object_names = $args;

  // Return any exportables based on table name, object names, options.
  $exportables = _drush_ctools_export_op_command_logic($op, $table_name, $object_names);

  if ($exportables) {
    drush_ctools_export_op($op, $table_name, $exportables);
  }
}

/**
 * Iterate through exportable object names, load them, and pass each
 * object to the correct op function.
 *
 * @param $op
 * @param $table_name
 * @param $exportables
 *
 */
function drush_ctools_export_op($op = '', $table_name = '', $exportables = NULL) {
  $objects = ctools_export_crud_load_multiple($table_name, array_keys($exportables));

  $function = '_drush_ctools_export_' . $op;
  if (function_exists($function)) {
    foreach ($objects as $object) {
      $function($table_name, $object);
    }
  }
  else {
    drush_log(dt('CTools CRUD function !function doesn\'t exist.',
      array('!function' => $function)), 'error');
  }
}

/**
 * Helper function to abstract logic for selecting exportable types/objects
 * from individual commands as they will all share this same error
 * handling/arguments for returning list of exportables.
 *
 * @param $table_name
 * @param $object_names
 *
 * @return
 *  Array of exportable objects (filtered if necessary, by name etc..) or FALSE if not.
 */
function _drush_ctools_export_op_command_logic($op = '', $table_name = NULL, $object_names = array()) {
  if (!$table_name) {
    drush_log(dt('Exportable table name empty.'), 'error');
    return FALSE;
  }

  // Get export info based on table name.
  $info = _drush_ctools_export_info(array($table_name));

  if (!isset($info[$table_name])) {
    drush_log(dt('Exportable table name not found.'), 'error');
    return FALSE;
  }

  $exportables = $info[$table_name];

  if (empty($object_names)) {
    $all = drush_confirm(dt('No object names entered. Would you like to try and !op all exportables of type !type',
      array('!op' => _drush_ctools_export_op_aliases($op), '!type' => $table_name)));
    if (!$all) {
      drush_log(dt('Command cancelled'), 'success');
      return FALSE;
    }
  }
  else {
    // Iterate through object names and check they exist in exportables array.
    // Log error and unset them if they don't.
    foreach ($object_names as $object_name) {
      if (!isset($exportables[$object_name])) {
        drush_log(dt('Invalid exportable: !exportable', array('!exportable' => $object_name)), 'error');
        unset($object_names[$object_name]);
      }
    }
    // Iterate through exportables to get just a list of selected ones.
    foreach (array_keys($exportables) as $exportable) {
      if (!in_array($exportable, $object_names)) {
        unset($exportables[$exportable]);
      }
    }
  }

  return $exportables;
}

/**
 * Return array of CTools exportable info based on available tables returned from
 * ctools_export_get_schemas().
 *
 * @param $table_names
 *  Array of table names to return.
 * @param $load
 *  (bool) should ctools exportable objects be loaded for each type.
 *  The default behaviour will load just a list of exportable names.
 *
 * @return
 *  Nested arrays of available exportables, keyed by table name.
 */
function _drush_ctools_export_info($table_names = array(), $load = FALSE) {
  ctools_include('export');
  // Get available schemas that declare exports.
  $schemas = ctools_export_get_schemas(TRUE);
  $exportables = array();

  if (!empty($schemas)) {
    // Remove types we don't want, if any.
    if (!empty($table_names)) {
      foreach (array_keys($schemas) as $table_name) {
        if (!in_array($table_name, $table_names)) {
          unset($schemas[$table_name]);
        }
      }
    }
    // Load array of available exportables for each schema.
    foreach ($schemas as $table_name => $schema) {
      // Load all objects.
      if ($load) {
        $exportables[$table_name] = ctools_export_crud_load_all($table_name);
      }
      // Get a list of exportable names.
      else {
        if (!empty($schema['export']['list callback']) && function_exists($schema['export']['list callback'])) {
          $exportables[$table_name] = $schema['export']['list callback']();
        }
        else {
          $exportables[$table_name] = ctools_export_default_list($table_name, $schema);
        }
      }
    }
  }

  return $exportables;
}

/*
 * View a single object.
 *
 * @param $table_name
 * @param $object
 */
function _drush_ctools_export_view($table_name, $object) {
  $indent = drush_get_option('indent', '');
  $no_colour = drush_get_option('no-colour', FALSE);
  $export = ctools_export_crud_export($table_name, $object, $indent);
  if ($no_colour) {
    drush_print($export);
  }
  else {
    drush_print(shellColours::getColouredOutput("\n$export", 'light_green'));
  }
}

/*
 * Revert a single object.
 *
 * @param $table_name
 * @param $object
 */
function _drush_ctools_export_delete($table_name, $object) {
  if (_ctools_drush_object_is_overridden($object)) {
    // Remove from db.
    ctools_export_crud_delete($table_name, $object);
    drush_log("Reverted object: $object->name", 'success');
  }
  else {
    drush_log("Nothing to revert for: $object->name", 'notice');
  }
}

/*
 * Enable a single object.
 *
 * @param $table_name
 * @param $object
 */
function _drush_ctools_export_enable($table_name, $object) {
  if (_ctools_drush_object_is_disabled($object)) {
    // Enable object.
    ctools_export_crud_enable($table_name, $object);
    drush_log("Enabled object: $object->name", 'success');
  }
  else {
    drush_log("$object->name is already Enabled", 'notice');
  }
}

/*
 * Disable a single object.
 *
 * @param $table_name
 * @param $object
 */
function _drush_ctools_export_disable($table_name, $object) {
  if (!_ctools_drush_object_is_disabled($object)) {
    // Disable object.
    ctools_export_crud_disable($table_name, $object);
    drush_log("Disabled object: $object->name", 'success');
  }
  else {
    drush_log("$object->name is already disabled", 'notice');
  }
}

/**
 * Helper to determine if an object is disabled.
 *
 * @param $object
 *  Loaded CTools exportable object.
 *
 * @return TRUE or FALSE
 */
function _ctools_drush_object_is_disabled($object) {
  return (isset($object->disabled) && ($object->disabled == TRUE)) ? TRUE : FALSE;
}

/**
 * Helper to determine if an object is overridden.
 *
 * @param $object
 *  Loaded CTools exportable object.
 *
 * @return TRUE or FALSE
 */
function _ctools_drush_object_is_overridden($object) {
  return ($object->export_type == 3) ? TRUE : FALSE;
}

/**
 * Helper to determine if an object is only in the db.
 *
 * @param $object
 *  Loaded CTools exportable object.
 *
 * @return TRUE or FALSE
 */
function _ctools_drush_object_is_db_only($object) {
  return ($object->export_type == 1) ? TRUE : FALSE;
}

/**
 * Return any aliases for an op, that will be used to show as output.
 * For now, this is mainly necessary for delete => revert alias.
 *
 * @param $op
 *  The op name. Such as 'enable', 'disable', or 'delete'.
 *
 * @return
 *  The matched alias value or the original $op passed in if not found.
 */
function _drush_ctools_export_op_aliases($op) {
  $aliases = array(
    'delete' => 'revert',
  );

  if (isset($aliases[$op])) {
    return $aliases[$op];
  }

  return $op;
}

/**
 * Class to deal with wrapping output strings with
 * colour formatting for the shell.
 */
class shellColours {

  private static $foreground_colours = array(
    'black' => '0;30',
    'dark_gray' => '1;30',
    'blue' => '0;34',
    'light_blue' => '1;34',
    'green' => '0;32',
    'light_green' => '1;32',
    'cyan' => '0;36',
    'light_cyan' => '1;36',
    'red' => '0;31',
    'light_red' => '1;31',
    'purple' => '0;35',
    'light_purple' => '1;35',
    'brown' => '0;33',
    'yellow' => '1;33',
    'light_gray' => '0;37',
    'white' => '1;37',
  );

  private static $background_colours = array(
    'black' => '40',
    'red' => '41',
    'green' => '42',
    'yellow' => '43',
    'blue' => '44',
    'magenta' => '45',
    'cyan' => '46',
    'light_gray' => '47',
  );

  private function __construct() {}

  // Returns coloured string
  public static function getColouredOutput($string, $foreground_colour = NULL, $background_colour = NULL) {
    $coloured_string = "";

    // Check if given foreground colour found
    if ($foreground_colour) {
      $coloured_string .= "\033[" . self::$foreground_colours[$foreground_colour] . "m";
    }
    // Check if given background colour found
    if ($background_colour) {
      $coloured_string .= "\033[" . self::$background_colours[$background_colour] . "m";
    }

    // Add string and end colouring
    $coloured_string .=  $string . "\033[0m";

    return $coloured_string;
  }

  // Returns all foreground colour names
  public static function getForegroundColours() {
    return array_keys(self::$foreground_colours);
  }

  // Returns all background colour names
  public static function getBackgroundColours() {
    return array_keys(self::$background_colours);
  }

} // shellColours
