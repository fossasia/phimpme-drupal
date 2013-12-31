<?php

/**
 * @file
 * Provides Features integration for entity types using the CRUD API.
 */

/**
 * Returns the configured entity features controller.
 */
function entity_features_get_controller($type) {
  $static = &drupal_static(__FUNCTION__);
  if (!isset($static[$type])) {
    $info = entity_get_info($type);
    $info += array('features controller class' => 'EntityDefaultFeaturesController');
    $static[$type] = $info['features controller class'] ? new $info['features controller class']($type) : FALSE;
  }
  return $static[$type];
}

/**
 * Default controller handling features integration.
 */
class EntityDefaultFeaturesController {

  protected $type, $info;

  public function __construct($type) {
    $this->type = $type;
    $this->info = entity_get_info($type);
    $this->info['entity keys'] += array('module' => 'module', 'status' => 'status');
    $this->statusKey = $this->info['entity keys']['status'];
    $this->moduleKey = $this->info['entity keys']['module'];
    if (!empty($this->info['bundle of'])) {
      $entity_info = entity_get_info($this->info['bundle of']);
      $this->bundleKey = $entity_info['bundle keys']['bundle'];
    }
  }

  /**
   * Defines the result for hook_features_api().
   */
  public function api() {
    return array(
      // The entity type has to be the features component name.
      $this->type => array(
        'name' => $this->info['label'],
        'feature_source' => TRUE,
        'default_hook' => isset($this->info['export']['default hook']) ? $this->info['export']['default hook'] : 'default_' . $this->type,
        // Use the provided component callbacks making use of the controller.
        'base' => 'entity',
      ),
    );
  }

  /**
   * Generates the result for hook_features_export_options().
   */
  public function export_options() {
    $options = array();
    foreach (entity_load_multiple_by_name($this->type, FALSE) as $name => $entity) {
      $options[$name] = entity_label($this->type, $entity);
    }
    return $options;
  }

  /**
   * Generates the result for hook_features_export().
   */
  public function export($data, &$export, $module_name = '') {
    $pipe = array();
    foreach (entity_load_multiple_by_name($this->type, $data) as $name => $entity) {
      // If this entity is provided by a different module, add it as dependency.
      if (($entity->{$this->statusKey} & ENTITY_IN_CODE) && $entity->{$this->moduleKey} != $module_name) {
        $module = $entity->{$this->moduleKey};
        $export['dependencies'][$module] = $module;
      }
      // Otherwise export the entity.
      else {
        $export['features'][$this->type][$name] = $name;

        // If this is a bundle of a fieldable entity, add its fields to the pipe.
        if (!empty($this->info['bundle of'])) {
          $fields = field_info_instances($this->info['bundle of'], $entity->{$this->bundleKey});
          foreach ($fields as $name => $field) {
            $pipe['field'][] = "{$field['entity_type']}-{$field['bundle']}-{$field['field_name']}";
          }
        }
      }
    }
    // Add the module providing the entity type as dependency.
    if ($data && !empty($this->info['module'])) {
      $export['dependencies'][$this->info['module']] = $this->info['module'];
      // In case entity is not already an indirect dependency, add it.
      // We can do so without causing redundant dependencies because,
      // if entity is an indirect dependency, Features will filter it out.
      $export['dependencies']['entity'] = 'entity';
    }
    return $pipe;
  }

  /**
   * Generates the result for hook_features_export_render().
   */
  function export_render($module, $data, $export = NULL) {
    $output = array();
    $output[] = '  $items = array();';
    foreach (entity_load_multiple_by_name($this->type, $data) as $name => $entity) {
      $export  = "  \$items['$name'] = entity_import('{$this->type}', '";
      // Make sure to escape the characters \ and '.
      $export .= addcslashes(entity_export($this->type, $entity, '  '), '\\\'');
      $export .= "');";
      $output[] = $export;
    }
    $output[] = '  return $items;';
    $output = implode("\n", $output);

    $hook = isset($this->info['export']['default hook']) ? $this->info['export']['default hook'] : 'default_' . $this->type;
    return array($hook => $output);
  }

  /**
   * Generates the result for hook_features_revert().
   */
  function revert($module = NULL) {
    if ($defaults = features_get_default($this->type, $module)) {
      foreach ($defaults as $name => $entity) {
        entity_delete($this->type, $name);
      }
    }
  }
}

/**
 * Implements of hook_features_api().
 */
function entity_features_api() {
  $items = array();
  foreach (entity_crud_get_info() as $type => $info) {
    if (!empty($info['exportable']) && $controller = entity_features_get_controller($type)) {
      $items += $controller->api();
    }
  }
  return $items;
}

/**
 * Features component callback.
 */
function entity_features_export_options($entity_type) {
  return entity_features_get_controller($entity_type)->export_options();
}

/**
 * Features component callback.
 */
function entity_features_export($data, &$export, $module_name = '', $entity_type) {
  return entity_features_get_controller($entity_type)->export($data, $export, $module_name);
}

/**
 * Features component callback.
 */
function entity_features_export_render($module, $data, $export = NULL, $entity_type) {
  return entity_features_get_controller($entity_type)->export_render($module, $data, $export);
}

/**
 * Features component callback.
 */
function entity_features_revert($module = NULL, $entity_type) {
  return entity_features_get_controller($entity_type)->revert($module);
}
