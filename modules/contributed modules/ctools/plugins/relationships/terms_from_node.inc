<?php

/**
 * @file
 * Plugin to provide an relationship handler for all terms from node.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Multiple terms from node'),
  'keyword' => 'terms',
  'description' => t('Adds a taxonomy terms from a node context; if multiple terms are selected, they wil be concatenated.'),
  'required context' => new ctools_context_required(t('Node'), 'node'),
  'context' => 'ctools_terms_from_node_context',
  'edit form' => 'ctools_terms_from_node_settings_form',
  'defaults' => array('vocabulary' => array(), 'concatenator' => ','),
);

/**
 * Return a new context based on an existing context.
 */
function ctools_terms_from_node_context($context, $conf) {
  // If unset it wants a generic, unfilled context, which is just NULL.
  if (empty($context->data)) {
    return ctools_context_create_empty('terms', NULL);
  }

  // Collect all terms for the chosen vocabulary and concatenate them.
  $node = $context->data;
  $terms = array();
  
  $fields = field_info_instances('node', $node->type);
  foreach ($fields as $name => $info) {
    $field_info = field_info_field($name);
    if ($field_info['type'] == 'taxonomy_term_reference' && (empty($conf['vocabulary']) || $conf['vocabulary'][$field_info['settings']['allowed_values'][0]['vocabulary']])) {
      $items = field_get_items('node', $node, $name);
      if (is_array($items)) {
        foreach ($items as $item) {
          $terms[] = $item['tid'];
        }
      }
    }
  }
  
  if (!empty($terms)) {
    $all_terms = ctools_break_phrase(implode($conf['concatenator'], $terms));
    return ctools_context_create('terms', $all_terms);
  }
}

/**
 * Settings form for the relationship.
 */
function ctools_terms_from_node_settings_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $options = array();
  foreach (taxonomy_vocabulary_get_names() as $name => $vocabulary) {
    $options[$name] = $vocabulary->name;
  }
  $form['vocabulary'] = array(
    '#title' => t('Vocabulary'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $conf['vocabulary'],
    '#prefix' => '<div class="clearfix">',
    '#suffix' => '</div>',
  );
  $form['concatenator'] = array(
    '#title' => t('Concatenator'),
    '#type' => 'select',
    '#options' => array(',' => ', (AND)', '+' => '+ (OR)'),
    '#default_value' => $conf['concatenator'],
    '#prefix' => '<div class="clearfix">',
    '#suffix' => '</div>',
    '#description' => t("When the value from this context is passed on to a view as argument, the terms can be concatenated in the form of 1+2+3 (for OR) or 1,2,3 (for AND)."),
  );

  return $form;
}
