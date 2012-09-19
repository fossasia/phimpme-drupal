<?php

/**
 * @file
 * This file contains the admin functions for the Media module.
 */

/**
 * Include media.pages.inc since it has some form definitions we will use.
 */

require_once dirname(__FILE__) . '/media.pages.inc';

/**
 * Display the list or thumbnails media admin display.
 */
function media_admin($form, $form_state) {
  global $user;

  $path = drupal_get_path('module', 'media');

  $form['#attached'] = array(
    'js' => array($path . '/js/media.admin.js'),
    'css' => array($path . '/css/media.css'),
  );

  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    $form['#attributes']['class'][] = "media-list-operation";
    return media_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['files']), 'admin/content/media', 'admin/content/media');
  }

  require_once dirname(__FILE__) . '/media.browser.inc';
  media_attach_browser_js($form);

  $types = media_display_types();

  if (arg(3)) {
    $display = arg(3);
    if (!in_array($display, array_keys($types))) {
      exit(drupal_not_found());
    }
    // Save their preference.
    db_merge('media_list_type')
      ->key(array('uid' => $user->uid))
      ->fields(array(
          'type' => $display,
      ))
      ->execute();
  }
  else {
    $display = db_query("SELECT type FROM {media_list_type} WHERE uid = :uid", array(':uid' => $user->uid))->fetch();
    if (!$display) {
      $display = 'list';
    }
    else {
      $display = $display->type;
    }
  }

  // Build the display switch.
  $form['switch'] = media_admin_display_switch(array('active display' => $display));

  // Build the 'Media operations' form.
  $options = array();
  foreach (module_invoke_all('media_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Operations'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#access' => !empty($options),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => 'delete',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#submit' => array('media_admin_submit'),
    '#validate' => array('media_admin_validate'),
  );

  include_once $types[$display]['file'];

  $form['admin'] = $types[$display]['callback']($form);

  return $form;
}


/**
 * Form builder: Builds the media list administration overview.
 */
function media_admin_list(&$parent_form) {
  // @todo Change to media_variable_get('admin_pager_limit') for consistency
  //   with browser_pager_limit?
  $limit = variable_get('media_admin_limit', 50);

  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'f.filename'),
    'type' => array('data' => t('Type'), 'field' => 'f.filemime'),
    'size' => array('data' => t('Size'), 'field' => 'f.filesize'),
    'author' => array('data' => t('Author'), 'field' => 'u.name'),
    'timestamp' => array('data' => t('Updated'), 'field' => 'f.timestamp', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('file_managed', 'f')->extend('PagerDefault')->extend('TableSort');
  $query->join('users', 'u', 'f.uid = u.uid');
  $query->fields('f', array('fid'));
  $query->fields('u', array('uid'));
  $query->condition('f.status', FILE_STATUS_PERMANENT);
  $query->limit($limit);
  $query->orderByHeader($header);

  foreach (array_keys(media_get_hidden_stream_wrappers()) as $name) {
    $query->condition('f.uri', $name . '%', 'NOT LIKE');
  }

  // Result array keys are file IDs, values are the file owner's UIDs.
  $result = $query->execute()->fetchAllKeyed();

  // Hide the operations form if there are no files to operate on.
  $parent_form['options']['#access'] &= !empty($result);

  // Load all the file entities.
  $files = $form['#files'] = file_load_multiple(array_keys($result));

  // Load all the file owner user entities to display usernames.
  $accounts = $form['#accounts'] = user_load_multiple(array_unique($result));

  $destination = drupal_get_destination();
  $options = array();
  foreach ($files as $file) {
    $options[$file->fid] = array(
      'title' => theme('media_link', array('file' => $file)),
      'type' =>  check_plain($file->filemime),
      'size' => format_size($file->filesize),
      'author' => theme('username', array('account' => $accounts[$file->uid])),
      'timestamp' => format_date($file->timestamp, 'short'),
    );
    $options[$file->fid]['operations'] = l(t('Edit'), 'media/' . $file->fid . '/edit', array('query' => $destination));
  }

  $form['files'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No media available.'),
    '#attributes' => array('class' => array('media-display-table', 'media-clear')),
  );
  $form['pager'] = array('#markup' => theme('pager', array('tags' => NULL)));

  return $form;
}

