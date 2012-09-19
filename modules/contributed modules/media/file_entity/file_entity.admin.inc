<?php

/**
 * @file
 * Administrative interface for file type configuration.
 */

/**
 * Displays the file type admin overview page.
 */
function file_entity_list_types_page() {
  $types = file_info_file_types();
  $entity_info = entity_get_info('file');
  $field_ui = module_exists('field_ui');
  $header = array(
    array('data' => t('Name')),
    array('data' => t('Operations'), 'colspan' => $field_ui ? '3' : '1'),
  );
  $rows = array();

  foreach ($types as $type => $info) {
    $row = array(array('data' => theme('file_entity_file_type_overview', $info)));
    $path = isset($entity_info['bundles'][$type]['admin']['real path']) ? $entity_info['bundles'][$type]['admin']['real path'] : NULL;
    if ($field_ui) {
      $row[] = array('data' => isset($path) ? l(t('manage fields'), $path . '/fields') : '');
      $row[] = array('data' => isset($path) ? l(t('manage display'), $path . '/display') : '');
    }
    $row[] = array('data' => isset($path) ? l(t('manage file display'), $path . '/file-display') : '');
    $rows[] = $row;
  }

  $build['file_type_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No file types available.'),
  );

  return $build;
}

/**
 * Form callback; presents file display settings for a given view mode.
 */
function file_entity_file_display_form($form, &$form_state, $bundle, $view_mode) {
  $file_type = field_extract_bundle('file', $bundle);
  $form['#file_type'] = $file_type;
  $form['#view_mode'] = $view_mode;
  $form['#tree'] = TRUE;
  $form['#attached']['js'][] = drupal_get_path('module', 'file_entity') . '/file_entity.admin.js';

  // Retrieve available formatters for this file type and load all configured
  // filters for existing text formats.
  $formatters = file_info_formatter_types();
  foreach ($formatters as $name => $formatter) {
    if (isset($formatter['file types']) && !in_array($file_type, $formatter['file types'])) {
      unset ($formatters[$name]);
    }
  }
  $current_displays = file_displays_load($file_type, $view_mode, TRUE);
  foreach ($current_displays as $name => $display) {
    $current_displays[$name] = (array) $display;
  }

  // Formatter status.
  $form['displays']['status'] = array(
    '#type' => 'item',
    '#title' => t('Enabled displays'),
    '#prefix' => '<div id="file-displays-status-wrapper">',
    '#suffix' => '</div>',
  );
  $i=0;
  foreach ($formatters as $name => $formatter) {
    $form['displays']['status'][$name] = array(
      '#type' => 'checkbox',
      '#title' => $formatter['label'],
      '#default_value' => !empty($current_displays[$name]['status']),
      '#description' => isset($formatter['description']) ? $formatter['description'] : NULL,
      '#parents' => array('displays', $name, 'status'),
      '#weight' => $formatter['weight'] + $i/1000,
    );
    $i++;
  }

  // Formatter order (tabledrag).
  $form['displays']['order'] = array(
    '#type' => 'item',
    '#title' => t('Display precedence order'),
    '#theme' => 'file_entity_file_display_order',
  );
  foreach ($formatters as $name => $formatter) {
    $form['displays']['order'][$name]['label'] = array(
      '#markup' => check_plain($formatter['label']),
    );
    $form['displays']['order'][$name]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for @title', array('@title' => $formatter['label'])),
      '#title_display' => 'invisible',
      '#delta' => 50,
      '#default_value' => isset($current_displays[$name]['weight']) ? $current_displays[$name]['weight'] : 0,
      '#parents' => array('displays', $name, 'weight'),
    );
    $form['displays']['order'][$name]['#weight'] = $form['displays']['order'][$name]['weight']['#default_value'];
  }

  // Formatter settings.
  $form['display_settings_title'] = array(
    '#type' => 'item',
    '#title' => t('Display settings'),
  );
  $form['display_settings'] = array(
    '#type' => 'vertical_tabs',
  );
  $i=0;
  foreach ($formatters as $name => $formatter) {
    if (isset($formatter['settings callback']) && ($function = $formatter['settings callback']) && function_exists($function)) {
      $defaults = !empty($formatter['default settings']) ? $formatter['default settings'] : array();
      $settings = !empty($current_displays[$name]['settings']) ? $current_displays[$name]['settings'] : array();
      $settings += $defaults;
      $settings_form = $function($form, $form_state, $settings, $name, $file_type, $view_mode);
      if (!empty($settings_form)) {
        $form['displays']['settings'][$name] = array(
          '#type' => 'fieldset',
          '#title' => $formatter['label'],
          '#parents' => array('displays', $name, 'settings'),
          '#group' => 'display_settings',
          '#weight' => $formatter['weight'] + $i/1000,
        ) + $settings_form;
      }
    }
    $i++;
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
}

/**
 * Process file display settings form submissions.
 */
function file_entity_file_display_form_submit($form, &$form_state) {
  $file_type = $form['#file_type'];
  $view_mode = $form['#view_mode'];
  $displays = isset($form_state['values']['displays']) ? $form_state['values']['displays'] : array();
  $displays_original = file_displays_load($file_type, $view_mode, TRUE);
  foreach ($displays as $formatter_name => $display) {
    $display_original = isset($displays_original[$formatter_name]) ? $displays_original[$formatter_name] : file_display_new($file_type, $view_mode, $formatter_name);
    $display += (array) $display_original;
    file_display_save((object) $display);
  }
  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Returns HTML for a file type label and description for the file type admin overview page.
 */
function theme_file_entity_file_type_overview($variables) {
  return check_plain($variables['label']) . '<div class="description">' . $variables['description'] . '</div>';
}

/**
 * Returns HTML for a file display's display order table.
 */
function theme_file_entity_file_display_order($variables) {
  $element = $variables['element'];

  $rows = array();
  foreach (element_children($element, TRUE) as $name) {
    $element[$name]['weight']['#attributes']['class'][] = 'file-display-order-weight';
    $rows[] = array(
      'data' => array(
        drupal_render($element[$name]['label']),
        drupal_render($element[$name]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $output = drupal_render_children($element);
  $output .= theme('table', array('rows' => $rows, 'attributes' => array('id' => 'file-displays-order')));
  drupal_add_tabledrag('file-displays-order', 'order', 'sibling', 'file-display-order-weight', NULL, NULL, TRUE);

  return $output;
}
