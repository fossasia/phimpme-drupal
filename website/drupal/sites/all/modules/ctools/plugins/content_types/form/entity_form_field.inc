<?php

/**
 * @file
 * Handle rendering entity fields as panes.
 */

$plugin = array(
  'title' => t('Entity field'),
  'defaults' => array('label' => '', 'formatter' => ''),
  'content type' => 'ctools_entity_form_field_content_type_content_type',
);

/**
 * Just one subtype.
 *
 * Ordinarily this function is meant to get just one subtype. However, we are
 * using it to deal with the fact that we have changed the subtype names. This
 * lets us translate the name properly.
 */
function ctools_entity_form_field_content_type_content_type($subtype) {
  $types = ctools_entity_form_field_content_type_content_types();
  if (isset($types[$subtype])) {
    return $types[$subtype];
  }
}

/**
 * Return all field content types available.
 */
function ctools_entity_form_field_content_type_content_types() {
  // This will hold all the individual field content types.
  $types = array();
  $content_types = array();
  $entities = entity_get_info();

  foreach ($entities as $entity_type => $entity) {
    foreach ($entity['bundles'] as $type => $bundle) {
      foreach (field_info_instances($entity_type, $type) as $field_name => $field) {
        if (!isset($types[$entity_type . ':' . $field_name])) {
          $types[$entity_type . ':' . $field_name] = array(
            'category' => t('Form'),
            'icon' => 'icon_field.png',
            'title' => t('Field form: @widget_label', array(
              '@widget_label' => t($field['label']),
            )),
            'description' => t('Field on the referenced entity.'),
          );
        }
        $content_types[$entity_type . ':' . $field_name]['types'][$type] = $bundle['label'];
      }
    }
  }

  // Create the required context for each field related to the bundle types.
  foreach ($types as $key => $field_content_type) {
    list($entity_type, $field_name) = explode(':', $key, 2);
    $types[$key]['required context'] = new ctools_context_required(t(ucfirst($entity_type)), $entity_type, array(
      'form' => array('form'),
      'type' => array_keys($content_types[$key]['types']),
    ));
    unset($content_types[$key]['types']);
  }
  return $types;
}

/**
* Render the custom content type.
*/
function ctools_entity_form_field_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return;
  }

  // Get a shortcut to the entity.
  $entity = $context->data;
  list($entity_type, $field_name) = explode(':', $subtype, 2);

  // Load the entity type's information for this field.
  $ids = entity_extract_ids($entity_type, $entity);
  $field = field_info_instance($entity_type, $field_name, $ids[2]);

  // Do not render if the entity type does not have this field.
  if (empty($field)) {
    return;
  }

  if (isset($context->form)) {
    $block->content[$field_name] = $context->form[$field_name];
    unset($context->form[$field_name]);
  }
  else {
    $block->content = t('Entity info.');
  }

  return $block;
}

/**
* Returns the administrative title for a type.
*/
function ctools_entity_form_field_content_type_admin_title($subtype, $conf, $context) {
  list($entity_type, $field_name) = explode(':', $subtype, 2);

  $field = field_info_instance($entity_type, $field_name, $context->restrictions['type'][0]);

  return t('"@s" @field form', array('@s' => $context->identifier, '@field' => $field['label']));
}

function ctools_entity_form_field_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
