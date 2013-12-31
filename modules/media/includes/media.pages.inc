<?php

/**
 * @file
 * Common pages for the Media module.
 */

/**
 * Menu callback; view a single file entity.
 */
function media_view_page($file) {
  // @todo Implement granular editorial access: http://drupal.org/node/696970.
  //   In the meantime, protect information about private files from being
  //   discovered by unprivileged users. File IDs are autoincrement, so one can
  //   attempt discovery by trying to access different media/ID paths. See also
  //   media_browser_list(). This logic potentially belongs within
  //   media_access(), but that would require extending that function's
  //   signature to accept a $file paramter, and this is temporary code anyway.
  if (!user_access('administer media') && (file_uri_scheme($file->uri) === 'private')) {
    return MENU_ACCESS_DENIED;
  }

  drupal_set_title($file->filename);
  return file_view($file, 'media_original');
}

/**
 * Menu callback; presents the Media editing form.
 */
function media_page_edit($file) {
  drupal_set_title(t('<em>Edit @type</em> @title', array('@type' => ($file->type != FILE_TYPE_NONE ? $file->type : ''), '@title' => $file->filename)), PASS_THROUGH);
  return drupal_get_form('media_edit', $file);
}

/**
 * Menu callback; presents the Media editing form for multiple file entities.
 */
function media_page_multiedit($files) {
  if (!module_exists('multiform')) {
    drupal_set_message(t('To edit multiple media items, you must install the multiform module.'));
  }
  $i = 0;
  $forms = array();
  foreach ($files as $file) {
    // To maintain unique form_ids, increment this counter.
    // @see media_forms().
    $i++;
    $forms[] = array("media_edit_$i", $file);
  }

  $form = call_user_func_array('multiform_get_form', $forms);
  $form['#attributes']['class'][] = 'media-multiedit-form';

  unset($form['buttons']['Delete']);
  // Would be nice to add this to show a message, but not working.
  // Can debug.
  //$form['buttons']['Save']['#submit'][] = 'media_page_multiedit_submit';

  drupal_set_title(t('Editing multiple media files'));
  return $form;
}

/**
 * Menu callback; shows delete confirmation form.
 */
function media_page_delete($file) {
  drupal_set_title(t('<em>Delete @type</em> @title', array('@type' => ($file->type != FILE_TYPE_NONE ? $file->type : ''), '@title' => $file->filename)), PASS_THROUGH);
  // Don't bother showing the form if the item is in use, since we won't allow
  // them to delete it anyway.
  $references = file_usage_list($file);
  if (!empty($references)) {
    return t('The file %title is in use and cannot be deleted.', array('%title' => $file->filename));
  }
  else {
    $files = array($file->fid => $file);
    return drupal_get_form('media_multiple_delete_confirm', $files, '<front>', 'media/' . $file->fid);
  }
}

/**
 * Confirm the request to delete files.
 */
function media_multiple_delete_confirm($form, &$form_state, $files, $redirect_on_success = NULL, $redirect_on_cancel = NULL) {
  $form['files'] = array('#tree' => TRUE);
  $form['file_titles'] = array('#theme' => 'item_list');
  foreach ($files as $fid => $value) {
    $title = db_query('SELECT filename FROM {file_managed} WHERE fid = :fid', array(':fid' => $fid))->fetchField();
    $form['files'][$fid] = array(
      '#type' => 'value',
      '#value' => $fid,
    );
    $form['file_titles']['#items'][] = check_plain($title);
  }
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');
  if (isset($redirect_on_success)) {
    $form['redirect_on_success'] = array(
      '#type' => 'value',
      '#value' => $redirect_on_success,
    );
  }
  $form['#submit'][] = 'media_multiple_delete_confirm_submit';

  $confirm_question = format_plural(count($files),
                                  'Are you sure you want to delete this item?',
                                  'Are you sure you want to delete these items?');

  return confirm_form($form,
    $confirm_question,
    isset($redirect_on_cancel) ? $redirect_on_cancel : current_path(),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel'));
}

/**
 * Attempt to delete files and notify the user of the result.
 */
function media_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $results = array();
    $files = array_keys($form_state['values']['files']);
    foreach ($files as $fid) {
      $file = file_load($fid);
      $files[$fid] = $file;
      $results[$fid] = file_delete($file);
    }
    // The result of file_delete can be an array if the file is in use, or TRUE/FALSE.
    foreach ($results as $fid => $result) {
      if (is_array($result)) {
        drupal_set_message(t('The file @title is in use and cannot be deleted.', array('@title' => $files[$fid]->filename)), 'warning');
      }
      elseif (!$result) {
        drupal_set_message(t('The file @title was not deleted due to an error.', array('@title' => $files[$fid]->filename)), 'error');
      }
      else {
        $message = t('File @title was deleted', array('@title' => $files[$fid]->filename));
        watchdog('media', $message);
        drupal_set_message($message);
      }
    }
    if (isset($form_state['values']['redirect_on_success'])) {
      $form_state['redirect'] = $form_state['values']['redirect_on_success'];
    }
  }
}


