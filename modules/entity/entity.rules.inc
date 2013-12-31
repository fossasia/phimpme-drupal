<?php

/**
 * @file
 * Provides Rules integration for entities provided via the CRUD API.
 *
 * Rules automatically provides us with actions for CRUD and a suiting entity
 * data type. For events the controller automatically invokes Rules events once
 * Rules is active, so we just have to provide the appropriate info.
 */

/**
 * Default controller for generating Rules integration.
 */
class EntityDefaultRulesController {

  protected $type, $info;

  public function __construct($type) {
    $this->type = $type;
    $this->info = entity_get_info($type);
  }

  public function eventInfo() {
    $info = $this->info;
    $type = $this->type;

    $label = $info['label'];
    $defaults = array(
      'module' => isset($info['module']) ? $info['module'] : 'entity',
      'group' => $label,
      'access callback' => 'entity_rules_integration_event_access',
    );

    $items[$type . '_insert'] = $defaults + array(
      'label' => t('After saving a new @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => entity_rules_events_variables($type, t('created @entity', array('@entity' => drupal_strtolower($label)))),
    );
    $items[$type . '_update'] = $defaults + array(
      'label' => t('After updating an existing @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => entity_rules_events_variables($type, t('updated @entity', array('@entity' => drupal_strtolower($label))), TRUE),
    );
    $items[$type . '_presave'] = $defaults + array(
      'label' => t('Before saving a @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => entity_rules_events_variables($type, t('saved @entity', array('@entity' => drupal_strtolower($label))), TRUE),
    );
    $items[$type . '_delete'] = $defaults + array(
      'label' => t('After deleting a @entity', array('@entity' => drupal_strtolower($label))),
      'variables' => entity_rules_events_variables($type, t('deleted @entity', array('@entity' => drupal_strtolower($label)))),
    );
    if (count($info['view modes'])) {
      $items[$type . '_view'] = $defaults + array(
        'label' => t('@entity is viewed', array('@entity' => $label)),
        'variables' => entity_rules_events_variables($type, t('viewed @entity', array('@entity' => drupal_strtolower($label)))) + array(
          'view_mode' => array(
            'type' => 'text',
            'label' => t('view mode'),
            'options list' => 'rules_get_entity_view_modes',
             // Add the entity-type for the options list callback.
            'options list entity type' => $type,
          ),
        ),
      );
    }
    // Specify that on presave the entity is saved anyway.
    $items[$type . '_presave']['variables'][$type]['skip save'] = TRUE;
    return $items;
  }

}

/**
 * Returns some parameter info suiting for the specified entity type.
 */
function entity_rules_events_variables($type, $label, $update = FALSE) {
  $args = array(
    $type => array('type' => $type, 'label' => $label),
  );
  if ($update) {
    $args += array(
      $type . '_unchanged' => array(
        'type' => $type,
        'label' => t('unchanged entity'),
        'handler' => 'rules_events_entity_unchanged',
      ),
    );
  }
  return $args;
}

/**
 * Implements hook_rules_event_info().
 */
function entity_rules_event_info() {
  $items = array();
  foreach (entity_crud_get_info() as $type => $info) {
    // By default we enable the controller only for non-configuration.
    $configuration = !empty($info['configuration']) || !empty($info['exportable']);
    $info += array('rules controller class' => $configuration ? FALSE : 'EntityDefaultRulesController');
    if ($info['rules controller class']) {
      $controller = new $info['rules controller class']($type);
      $items += $controller->eventInfo();
    }
  }
  return $items;
}

/**
 * Rules integration access callback.
 */
function entity_rules_integration_event_access($type, $event_name) {
  // Cut of _insert/_update/.. from the event name.
  $entity_type = substr($event_name, 0, strrpos($event_name, '_'));

  $result = entity_access('view', $entity_type);
  // If no access callback is given, just grant access for viewing.
  return isset($result) ? $result : TRUE;
}
