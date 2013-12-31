<?php

/**
 * @file
 * Plugin to provide access control based upon entity bundle.
 */

$plugin = array(
  'title' => t("(Custom) Entity: Field Value"),
  'description' => t('Control access by entity field value.'),
  'callback' => 'ctools_entity_field_value_ctools_access_check',
  'default' => array('type' => array()),
  'settings form' => 'ctools_entity_field_value_ctools_access_settings',
  'settings form submit' => 'ctools_entity_field_value_ctools_access_settings_submit',
  'summary' => 'ctools_entity_field_value_ctools_access_summary',
  'get child' => 'ctools_entity_field_value_ctools_access_get_child',
  'get children' => 'ctools_entity_field_value_ctools_access_get_children',
);
function ctools_entity_field_value_ctools_access_get_child($plugin, $parent, $child) {

  $plugins = &drupal_static(__FUNCTION__, array());
  if (empty($plugins[$parent . ':' . $child])) {
    list($entity_type, $bundle_type, $field_name) = explode(':', $child);
    $plugins[$parent . ':' . $child] = _ctools_entity_field_value_ctools_access_get_child($plugin, $parent, $entity_type, $bundle_type, $field_name);
  }

  return $plugins[$parent . ':' . $child];
}

function ctools_entity_field_value_ctools_access_get_children($plugin, $parent) {
  $plugins = &drupal_static(__FUNCTION__, array());
  if (!empty($plugins)) {
    return $plugins;
  }
  $entities = entity_get_info();
  foreach ($entities as $entity_type => $entity) {
    foreach ($entity['bundles'] as $bundle_type => $bundle) {
      foreach (field_info_instances($entity_type, $bundle_type) as $field_name => $field) {
        if (!isset($plugins[$parent . ':' . $entity_type . ':' . $bundle_type . ':' . $field_name])) {
          $plugin = _ctools_entity_field_value_ctools_access_get_child($plugin, $parent, $entity_type, $bundle_type, $field_name, $entity, $bundle, $field);
          $plugins[$parent . ':' . $entity_type . ':' . $bundle_type . ':' . $field_name] = $plugin;
        }
      }
    }
  }

  return $plugins;
}

function _ctools_entity_field_value_ctools_access_get_child($plugin, $parent, $entity_type, $bundle_type, $field_name, $entity = NULL, $bundle = NULL, $field = NULL) {

  // check that the entity, bundle and field arrays have a value.
  // If not, load theme using machine names.
  if (empty($entity)) {
    $entity = entity_get_info($entity_type);
  }

  if (empty($bundle)) {
    $bundle = $entity['bundles'][$bundle_type];
  }

  if (empty($field)) {
    $field_instances = field_info_instances($entity_type, $bundle_type);
    $field = $field_instances[$field_name];
  }

  $plugin['title'] = t('@entity @type: @field Field', array('@entity' => $entity['label'], '@type' => $bundle_type, '@field' => $field['label']));
  $plugin['keyword'] = $entity_type;
  $plugin['description'] = t('Control access by @entity entity bundle.', array('@entity' => $entity_type));
  $plugin['name'] = $parent . ':' . $entity_type . ':' . $bundle_type . ':' . $field_name;
  $plugin['required context'] = new ctools_context_required(t(ucfirst($entity_type)), $entity_type, array(
      'type' => $bundle_type,
    ));

  return $plugin;
}

/**
 * Settings form for the 'by entity_bundle' access plugin
 */
function ctools_entity_field_value_ctools_access_settings($form, &$form_state, $conf) {
  $plugin = $form_state['plugin'];
  list($parent, $entity_type, $bundle_type, $field_name) = explode(':', $plugin['name']);
  $entity_info = entity_get_info($entity_type);
  $instances   = field_info_instances($entity_type, $bundle_type);
  $instance    = $instances[$field_name];
  $field       = field_info_field_by_id($instance['field_id']);
  foreach ($field['columns'] as $column => $attributes) {
    $columns[] = _field_sql_storage_columnname($field_name, $column);
  }
  ctools_include('fields');
  $entity = (object)array(
    $entity_info['entity keys']['bundle'] => $bundle_type,
  );
  $langcode = field_valid_language(NULL);
  $form['settings'] += (array) ctools_field_invoke_field($instance, 'form', $entity_type, $entity, $form, $form_state, array('default' => TRUE, 'language' => $langcode));
  // weight is really not important once this is populated and will only interfere with the form layout.
  foreach (element_children($form['settings']) as $element) {
    unset($form['settings'][$element]['#weight']);
  }
  // Need more logic here to handle compound fields.
  foreach ($columns as $column) {
    if (isset($conf[$column]) && is_array($conf[$column])) {
      foreach ($conf[$column] as $delta => $conf_value) {
        if (is_numeric($delta) && is_array($conf_value)) {
          $form['settings'][$field_name][LANGUAGE_NONE][$delta]['value']['#default_value'] = $conf_value['value'];
        }
        else {
          $form['settings'][$field_name][LANGUAGE_NONE]['#default_value'] = $conf[$column];
        }
      }
    }
  }
  return $form;
}

