<?php

/**
 * @file Rules theme functions
 */


/**
 * Themes a tree of rule elements in a draggable table.
 *
 * @ingroup themeable
 */
function theme_rules_elements($variables) {
  $form = $variables['element'];
  $form['#theme'] = 'table';
  $form['#header'] = array(t('Elements'), t('Weight'), t('Operations'));
  $form['#attributes']['id'] = 'rules-' . drupal_html_id($form['#caption']) . '-id';

  foreach (element_children($form) as $element_id) {
    $element = &$form[$element_id];

    // Add special classes to be used for tabledrag.js.
    $element['parent_id']['#attributes']['class'] = array('rules-parent-id');
    $element['element_id']['#attributes']['class'] = array('rules-element-id');
    $element['weight']['#attributes']['class'] = array('rules-element-weight');

    $row = array();
    $row[] = theme('indentation', array('size' => $element['#depth'])) . drupal_render($element['label']);

    $row[] = drupal_render($element['weight']) . drupal_render($element['parent_id']) . drupal_render($element['element_id']);
    $row[] = array('class' => 'rules-operations', 'data' => $element['operations']);

    $row = array('data' => $row) + $element['#attributes'];
    $row['class'][] = 'draggable';
    if (!$element['#container']) {
      $row['class'][] = 'tabledrag-leaf';
    }
    $form['#rows'][] = $row;
  }
  if (!empty($form['#rows'])) {
    drupal_add_tabledrag($form['#attributes']['id'], 'match', 'parent', 'rules-parent-id', 'rules-parent-id', 'rules-element-id', TRUE, 10);
    drupal_add_tabledrag($form['#attributes']['id'], 'order', 'sibling', 'rules-element-weight');
  }
  else {
    $form['#rows'][] = array(array('data' => t('None'), 'colspan' => 3));
  }
  if (!empty($form['#add'])) {
    $row = array();
    $row[] = array('data' => $form['#add'], 'colspan' => 3);
    $form['#rows'][] = array('data' => $row, 'class' => array('rules-elements-add'));
  }

  // Add a wrapping div.
  return '<div class="rules-elements-table">' . drupal_render($form) . '</div>';
}

/**
 * Themes the rules form for editing the used variables.
 *
 * @see RulesPluginUI::getVariableForm()
 * @ingroup themeable
 */
function theme_rules_ui_variable_form($variables) {
  $elements = $variables['element'];

  $table['#theme'] = 'table';
  $table['#header'] = array(t('Data type'), t('Label'), t('Machine name'), t('Usage'), array('data' => t('Weight'), 'class' => array('tabledrag-hide')));
  $table['#attributes']['id'] = 'rules-' . drupal_html_id($elements['#title']) . '-id';

  foreach (element_children($elements['items']) as $key) {
    $element = &$elements['items'][$key];
    // Add special classes to be used for tabledrag.js.
    $element['weight']['#attributes']['class'] = array('rules-element-weight');

    $row = array();
    $row[] = array('data' => $element['type']);
    $row[] = array('data' => $element['label']);
    $row[] = array('data' => $element['name']);
    $row[] = array('data' => $element['usage']);
    $row[] = array('data' => $element['weight']);
    $row = array('data' => $row) + $element['#attributes'];
    $row['class'][] = 'draggable';
    $table['#rows'][] = $row;
  }
  $elements['items']['#printed'] = TRUE;
  if (!empty($table['#rows'])) {
    drupal_add_tabledrag($table['#attributes']['id'], 'order', 'sibling', 'rules-element-weight');
  }

  // Theme it like a form item, but with the description above the content.
  $attributes['class'][] = 'form-item';
  $attributes['class'][] = 'rules-variables-form';

  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";
  $output .= theme('form_element_label', $variables);
  if (!empty($elements['#description'])) {
    $output .= ' <div class="description">' . $elements['#description'] . "</div>\n";
  }
  $output .= ' ' . drupal_render($table) . "\n";
  // Add in any further children elements.
  foreach (element_children($elements, TRUE) as $key) {
    $output .= drupal_render($elements[$key]);
  }
  $output .= "</div>\n";
  return $output;
}

/**
 * Themes a view of multiple configuration items.
 * @ingroup themeable
 */
function theme_rules_content_group($variables) {
  $element = $variables['element'];
  $output = array();
  foreach (element_children($element) as $key) {
    $output[] = drupal_render($element[$key]);
  }
  $output = array_filter($output);
  $heading = !empty($element['#caption']) ? "<span class='rules-content-heading'>" . $element['#caption'] . ': </span>' : '';
  if (!empty($output)) {
    $element['#attributes']['class'][] = 'rules-element-content-group';
    return '<div' . drupal_attributes($element['#attributes']) . '>' . $heading . implode(', ', $output) . '</div>';
  }
}

/**
 * Themes the view of a single parameter configuration.
 * @ingroup themeable
 */