/**
 * Form builder: Builds the media thumbnails administration overview.
 */
function media_admin_thumbnails(&$parent_form) {
  // @todo Change to media_variable_get('admin_pager_limit') for consistency
  //   with browser_pager_limit?
  $limit = variable_get('media_admin_limit', 50);

  $query = db_select('file_managed', 'f')->extend('PagerDefault');
  $query->fields('f', array('fid'));
  $query->condition('f.status', FILE_STATUS_PERMANENT);
  $query->limit($limit);
  $query->orderBy('f.timestamp', 'DESC');

  foreach (array_keys(media_get_hidden_stream_wrappers()) as $name) {
    $query->condition('f.uri', $name . '%', 'NOT LIKE');
  }

  $fids = $query->execute()->fetchCol();
  $files = file_load_multiple($fids);
  $files = array();

  // Hide the operations form if there are no files to operate on.
  $parent_form['options']['#access'] &= !empty($files);

  if (empty($files)) {
    // Display empty text if there are no files.
    $form['files'] = array(
      '#markup' => '<p>' . t('No media available.') . '</p>',
    );
  }
  else {
    $form['files'] = array(
      '#tree' => TRUE,
      '#prefix' => '<div class="media-display-thumbnails media-clear clearfix"><ul class="media-list-thumbnails">',
      '#suffix' => '</ul></div>',
    );

    foreach ($files as $file) {
      $preview = media_get_thumbnail_preview($file, TRUE);
      if (!isset($preview['#file'])) { dpm($file); }
      $form['files'][$file->fid] = array(
        '#type' => 'checkbox',
        '#title' => '',
        '#prefix' => '<li>' . drupal_render($preview),
        '#suffix' => '</li>',
      );
    }

    $form['pager'] = array('#markup' => theme('pager', array('tags' => NULL)));
  }

  return $form;
}

/**
 * Build the display switch portion of the file listings form.
 */
function media_admin_display_switch($options = array()) {
  $options += array(
    'form location' => 'admin/content/media',
    'active display' => 'list',
  );
  $display_types = media_display_types();

  // Build the item list.
  $display_items = array();
  foreach ($display_types as $delta => $item) {
    $attributes = array('title' => $item['description']);

    // Set a seperate icon for the active item.
    if ($delta == $options['active display']) {
      $icon = $item['icon_active'];
      $attributes['class'][] = 'active';
    }
    else {
      $icon = $item['icon'];
    }

    $display_items[] = array(
      'data' => l(theme('image', array('path' => $icon, 'alt' => $item['title'])),
        $options['form location'] . '/' . $delta,
        array(
          'html' => TRUE,
          'attributes' => $attributes,
        )),
    );
  }

  return array(
    '#type' => 'markup',
    '#markup' => theme('item_list', array(
       'items' => $display_items,
       'attributes' => array('class' => 'media-display-switch'),
      )
    ),
  );
}

/**
 * Validate media_admin_list form submissions.
 *
 * Check if any files have been selected to perform the chosen
 * 'Update option' on.
 */
function media_admin_validate($form, &$form_state) {
  $files = array_filter($form_state['values']['files']);
  if (count($files) == 0) {
    form_set_error('', t('No items selected.'));
  }
}

/**
 * Process media_admin_list form submissions.
 *
 * Execute the chosen 'Update option' on the selected files.
 */
function media_admin_submit($form, &$form_state) {
  $operations = module_invoke_all('media_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked nodes
  $files = array_filter($form_state['values']['files']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($files), $operation['callback arguments']);
    }
    else {
      $args = array($files);
    }
    call_user_func_array($function, $args);
    cache_clear_all();
  }
  elseif (!empty($operation['redirect'])) {
    $fids = implode(' ', array_keys(array_filter($form_state['values']['files'])));
    $form_state['redirect'] = array(str_replace('%fids', $fids, $operation['redirect']), array('query' => array('destination' => 'admin/content/media')));
  }
  else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 *  The administration form for managing media types.
 */