/**
 * Form callback for adding media via an upload form.
 * @todo: should use the AJAX uploader
 */
function media_add_upload($form, &$form_state, $params = array()) {
  // Set up file upload validators.
  $validators = array();

  // Validate file extensions. If there are no file extensions in $params and
  // there are no Media defaults, there is no file extension validation.
  if (!empty($params['file_extensions'])) {
    $validators['file_validate_extensions'] = array($params['file_extensions']);
  }
  elseif ($tmp = media_variable_get('file_extensions')) {
    $validators['file_validate_extensions'] = array($tmp);
  }

  // Validate file size but do not allow anything higher than file_upload_max_size().
  $max_filesize = file_upload_max_size();
  if (!empty($params['max_filesize']) && $params['max_filesize'] < $max_filesize) {
    $validators['file_validate_size'] = array(parse_size($params['max_filesize']));
  }
  elseif (($tmp = media_variable_get('max_filesize')) && $tmp < $max_filesize) {
    $validators['file_validate_size'] = array(parse_size($tmp));
  }
  else {
    $validators['file_validate_size'] = array($max_filesize);
  }

  // Add image validators.
  $params += array('min_resolution' => 0, 'max_resolution' => 0);
  if ($params['min_resolution'] || $params['max_resolution']) {
    $validators['file_validate_image_resolution'] = array($params['max_resolution'], $params['min_resolution']);
  }

  $form['#validators'] = $validators;

  $form['upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload a new file'),
    '#description' => theme('file_upload_help', array('description' => '', 'upload_validators' => $validators)),
    '#upload_validators' => $validators,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Validate the generic file upload with the global media settings.
 */
function media_add_upload_validate($form, &$form_state) {
  // Save the file as a temporary file.
  $file = file_save_upload('upload', $form['#validators']);

  if ($file === NULL) {
    form_set_error('upload', t("No file appears to have been selected."));
  }
  elseif ($file === FALSE) {
    form_set_error('upload', t('File upload error.'));
  }
  else {
    $form_state['values']['upload'] = $file;
  }
}

/**
 * Upload a file.
 */
function media_add_upload_submit($form, &$form_state) {
  $params = $form_state['build_info']['args'][0];
  $file = $form_state['values']['upload'];

  // The media browser widget does not use the 'display' field.
  $file->display = TRUE;

  // Change the file status from temporary to permanent.
  _media_save_file_permenently($file);

  // Determine what URI scheme this file should use.
  $scheme = !empty($params['uri_scheme']) && file_stream_wrapper_valid_scheme($params['uri_scheme']) ? $params['uri_scheme'] : file_default_scheme();
  $scheme .= '://';

  // Prepare the file's subdirectory path.
  $directory = '';
  if (!empty($params['file_directory'])) {
    $directory = token_replace($params['file_directory']) . '/';

    // If the directory isn't writable, or doesn't exist and can't be created,
    // the upload will fail.
    $prepare_directory = file_stream_wrapper_uri_normalize($scheme . $directory);
    if (!file_prepare_directory($prepare_directory, FILE_CREATE_DIRECTORY)) {
      drupal_set_message(t('The file directory @dir does not exist or is not writable. Please contact an administrator.', array('@dir' => $prepare_directory)), 'error');
      return;
    }
  }

  // Compose the file's permanent destination.
  $destination = file_stream_wrapper_uri_normalize($scheme . $directory . $file->filename);

  // Save the uploaded file to the permanent location.
  $file = file_move($file, $destination, FILE_EXISTS_RENAME);

  if ($file) {
    drupal_set_message(t('The file @name was uploaded', array('@name' => $file->filename)));
  }
  else {
    drupal_set_message(t('An error occurred and no file was uploaded.'), 'error');
    return;
  }

  $form_state['redirect'] = array('media/browser', array('query' => array('render' => 'media-popup', 'fid' => $file->fid)));
}

function media_add_upload_multiple($form, &$form_state, $params = array()) {
  $form = media_add_upload($form, $form_state, $params);
  unset($form['upload']['#title']);
  // The validators will be set from plupload anyway.  This isn't pretty, but don't
  // it to show up twice.
  unset($form['upload']['#description']);

  $form['upload']['#type'] = 'plupload';
  $form['submit']['#value'] = t('Start upload');
  return $form;
}

function media_add_upload_multiple_submit($form, &$form_state) {
  $scheme = variable_get('file_default_scheme', 'public') . '://';
  $saved_files = array();
  // We can't use file_save_upload() because of http://www.jacobsingh.name/content/tight-coupling-no-not
  foreach ($form_state['values']['upload'] as $uploaded_file) {
    if ($uploaded_file['status'] == 'done') {
      $source = $uploaded_file['tmppath'];
      $destination = file_stream_wrapper_uri_normalize($scheme . $uploaded_file['name']);
      // Rename it to its original name, and put it in its final home.
      // Note - not using file_move here because if we call file_get_mime
      // (in file_uri_to_object) while it has a .tmp extension, it horks.

      $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);

      $file = file_uri_to_object($destination);
      file_save($file);
      _media_save_file_permenently($file);

      $saved_files[] = $file;
    }
    else {
      // @todo: move this to element validate or something.
      form_set_error('pud', t('The specified file %name could not be uploaded.', array('%name' => $uploaded_file['name'])));
    }
  }
  // Get a list of fids to pass back.
  $fids = array();
  foreach ($saved_files as $file) {
    $fids[] = $file->fid;
  }
  $form_state['redirect'] = array('media/browser', array('query' => array('render' => 'media-popup', 'fid' => $fids)));
}