/**
 * Compress the entity bundles allowed to the minimum.
 */
function ctools_entity_field_value_ctools_access_settings_submit($form, &$form_state) {
  $plugin = $form_state['plugin'];
  list($parent, $entity_type, $bundle_type, $field_name) = explode(':', $plugin['name']);
  $langcode  = field_valid_language(NULL);
  $langcode  = isset($form_state['input']['settings'][$field_name][$langcode]) ? $langcode : LANGUAGE_NONE;
  $instances = field_info_instances($entity_type, $bundle_type);
  $instance  = $instances[$field_name];
  $field     = field_info_field_by_id($instance['field_id']);
  foreach ($field['columns'] as $column => $attributes) {
    $columns[] = _field_sql_storage_columnname($field_name, $column);
  }
  foreach ($columns as $column) {
    $form_state['values']['settings'][$column] = $form_state['input']['settings'][$field_name][$langcode];
  }
}

/**
 * Check for access.
 */
function ctools_entity_field_value_ctools_access_check($conf, $context, $plugin) {
  list($parent, $entity_type, $bundle_type, $field_name) = explode(':', $plugin['name']);

  if ($field_items = field_get_items($entity_type, $context->data, $field_name)) {
    $langcode = field_language($entity_type, $context->data, $field_name);

    foreach ($conf as $potential_field => $values) {
      if ($field_name === $potential_field) {

        $conf_value_array = _ctools_entity_field_value_ctools_access_get_conf_field_values($values, $langcode);
        if (empty($conf_value_array)) {
          return FALSE;
        }

        // Check field value.
        foreach ($field_items as $field_value) {
          if (in_array($field_value['value'], $conf_value_array)) {
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}

function _ctools_entity_field_value_ctools_access_get_conf_field_values($values, $langcode = LANGUAGE_NONE) {
  if (!is_array($values) || !isset($values[$langcode])) {
    return;
  }
  $conf_values = array();

  foreach ($values[$langcode] as $value) {
    $conf_values[] = $value['value'];
  }

  return $conf_values;
}

/**
 * Provide a summary description based upon the checked entity_bundle.
 */
function ctools_entity_field_value_ctools_access_summary($conf, $context, $plugin) {
  list($parent, $entity_type, $bundle_type, $field_name) = explode(':', $plugin['name']);
  $instances   = field_info_instances($entity_type, $bundle_type);
  $instance    = $instances[$field_name];
  $field       = field_info_field_by_id($instance['field_id']);
  $entity_info = entity_get_info($entity_type);
  $entity      = (object)array(
    $entity_info['entity keys']['bundle'] => $bundle_type,
  );
  $string = '';
  $keys   = array();
  $values = array();
  foreach ($field['columns'] as $column => $attributes) {
    $conf_key = _field_sql_storage_columnname($field_name, $column);
    if (count($field['columns']) > 1) {
      // Add some sort of handling for compound fields
    }
    else {
      if (isset($conf[$conf_key])) {
        $entity->{$field_name}[LANGUAGE_NONE][] = array($column => $conf[$conf_key]);
      }
    }
    $string .= " @{$column} equals @{$column}_value";
    $keys['@' . $column] = $column;
    $values["@{$column}_value"] = $conf[$conf_key];
  }
  $view_mode = 'full';
  $null      = NULL;
  $options   = array('language' => LANGUAGE_NONE);
  ctools_include('fields');
  $display         = field_get_display($instance, $view_mode, $entity);
  $display['type'] = 'list_default';
  $function        = $display['module'] . '_field_formatter_view';
  $items           = isset($entity->{$field_name}[LANGUAGE_NONE]) ? $entity->{$field_name}[LANGUAGE_NONE] : array();
  if (function_exists($function)) {
    $elements = $function($entity_type, $entity, $field, $instance, LANGUAGE_NONE, $items, $display);
  }
  $value_keys = array_keys($values);
  foreach ($value_keys as $key => $value) {
    $values[$value] = $elements[$key]['#markup'];
  }
  $values = array_merge($keys, $values);
  return t($string, $values);
}