function media_admin_type_manage_form($form, &$form_state, $media_type) {
  $form = array();
  $form['media_type'] = array(
    '#type' => 'value',
    '#value' => $media_type->name,
  );

  // If this Media type is handled by us, then we can put in some default
  // options. Otherwise, we leave it to the implementing module to form_alter.
  if ($media_type->type_callback == 'media_is_type') {
    // Options for match_type.
    $options = array(
      'all' => t('All'),
      'any' => t('Any'),
      'other' => t('Other'),
    );
    if ($media_type->type_callback_args['match_type'] && isset($options[$media_type->type_callback_args['match_type']])) {
      $default_value = $media_type->type_callback_args['match_type'];
      $other_default_value = '';
    }
    else {
      $default_value = 'other';
      $other_default_value = $media_type->type_callback_args['match_type'];
    }
    $form['match_type'] = array(
      '#type' => 'radios',
      '#title' => t('Match type'),
      '#options' => $options,
      '#default_value' => $default_value,
    );
    $form['match_type_other'] = array(
      '#type' => 'textfield',
      '#title' => t('Other match type value'),
      '#default_value' => $other_default_value,
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'media') . '/js/media.admin.js'),
      ),
    );

    // Options for allowed Streams.
    $options = array('public' => t('Public files'), 'private' => t('Private files'));
    foreach (file_get_stream_wrappers() as $stream => $wrapper) {
      $options[$stream] = $wrapper['name'];
    }
    unset($options['temporary']);
    $default_value = array();
    if (isset($media_type->type_callback_args['streams'])) {
      foreach ($media_type->type_callback_args['streams'] as $stream) {
        $default_value[$stream] = $stream;
      }
    }
    $form['streams'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Allowed streams'),
      '#options' => $options,
      '#default_value' => $default_value,
    );

    // Options for allowed mimetypes & extensions.
    $default_value = isset($media_type->type_callback_args['mimetypes']) ? implode(' ', $media_type->type_callback_args['mimetypes']) : '';
    $form['mimetypes'] = array(
      '#type' => 'textfield',
      '#title' => t('Allowed mimetypes'),
      '#description' => t('You may optionally enter one or more allowed file mimetypes for this Media type, if appropriate, separating each with a space. You may use a regular expression for matching, such as %image_match (which would match any mimetype beginning with %image) or %any_match, which would match any file mimetype.', array('%image_match' => '/^image/', '%image' => t('image'), '%any_match' => '/.*/')),
      '#default_value' => check_plain($default_value),
    );
    $default_value = isset($media_type->type_callback_args['extensions']) ? implode(' ', $media_type->type_callback_args['extensions']) : '';
    $form['extensions'] = array(
      '#type' => 'textfield',
      '#title' => t('Allowed extensions'),
      '#description' => t('You may optionally enter one or more allowed file extensions for this Media type, if appropriate, separating each with a space (and no dots).'),
      '#default_value' => check_plain($default_value),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 100,
  );
  return $form;
}

function media_admin_type_manage_form_submit($form, &$form_state) {
  $media_type = media_type_load($form_state['values']['media_type']);
  // Reset all values to empty.
  $media_type->type_callback_args = array();

  // What is the logic of the match (AND / OR).
  if ($form_state['values']['match_type']) {
    $media_type->type_callback_args['match_type'] = $form_state['values']['match_type'];
  }
  else {
    $media_type->type_callback_args['match_type'] = $form_state['values']['match_type_other'];
  }

  // Which streams are valid for this type.
  $media_type->type_callback_args['streams'] = array();
  foreach ($form_state['values']['streams'] as $stream) {
    if ($stream) {
      $media_type->type_callback_args['streams'][] = $stream;
    }
  }

  // Which mimetypes are valid for this type.
  if (trim($form_state['values']['mimetypes'])) {
    $media_type->type_callback_args['mimetypes'] = explode(' ', $form_state['values']['mimetypes']);
    array_walk($media_type->type_callback_args['mimetypes'], 'trim');
    array_filter($media_type->type_callback_args['mimetypes']);
  }

  // Which file extensions are valid for this type.
  if (trim($form_state['values']['extensions'])) {
    $media_type->type_callback_args['extensions'] = explode(' ', $form_state['values']['extensions']);
    array_walk($media_type->type_callback_args['extensions'], 'trim');
    array_filter($media_type->type_callback_args['extensions']);
  }

  media_type_save($media_type);
  drupal_set_message(t('The @label media type has been saved.', array('@label' => $media_type->label)));
}

