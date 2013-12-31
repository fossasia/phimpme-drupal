<?php

if (module_exists('taxonomy')) {
  /**
   * Plugins are described by creating a $plugin array which will be used
   * by the system that includes this file.
   */
  $plugin = array(
    'single' => TRUE,
    'title' => t('Vocabulary terms'),
    'icon' => 'icon_vocabulary.png',
    'description' => t('All the terms in a vocabulary.'),
    'required context' => new ctools_context_required(t('Vocabulary'), 'entity:taxonomy_vocabulary'),
    'category' => t('Vocabulary'),
    'defaults' => array('max_depth' => 0, 'tree' => 1),
  );
}

/**
 * Output function for the 'vocabulary terms' content type. Outputs a
 * list of terms for the input vocabulary.
 */
function ctools_vocabulary_terms_content_type_render($subtype, $conf, $panel_args, $context) {
  $vocab = isset($context->data) ? clone($context->data) : NULL;
  $max_depth = (!empty($conf['max_depth']) ? (int)$conf['max_depth'] : NULL);
  if ($conf['tree'] == FALSE) {
    $terms = taxonomy_get_tree($vocab->vid, 0, $max_depth);
    $items = array();
    foreach ($terms as $term) {
      $items[] = l($term->name, 'taxonomy/term/' . $term->tid);
    }
    $output = theme('item_list', array('items' => $items));
  }
  else {
    $output = theme('item_list', array('items' => _ctools_content_vocabulary_terms($vocab->vid, $max_depth)));
  }

  $block = new stdClass();
  $block->module  = 'node_type';
  $block->title = check_plain($vocab->name);
  $block->content = $output;
  $block->delta   = $vocab->vid;

  return $block;
}

function _ctools_content_vocabulary_terms($vid, $max_depth, $depth = -1, $tid = 0) {
  $depth++;
  if ($max_depth != NULL && $depth == $max_depth) {
    return array();
  }
  $return = array();
  $query = db_select('taxonomy_term_data', 't')->fields('t', array('tid'));
  $query->join('taxonomy_term_hierarchy', 'h', ' t.tid = h.tid');
  $query->condition('t.vid', $vid)
    ->condition('h.parent', $tid)
    ->orderBy('t.weight')
    ->orderBy('t.name');
  $tids = $query->execute()->fetchCol();
  $terms = taxonomy_term_load_multiple($tids);
  foreach ($terms as $term) {
    $return[] = array(
      'data' => l($term->name, 'taxonomy/term/' . $term->tid),
      'children' => _ctools_content_vocabulary_terms($vid, $max_depth, $depth, $term->tid),
    );
  }
  return $return;
}

function ctools_vocabulary_terms_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" terms', array('@s' => $context->identifier));
}

function ctools_vocabulary_terms_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['max_depth'] = array(
    '#type' => 'select',
    '#title' => t('Maximum depth'),
    '#options' => array_merge(array(t('unlimited')), range(1, 9)),
    '#default_value' => $conf['max_depth'],
    '#description' => t('Define the maximum depth of terms being displayed.'),
  );

  $form['tree'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display as tree'),
    '#default_value' => $conf['tree'],
    '#description' => t('If checked, the terms are displayed in a tree, otherwise in a flat list.'),
  );

  return $form;
}

function ctools_vocabulary_terms_content_type_edit_form_submit($form, &$form_state) {
  // Copy everything from our defaults.
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

