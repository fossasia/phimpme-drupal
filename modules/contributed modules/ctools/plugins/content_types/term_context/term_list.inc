<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('List of related terms'),
  'icon' => 'icon_term.png',
  'description' => t('Terms related to an existing term; may be child, siblings or top level.'),
  'required context' => new ctools_context_required(t('Term'), array('term', 'taxonomy_term')),
  'category' => t('Taxonomy term'),
  'defaults' => array('title' => '', 'type' => 'child', 'list_type' => 'ul', 'path' => 'taxonomy/term'),
);

function ctools_term_list_content_type_render($subtype, $conf, $panel_args, $context) {
  $term = isset($context->data) ? clone($context->data) : NULL;
  $block = new stdClass();
  $block->module = 'term-list';
  $path = empty($conf['path']) ? 'taxonomy/term/%tid' : $conf['path'];
  if (strpos($path, '%tid') === FALSE) {
    if (substr($path, -1) != '/') {
      $path .= '/';
    }
    $path .= '%tid';
  }

  $options = ctools_admin_term_list_options();
  $skip = array();

  if ($term) {
    $block->title = $options[$conf['type']];
    $block->delta = $conf['type'];
    switch ($conf['type']) {
      case 'related':
        // FIXME this no longer exists, must be done with Field API
        // $terms = taxonomy_get_related($term->tid);
        break;

      case 'child':
      default:
        $terms = taxonomy_get_children($term->tid);
        break;

      case 'top':
        $terms = taxonomy_get_tree($term->vid, 0, 1);
        break;

      case 'parent':
        $terms = taxonomy_get_parents($term->tid);
        $block->title = format_plural(count($terms), 'Parent term', 'Parent terms');
        break;

      case 'sibling':
        $parent = db_query('SELECT parent FROM {taxonomy_term_hierarchy} WHERE tid = :tid', array(':tid' => $term->tid))->fetchField();
        if ($parent) {
          $terms = taxonomy_get_children($parent, $term->vid);
        }
        else {
          $terms = taxonomy_get_tree($term->vid, 0, 1);
        }

        $skip[$term->tid] = $term->tid;
        break;

      case 'synonyms':
        // FIXME this no longer exists, must be done with Field API
//        $terms = taxonomy_get_synonyms($term->tid);
        break;
    }

    if (!empty($terms)) {
      foreach ($terms as $related) {
        if (empty($skip[$related->tid])) {
          $items[] = l($related->name, str_replace('%tid', $related->tid, $path), array('rel' => 'tag', 'title' => strip_tags($related->description)));
        }
      }

      $block->content = theme('item_list', array('items' => $items, 'type' => $conf['list_type']));
    }
  }
  else {
    $block->content = t('Term description goes here.');
    $block->delta = 'unknown';
  }

  return $block;
}

function ctools_admin_term_list_options() {
  return array(
    'child' => t('Child terms'),
    'related' => t('Related terms (does not work in D7)'),
    'sibling' => t('Sibling terms'),
    'top' => t('Top level terms'),
    'synonyms' => t('Term synonyms (does not work in D7)'),
    'parent' => t('Parent term(s)'),
  );
}

/**
 * Returns an edit form for the custom type.
 */
function ctools_term_list_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('Which terms'),
    '#options' => ctools_admin_term_list_options(),
    '#default_value' => $conf['type'],
    '#prefix' => '<div class="clearfix no-float">',
    '#suffix' => '</div>',
  );

  $form['list_type'] = array(
    '#type' => 'select',
    '#title' => t('List type'),
    '#options' => array('ul' => t('Unordered'), 'ol' => t('Ordered')),
    '#default_value' => $conf['list_type'],
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => empty($conf['path']) ? 'taxonomy/term/%tid' : $conf['path'],
    '#description' => t('The path to use for the terms. You may use %tid to place the term id as part of the path; if let off, it will be appended to the end.'),
  );
  return $form;
}

function ctools_term_list_content_type_admin_title($subtype, $conf, $context) {
  $options = ctools_admin_term_list_options();
  return t('"@s" @type', array('@s' => $context->identifier, '@type' => drupal_strtolower($options[$conf['type']])));
}

function ctools_term_list_content_type_edit_form_submit($form, &$form_state) {
  // Copy everything from our defaults.
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    $form_state['conf'][$key] = $form_state['values'][$key];
  }
}