/**
 * Form callback for mass import.
 */
function media_import($form, &$form_state) {
  if (!isset($form_state['storage']['files'])) {
    $form_state['storage']['step'] = 'choose';
    $form_state['storage']['next_step'] = 'preview';
    $form['directory'] = array(
      '#type' => 'textfield',
      '#title' => t('Directory'),
      '#required' => TRUE,
    );

    $form['pattern'] = array(
      '#type' => 'textfield',
      '#title' => t('Pattern'),
      '#description' => 'Only files matching this pattern will be imported. For example, to import all jpg and gif files, the pattern would be <em>*.jpg|*.gif</em>.',
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Preview')
    );
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/media',
    );
  }
  else {
    $form['preview'] = array(
      '#markup' => theme('item_list', array('items' => $form_state['storage']['files'])),
    );

    $form = confirm_form($form, t('Import these files?'), 'admin/content/media/import');
  }
  return $form;

}

/**
 * Validate handler for media_import().
 */
function media_import_validate($form, &$form_state) {
  if ($form_state['values']['op'] != t('Confirm')) {
    $directory = $form_state['values']['directory'];
    $pattern = $form_state['values']['pattern'];
    if (!is_dir($directory)) {
      form_set_error('directory', t('The provided directory does not exist.'));
    }
    $pattern = !empty($pattern) ? $pattern :  '*';
    $files = glob("$directory/$pattern");
    if (empty($files)) {
      form_set_error('pattern', t('No files were found in %directory matching %pattern', array('%directory' => $directory, '%pattern' => $pattern)));
    }
    $form_state['storage']['files'] = $files;
  }
}

/**
 * Submit handler for media_import().
 */
function media_import_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Confirm')) {
    $files = $form_state['storage']['files'];
    $batch = array(
      'title' => t('Importing'),
      'operations' => array(
        array('media_import_batch_import_files', array($files)),
      ),
      'finished' => 'media_import_batch_import_complete',
      'file' => drupal_get_path('module', 'media') . '/includes/media.admin.inc',
    );
    batch_set($batch);
    return;

  }
  $form_state['rebuild'] = TRUE;
}

/**
 * BatchAPI callback op for media import.
 */
function media_import_batch_import_files($files, &$context) {
  if (!isset($context['sandbox']['files'])) {
    // This runs the first time the batch runs.
    // This is stupid, but otherwise, I don't think it will work...
    $context['results'] = array('success' => array(), 'errors' => array());
    $context['sandbox']['max'] = count($files);
    $context['sandbox']['files'] = $files;
  }
  $files =& $context['sandbox']['files'];

  // Take a cut of files.  Let's do 10 at a time.
  $length = (count($files) > media_variable_get('import_batch_size')) ? media_variable_get('import_batch_size') : count($files);
  $to_process = array_splice($files, 0, $length);
  $image_in_message = '';

  foreach ($to_process as $file) {
    try {
      $file_obj = media_parse_to_file($file);
      $context['results']['success'][] = $file;
      if (!$image_in_message) {
        // @todo Is this load step really necessary? When there's time, test
        //   this, and either remove it, or comment why it's needed.
        $loaded_file = file_load($file_obj->fid);
        $image_in_message = file_view_file($loaded_file, 'media_preview');
      }
    }
    catch (Exception $e) {
      $context['results']['errors'][] = $file . " Reason: " . $e->getMessage();
    }
  }

  $context['message'] = "Importing " . theme('item_list', array('items' => $to_process));
  $context['message'] .= drupal_render($image_in_message); // Just for kicks, show an image we are importing

  $context['finished'] = ($context['sandbox']['max'] - count($files)) / $context['sandbox']['max'];

}

