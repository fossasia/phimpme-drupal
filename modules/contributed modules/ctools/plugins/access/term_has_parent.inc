<?php
/**
 * @file
 * Plugin to provide access control based upon a parent term.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("Taxonomy: term has parent(s)"),
  'description' => t('Control access if a term belongs to a specific parent term.'),
  'callback' => 'ctools_term_has_parent_ctools_access_check',
  'default' => array('vid' => array(), 'negate' => 0),
  'settings form' => 'ctools_term_has_parent_ctools_access_settings',
  'settings form submit' => 'ctools_term_has_parent_ctools_access_settings_submit',
  'summary' => 'ctools_term_has_parent_ctools_access_summary',
  'required context' => new ctools_context_required(t('Term'), array('taxonomy_term', 'terms')),
);

/**
 * Settings form for the 'by parent term' access plugin
 */
function ctools_term_has_parent_ctools_access_settings($form, &$form_state, $conf) {
  // If no configuration was saved before, set some defaults.
  if (empty($conf)) {
    $conf = array(
      'vid' => 0,
    );
  }
  if (!isset($conf['vid'])) {
    $conf['vid'] = 0;
  }

  $form['settings']['vid'] = array(
    '#title' => t('Vocabulary'),
    '#type' => 'select',
    '#options' => array(),
    '#description' => t('Select the vocabulary for this form.'),
    '#id' => 'ctools-select-vid',
    '#default_value' => $conf['vid'],
    '#required' => TRUE,
  );

  ctools_include('dependent');
  $options = array();

  // A note: Dependency works strangely on these forms as they have never been
  // updated to a more modern system so they are not individual forms of their
  // own like the content types.

  $form['settings']['#tree'] = TRUE;

  // Loop over each of the configured vocabularies.
  foreach (taxonomy_get_vocabularies() as $vid => $vocabulary) {
    $options[$vid] = $vocabulary->name;
    $form['settings']['vid_' . $vid] = array(
      '#title' => t('Terms'),
      '#description' => t('Select a term or terms from @vocabulary.', array('@vocabulary' => $vocabulary->name)),
      '#dependency' => array('ctools-select-vid' => array($vocabulary->vid)),
      '#default_value' => !empty($conf['vid_' . $vid]) ? $conf['vid_' . $vid] : '',
      '#size' => 10,
      '#multiple' => TRUE,
      //@todo: Remove the following workaround when the following patch is in core. {@see:http://drupal.org/node/1117526}
      '#name' => sprintf("settings[%u][]", $vid),
      '#attributes' => array('multiple' => 'multiple'),
    );

    $terms = array();
    foreach (taxonomy_get_tree($vocabulary->vid) as $term) {
      $terms[$term->tid] = str_repeat('-', $term->depth) . ($term->depth ? ' ' : '') . $term->name;
    }
    //$form['settings']['vid_' . $vid]['#type'] = 'select';
    $form['settings']['vid_' . $vid]['#type'] = 'checkboxes';
    $form['settings']['vid_' . $vid]['#options'] = $terms;
    unset($terms);
  }
  $form['settings']['vid']['#options'] = $options;
  $form['settings']['include_self'] = array(
    '#title' => t('Include these term(s) as candidates?'),
    '#description' => t('When this rule is evaluated, should the term(s) you select be included as candidates for access?'),
    '#default_value' => !empty($conf['include_self']) ? $conf['include_self'] : FALSE,
    '#type' => 'checkbox',
  );
  return $form;
}

/**
 * Filters values to store less.
 */
function ctools_term_has_parent_ctools_access_settings_submit($form, &$form_state) {
  foreach ($form_state['values']['settings'] as $key => $value) {
    if (strpos($key, 'vid_') === 0) {
      $form_state['values']['settings'][$key] = array_filter($form_state['values']['settings'][$key]);
    }
  }
}

/**
 * Check for access.
 */
function ctools_term_has_parent_ctools_access_check($conf, $context) {
  // As far as I know there should always be a context at this point, but this
  // is safe.
  if (empty($context) || empty($context->data) || empty($context->data->vid) || empty($context->data->tid)) {
    return FALSE;
  }

  // Get the $vid.
  if (!isset($conf['vid'])) {
    return FALSE;
  }
  $vid = $conf['vid'];

  // we'll start looking up the hierarchy from our context term id.
  $current_term = $context->data->tid;

  $term='';

  // scan up the tree.
  while (true) {
    // select parent as term_parent to avoid PHP5 complications with the parent keyword
    //@todo: Find a way to reduce the number of queries required for really deep hierarchies.
    $term = db_query("SELECT parent AS term_parent, tid AS tid FROM {taxonomy_term_hierarchy} th WHERE th.tid = :tid", array(':tid'=>$current_term))->fetchObject();

    // if no term is found, get out of the loop
    if (!$term || empty($term->tid)) {
      break;
    }

    // check the term selected, if the user asked it to.
    if (!empty($conf['include_self']) && isset($conf['vid_' . $vid][$term->tid])) {
      return TRUE;
    }

    // did we find the parent TID we were looking for?
    if (isset($conf['vid_' . $vid][$term->tid])) {
      // YES, we're done!
      return TRUE;
    }
    // Nope, we didn't find it.

    // If this is the top of the hierarchy, stop scanning.
    if ($term->term_parent==0) {
      break;
    }

    // update the parent, and keep scanning.
    $current_term = $term->term_parent;
  }

  return FALSE;
}

/**
 * Provide a summary description based upon the checked terms.
 */
function ctools_term_has_parent_ctools_access_summary($conf, $context) {
  $vid = (int)$conf['vid'];
  $terms = array();
  foreach ($conf['vid_' . $vid] as $tid) {
    $term = taxonomy_term_load($tid);
    $terms[] = $term->name;
  }

  return format_plural(count($terms),
    '@term can have the parent "@terms"',
    '@term can have one of these parents: @terms',
    array('@terms' => implode(', ', $terms),
      '@term' => $context->identifier));
}