function theme_rules_parameter_configuration($variables) {
  $element = $variables['element'];
  $content = drupal_render_children($element);
  // Add the full content to the span's title, but don't use drupal_attributes
  // for that as this would invoke check_plain() again.
  $title = strip_tags($content);
  $element['#attributes']['class'][] = 'rules-parameter-configuration';
  $attributes = drupal_attributes($element['#attributes']) . " title='$title'";

  $label_attributes['class'][] = 'rules-parameter-label';
  if (!empty($element['#info']['description'])) {
    $label_attributes['title'] = $element['#info']['description'];
  }
  $label_attributes = drupal_attributes($label_attributes);

  $output = "<span $label_attributes>" . check_plain($element['#info']['label']) . ': </span>';
  $output .= "<span $attributes>" . truncate_utf8($content, 30, TRUE, TRUE) . "</span>";
  return $output;
}

/**
 * Themes info about variables.
 * @ingroup themeable
 */
function theme_rules_variable_view($variables) {
  $element = $variables['element'];

  $label_attributes['class'][] = 'rules-variable-label';
  $label_attributes['title'] = '';
  if (!empty($element['#info']['description'])) {
    $label_attributes['title'] = $element['#info']['description'] . ' ';
  }
  $label_attributes['title'] .= t('Data type: !type', array('!type' => $element['#info']['type']));
  $label_attributes = drupal_attributes($label_attributes);

  $output = check_plain($element['#info']['label']);
  $output .= ' (' . check_plain($element['#name']) . ')';
  return "<span $label_attributes>" . $output . '</span>';
}

/**
 * Themes help for using the data selector.
 * @ingroup themeable
 */
function theme_rules_data_selector_help($variables) {
  $variables_info = $variables['variables'];
  $param_info = $variables['parameter'];

  $render = array(
    '#type' => 'fieldset',
    '#title' => t('Data selectors'),
    '#pre_render' => array(),
    '#attributes' => array(),
  );
  // Make it manually collapsible as we cannot use #collapsible without the
  // FAPI element processor.
  $render['#attached']['js'][] = 'misc/collapse.js';
  $render['#attributes']['class'][] = 'collapsible';
  $render['#attributes']['class'][] = 'collapsed';

  $render['table'] = array(
    '#theme' => 'table',
    '#header' => array(t('Selector'), t('Label'), t('Description')),
  );
  foreach (RulesData::matchingDataSelector($variables_info, $param_info) as $selector => $info) {
    $info += array('label' => '', 'description' => '');
    $render['table']['#rows'][] = array(check_plain($selector), check_plain(drupal_ucfirst($info['label'])), check_plain($info['description']));
  }
  return drupal_render($render);
}

/**
 * Themes the rules log debug output.
 * @ingroup themeable
 */
function theme_rules_log($variables) {
  $element = $variables['element'];
  drupal_add_css(drupal_get_path('module', 'rules') . '/ui/rules.ui.css');
  // Add jquery ui core css and functions, which are needed for the icons.
  drupal_add_library('system', 'ui');
  drupal_add_js(drupal_get_path('module', 'rules') . '/ui/rules.debug.js');

  $output = '<div class="rules-debug-log">';

  $output .= '<h3 class="rules-debug-log-head">';
  $output .= '<span class="rules-debug-open-main rules-debug-collapsible-link">';
  $output .= '<span unselectable="on" class="ui-icon ui-icon-triangle-1-e rules-debug-icon-open">&nbsp;</span>';
  $output .= t('Rules evaluation log');
  $output .= '</span>';
  $output .= '</span>';
  $output .= '<span class="rules-debug-open-all rules-debug-collapsible-link">-' . t('Open all') . '-<span>';
  $output .= '</h3>';

  $output .= '<div>';
  $output .= $element['#children'];
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Theme rules debug log elements.
 * @ingroup themeable
 */
function theme_rules_debug_element($variables) {
  $output = '<div class="rules-debug-block">';
  $output .= '<div class="rules-debug-log-head rules-debug-open rules-debug-collapsible-link">"';
  $output .= '<span unselectable="on" class="ui-icon ui-icon-triangle-1-e rules-debug-icon-open">&nbsp;</span>';
  $output .= $variables['head'];
  if (isset($variables['link'])) {
    $output .= ' ' . $variables['link'];
  }
  $output .= '</div>';
  $output .= $variables['log'];
  $output .= '</div>';
  return $output;
}

/**
 * Themes rules autocomplete forms.
 * @ingroup themeable
 */
function theme_rules_autocomplete($variables) {
  $element = $variables['element'];
  drupal_add_js(drupal_get_path('module', 'rules') . '/ui/rules.autocomplete.js');
  drupal_add_library('system', 'ui.autocomplete');
  drupal_add_library('system', 'ui.button');

  $element['#attributes']['type'] = 'text';
  element_set_attributes($element, array('id', 'name', 'value', 'size', 'maxlength'));
  _form_set_class($element, array('form-text', 'rules-autocomplete'));

  $id = $element['#attributes']['id'];
  $id_button = drupal_html_id($id . '-button');

  $js_vars['rules_autocomplete'][$id] = array(
    'inputId' => $id,
    'source' => url($element['#autocomplete_path'], array('absolute' => TRUE)),
  );
  drupal_add_js($js_vars, 'setting');

  $output = '<div class="rules-autocomplete">';
  $output .= '<input' . drupal_attributes($element['#attributes']) . ' />';
  $output .= '</div>';

  return $output;
}
