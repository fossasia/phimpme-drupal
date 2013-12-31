<?php

/**
 * @file
 * Test pages for the File Entity Test module.
 */

/**
 * Form callback; upload a file.
 */
function file_entity_test_add_form($form, &$form_state) {
  $form['file'] = array(
    '#type' => 'managed_file',
    '#required' => TRUE,
    '#title' => 'File',
    '#upload_location' => 'public://',
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form submit callback; save the uploaded file.
 */
function file_entity_test_add_form_submit($form, &$form_state) {
  $file = file_load($form_state['values']['file']);
  if (!$file->status) {
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
  }
  drupal_set_message(t('Your file has been saved.'));
  $form_state['redirect'] = 'file-entity-test/file/' . $file->fid;
}

/**
 * Page callback; view a file.
 */
function file_entity_test_view_page($file) {
  return file_view($file, 'default');
}

/**
 * Page callback; preview a file.
 */
function file_entity_test_preview_page($file) {
  return file_view($file, 'file_entity_test_preview');
}

/**
 * Form callback; edit a file.
 */
function file_entity_test_edit_form($form, &$form_state, $file) {
  $form_state['file'] = $file;
  field_attach_form('file', $file, $form, $form_state);
  $form['file'] = file_view($file, 'file_entity_test_preview');

  // Add internal file properties needed by
  // file_entity_test_edit_form_validate().
  foreach (array('fid', 'type') as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $file->$key);
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form validation handler for the file edit form.
 */
function file_entity_test_edit_form_validate($form, &$form_state) {
  entity_form_field_validate('file', $form, $form_state);
}

/**
 * Form submit handler for the file edit form
 */
function file_entity_test_edit_form_submit($form, &$form_state) {
  $file = $form_state['file'];
  entity_form_submit_build_entity('file', $file, $form, $form_state);
  file_save($file);
  drupal_set_message(t('Your changes to the file have been saved.'));
  $form_state['redirect'] = 'file-entity-test/file/' . $file->fid;
}