/**
 * Form builder: Builds the edit file form.
 */
function media_edit($form, $form_state, $file) {
  $form_state['file'] = $file;
  field_attach_form('file', $file, $form, $form_state);

  $form['#attached'] = array(
    'css' => array(drupal_get_path('module', 'media') . '/css/media.css'),
  );

  // Not sure about this class name, seems to indicate the style.
  $form['#attributes']['class'][] = 'media-image-left';
  $form['#attributes']['class'][] = 'media-edit-form';

  $form['preview'] = file_view_file($file, 'media_preview');
  $form['preview']['#weight'] = -10;
  $form['preview']['#suffix'] = '<div class="no-overflow">';

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['#prefix'] = '</div>';
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#weight' => 15,
    '#submit' => array('media_edit_delete_submit'),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('media_edit_submit'),
  );

  // Add internal file properties needed by media_edit_validate().
  foreach (array('fid', 'type') as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $file->$key);
  }

  return $form;
}

/**
 * Form validation handler for the media edit form.
 */
function media_edit_validate($form, &$form_state) {
  entity_form_field_validate('file', $form, $form_state);
}

/**
 * Form submit handler for the media submit form.
 */
function media_edit_submit($form, &$form_state) {
  $file = $form_state['file'];
  entity_form_submit_build_entity('file', $file, $form, $form_state);
  file_save($file);
  $form_state['redirect'] = 'media/' . $file->fid;
}

/**
 * Form submit handler for the Delete button on the media edit form.
 */
function media_edit_delete_submit($form, &$form_state) {
  $fid = $form_state['values']['fid'];
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $form_state['redirect'] = array('media/' . $fid . '/delete', array('query' => $destination));
}

function media_add_remote($form, &$form_state) {
  // Totally prototyping code to show designs.
  $form['sources'] = array(
    '#type' => 'vertical_tabs',
    '#title' => 'Sources',
  );

  $form['sources']['paste'] = array(
    '#type' => 'fieldset',
    '#title' => 'Paste URL or embed code',
  );


  $providers = '';
  $providers .= '<img style="height:50px; margin:20px" src="http://www.macuser.com/2008/10/09/top_youtube_logo_31_Dec_06.jpg">';
  $providers .= '<img style="height:50px; margin:20px" src="http://jasonhilldesign.com/FlikrLogo.jpg">';
  $form['sources']['paste']['code'] = array(
    '#type' => 'textarea',
    '#title' => t('URL or embed code'),
    '#description' => t('The following providers are supported: <br/>' . $providers),
  );

  $form['sources']['youtube'] = array(
    '#type' => 'fieldset',
    '#title' => 'YouTube',
    '#description' => t(''),
    '#attributes' => array('style' => 'height: 300px; overflow:auto'),
  );

  $form['sources']['flikr'] = array(
    '#type' => 'fieldset',
    '#title' => 'Flikr',
  );

  $box = '<div style="width:100px; height:100px; border:1px solid blue; padding:10px; float:left; margin:5px;"> Video </div>';
  $boxes = '';
  for ($i = 0; $i < 10; $i++) {
    $boxes .= $box;
  }
  $form['sources']['youtube']['stupid'] = array(
    '#markup' => $boxes,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
    '#attributes' => array('style' => 'float:right'),
    '#suffix' => '<br style="clear:both" />',
  );

  return $form;
}
