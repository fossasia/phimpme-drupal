<?php

$plugin = array(
  'title' => t('Entity extra field'),
  'defaults' => array('view_mode' => NULL),
  'content type' => 'ctools_entity_field_extra_content_type_content_type',
);

/**
 * Just one subtype.
 *
 * Ordinarily this function is meant to get just one subtype. However, we are
 * using it to deal with the fact that we have changed the subtype names. This
 * lets us translate the name properly.
 */
function ctools_entity_field_extra_content_type_content_type($subtype) {
  $types = ctools_entity_field_extra_content_type_content_types();
  if (isset($types[$subtype])) {
    return $types[$subtype];
  }
}

/**
 * Return all extra field content types available.
 */
function ctools_entity_field_extra_content_type_content_types() {
  // This will hold all the individual field content types.
  $types = array();
  $context_types = array();
  $entities = entity_get_info();

  foreach ($entities as $entity_type => $entity) {
    foreach ($entity['bundles'] as $type => $bundle) {
      foreach (field_info_extra_fields($entity_type, $type, 'display') as $field_name => $info) {
        if (!isset($types[$entity_type . ':' . $field_name])) {
          $types[$entity_type . ':' . $field_name] = array(
            'category' => t(ucfirst($entity_type)),
            'icon' => 'icon_field.png',
            'title' => $info['label'],
            'description' => isset($info['description']) ? $info['description'] : '',
          );
        }
        $context_types[$entity_type . ':' . $field_name]['types'][$type] = $bundle['label'];
      }
    }
  }

  // Create the required context for each field related to the bundle types.
  foreach ($types as $key => $field_content_type) {
    list($entity_type, $field_name) = explode(':', $key, 2);
    $types[$key]['required context'] = new ctools_context_required(t(ucfirst($entity_type)), $entity_type, array(
      'type' => array_keys($context_types[$key]['types']),
    ));
    unset($context_types[$key]['types']);
  }

  return $types;
}

/**
 * Returns an edit form for an extra field.
 */
function ctools_entity_field_extra_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $subtype = $form_state['subtype_name'];
  list($entity_type, $field_name) = explode(':', $subtype, 2);

  $info = entity_get_info($entity_type);
  $view_mode_options = array();
  foreach ($info['view modes'] as $mode => $option) {
    $view_mode_options[$mode] = $option['label'];
  }

  $form['view_mode'] = array(
    '#title' => t('View mode'),
    '#type' => 'select',
    '#description' => t('Select a view mode for this extra field.'),
    '#options' => $view_mode_options,
    '#default_value' => $conf['view_mode'],
  );

  return $form;
}

function ctools_entity_field_extra_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['view_mode'] = $form_state['values']['view_mode'];
}

/**
 * Render the extra field.
 */
function ctools_entity_field_extra_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return;
  }
  // Get a shortcut to the entity.
  $entity = clone $context->data;
  list($entity_type, $field_name) = explode(':', $subtype, 2);
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Invoke the view-hook to get the extra field.
  $entity->content = array();
  $langcode = $GLOBALS['language_content']->language;

  module_invoke_all($entity_type . '_view', $entity, $conf['view_mode'], $langcode);
  module_invoke_all('entity_view', $entity, $entity_type, $conf['view_mode'], $langcode);

  if (isset($entity->content[$field_name])) {
    // Build the content type block.
    $block = new stdClass();
    $block->module  = 'entity_field_extra';
    $block->content = $entity->content[$field_name];
    $block->delta   = $id;
    return $block;
  }
}

function ctools_entity_field_extra_content_type_admin_title($subtype, $conf, $context) {
  $info = ctools_entity_field_extra_content_type_content_type($subtype);
  return t('"@s" @field', array('@s' => $context->identifier, '@field' => $info['title']));
}