/**
 * BatchAPI complete callback for media import.
 */
function media_import_batch_import_complete($success, $results, $operations) {
  if ($results['errors']) {
    drupal_set_message(theme('item_list', array('items' => $results['errors'])), 'error');
  }
  if ($results['success']) {
    drupal_set_message(theme('item_list', array('items' => $results['success'])));
  }
}

/**
 * Admin configruation form for media browser settings.
 */
function media_admin_config_browser($form, &$form_state) {
  $theme_options = array();
  $theme_options[NULL] = 'Default administration theme';
  foreach (list_themes() as $key => $theme) {
    if ($theme->status) {
      $theme_options[$key] = $theme->info['name'];
    }
  }

  $form[media_variable_name('dialog_theme')] = array(
    '#type' => 'select',
    '#title' => t('Media browser theme'),
    '#options' => $theme_options,
    '#description' => t("This theme will be used for all media related dialogs.  It can be different from your site's theme because many site themes do not work well in the small windows which media uses."),
    '#default_value' => media_variable_get('dialog_theme'),
  );

  $form[media_variable_name('file_extensions')] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed file extensions'),
    '#default_value' => media_variable_get('file_extensions'),
    '#description' => t('File extensions which are accepted in the media browser.  Use spaces to separate (e.g. "jpg gif pdf doc"). <br/><em>Note that this can be overriden on a per-field basis when creating multimedia asset fields and files of certain extensions cannot be embedded at this time.</em>'),
  );

  $form['#submit'][] = 'media_admin_config_browser_pre_submit';
  return system_settings_form($form);
}

function media_admin_config_browser_pre_submit(&$form, &$form_state) {
  if (!$form_state['values'][media_variable_name('dialog_theme')]) {
    media_variable_del('dialog_theme');
    unset($form_state['values'][media_variable_name('dialog_theme')]);
  }
}

/**
 * Confirmation form for rebuliding the file_managed table to include type
 * in rows where there is no type.
 */
function media_admin_rebuild_types_form($form, &$form_state) {
  $total = media_type_invalid_files_count();
  if ($total == 0) {
    media_variable_del('show_file_type_rebuild_nag');
    // @TODO: Make this not sound stupid.
    drupal_set_message(t('All files in the system have been assigned types. Media installation complete.'));
    drupal_goto('admin');
  }
  $form['total'] = array('#type' => 'value', '#value' => $total);

  return confirm_form(
    $form,
    t('Update types for existing files'),
    'admin/config/media',
    t('This process is required when installing media on an existing site. Media needs to scan through existing files and identify the file type. <br/><strong>Update types for @file files?</strong>', array('@total' => $total))
  );
}

/**
 * @see media_admin_rebuild_types_form()
 */
function media_admin_rebuild_types_form_submit(&$form, &$form_state) {
  $total = $form_state['values']['total'];

  $batch = array(
    'title' => t('Rebuilding type information for ' . $total . ' files'),
    'operations' => array(
      array('media_admin_rebuild_types_batch_op', array($total)),
    ),
    'finished' => 'media_admin_rebuild_types_batch_complete',
    'file' => drupal_get_path('module', 'media') . '/includes/media.admin.inc',
  );
  batch_set($batch);
}

/**
 * Batch operation for fixing the file_managed table for media, adding type values
 * where no value exists.
 */
function media_admin_rebuild_types_batch_op($total, &$context) {
  $per_run = media_variable_get('media_type_batch_update_per_run', 100);
  $context['results'] = array_merge($context['results'], media_type_batch_update(FALSE, $per_run));
  $context['finished'] = count($context['results']) / $total;
}
/**
 * Sets a message informing the user how many file records were updated.
 */
function media_admin_rebuild_types_batch_complete($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One file identified and given a type.', '@count files identified and given a type.');
    media_variable_del('show_file_type_rebuild_nag');
  }
  drupal_set_message($message);
}
