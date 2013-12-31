<?php

/**
 * @file
 * Plugin automatically declare 'tokens' as plugins.
 */

/**
 * Plugin decleration.
 */
$plugin = array(
  'title' => t('Tokens'),
  'content type' => 'ctools_token_content_type_content_type',
  'defaults' => array('sanitize' => TRUE),
);

/**
 * Just one subtype.
 *
 * Ordinarily this function is meant to get just one subtype. However, we are
 * using it to deal with the fact that we have changed the subtype names. This
 * lets us translate the name properly.
 */
function ctools_token_content_type_content_type($subtype) {
  $types = ctools_token_content_type_content_types();
  if (isset($types[$subtype])) {
    return $types[$subtype];
  }
}

/**
 * Return all field content types available.
 */
function ctools_token_content_type_content_types() {
  // This will hold all the properties.
  $types = array();
  $info = token_info();

  foreach ($info['tokens'] as $entity_type => $tokens) {
    foreach ($tokens as $name => $token) {
      if (!empty($token['name'])) {
        $token += array('description' => '');
        $types[$entity_type . ':' . $name] = array(
          'category' => t('@entity (tokens)', array('@entity' => ucfirst($entity_type))),
          'icon' => 'icon_token.png',
          'title' => $token['name'],
          'description' => $token['description'],
          'required context' => new ctools_context_required(t(ucfirst($entity_type)), $entity_type),
        );
      }
    }
  }

  return $types;
}

/**
* Render the custom content type.
*/
function ctools_token_content_type_render($subtype, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return FALSE;
  }

  $sanitize = $conf['sanitize'];

  $entity = $context->data;
  list($entity_type, $name) = explode(':', $subtype, 2);

  $info = token_info();
  $values = token_generate($entity_type, array($name => $name), array($entity_type => $entity), array('sanitize' => $sanitize));
  if (!isset($values[$name])) {
    return;
  }

  // Build the content type block.
  $block = new stdClass();
  $block->module  = 'ctools';
  $block->title   = $info['tokens'][$entity_type][$name]['name'];
  $block->content = $values[$name];
  $block->delta   = str_replace('_', '-', $entity_type . '-' . $name);

  return $block;
}

/**
* Returns an edit form for custom type settings.
*/
function ctools_token_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['sanitize'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($conf['sanitize']),
    '#title' => t('Sanitize'),
    '#description' => t('When enabled that output of the token will be stripped from dangerous HTML.'),
  );

  return $form;
}

/**
 * Validate the node selection.
 */
function ctools_token_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['sanitize'] = $form_state['values']['sanitize'];
}


/**
* Returns the administrative title for a type.
*/
function ctools_token_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" @name', array('@s' => $context->identifier, '@name' => $subtype));
}
