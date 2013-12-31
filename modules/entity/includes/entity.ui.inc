<?php

/**
 * @file
 * Provides a controller for building an entity overview form.
 */

/**
 * Default controller for providing UI.
 */
class EntityDefaultUIController {

  protected $entityType;
  protected $entityInfo, $path;

  public function __construct($entity_type, $entity_info) {
    $this->entityType = $entity_type;
    $this->entityInfo = $entity_info;
    $this->path = $this->entityInfo['admin ui']['path'];
    $this->statusKey = empty($this->entityInfo['entity keys']['status']) ? 'status' : $this->entityInfo['entity keys']['status'];
  }

  /**
   * Provides definitions for implementing hook_menu().
   */
  public function hook_menu() {
    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';
    $plural_label = isset($this->entityInfo['plural label']) ? $this->entityInfo['plural label'] : $this->entityInfo['label'] . 's';

    $items[$this->path] = array(
      'title' => $plural_label,
      'page callback' => 'drupal_get_form',
      'page arguments' => array($this->entityType . '_overview_form', $this->entityType),
      'description' => 'Manage ' . $plural_label . '.',
      'access callback' => 'entity_access',
      'access arguments' => array('view', $this->entityType),
      'file' => 'includes/entity.ui.inc',
    );
    $items[$this->path . '/add'] = array(
      'title callback' => 'entity_ui_get_action_title',
      'title arguments' => array('add', $this->entityType),
      'page callback' => 'entity_ui_get_form',
      'page arguments' => array($this->entityType, NULL, 'add'),
      'access callback' => 'entity_access',
      'access arguments' => array('create', $this->entityType),
      'type' => MENU_LOCAL_ACTION,
    );
    $items[$this->path . '/manage/' . $wildcard] = array(
      'title' => 'Edit',
      'title callback' => 'entity_label',
      'title arguments' => array($this->entityType, $id_count + 1),
      'page callback' => 'entity_ui_get_form',
      'page arguments' => array($this->entityType, $id_count + 1),
      'load arguments' => array($this->entityType),
      'access callback' => 'entity_access',
      'access arguments' => array('update', $this->entityType, $id_count + 1),
    );
    $items[$this->path . '/manage/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'load arguments' => array($this->entityType),
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );

    // Clone form, a special case for the edit form.
    $items[$this->path . '/manage/' . $wildcard . '/clone'] = array(
      'title' => 'Clone',
      'page callback' => 'entity_ui_get_form',
      'page arguments' => array($this->entityType, $id_count + 1, 'clone'),
      'load arguments' => array($this->entityType),
      'access callback' => 'entity_access',
      'access arguments' => array('create', $this->entityType),
    );
    // Menu item for operations like revert and delete.
    $items[$this->path . '/manage/' . $wildcard . '/%'] = array(
      'page callback' => 'drupal_get_form',
      'page arguments' => array($this->entityType . '_operation_form', $this->entityType, $id_count + 1, $id_count + 2),
      'load arguments' => array($this->entityType),
      'access callback' => 'entity_access',
      'access arguments' => array('delete', $this->entityType, $id_count + 1),
      'file' => 'includes/entity.ui.inc',
    );

    if (!empty($this->entityInfo['exportable'])) {
      // Menu item for importing an entity.
      $items[$this->path . '/import'] = array(
        'title callback' => 'entity_ui_get_action_title',
        'title arguments' => array('import', $this->entityType),
        'page callback' => 'drupal_get_form',
        'page arguments' => array($this->entityType . '_operation_form', $this->entityType, NULL, 'import'),
        'access callback' => 'entity_access',
        'access arguments' => array('create', $this->entityType),
        'file' => 'includes/entity.ui.inc',
        'type' => MENU_LOCAL_ACTION,
      );
    }

    if (!empty($this->entityInfo['admin ui']['file'])) {
      // Add in the include file for the entity form.
      foreach (array("/manage/$wildcard", "/manage/$wildcard/clone", '/add') as $path_end) {
        $items[$this->path . $path_end]['file'] = $this->entityInfo['admin ui']['file'];
        $items[$this->path . $path_end]['file path'] = isset($this->entityInfo['admin ui']['file path']) ? $this->entityInfo['admin ui']['file path'] : drupal_get_path('module', $this->entityInfo['module']);
      }
    }
    return $items;
  }

  /**
   * Provides definitions for implementing hook_forms().
   *
   * Use per bundle form ids if possible, such that easy per bundle alterations
   * are supported too.
   *
   * Note that for performance reasons, this method is only invoked for forms,
   * which receive the entity_type as first argument. Thus any forms added, must
   * follow that pattern.
   *
   * @see entity_forms()
   */
  public function hook_forms() {
    // The overview and the operation form are implemented by the controller,
    // the callback and validation + submit handlers just invoke the controller.
    $forms[$this->entityType . '_overview_form'] = array(
      'callback' => 'entity_ui_overview_form',
      'wrapper_callback' => 'entity_ui_form_defaults',
    );
    $forms[$this->entityType . '_operation_form'] = array(
      'callback' => 'entity_ui_operation_form',
      'wrapper_callback' => 'entity_ui_form_defaults',
    );

    // The entity form (ENTITY_TYPE_form) handles editing, adding and cloning.
    // For that form, the wrapper callback entity_ui_main_form_defaults() gets
    // directly invoked via entity_ui_get_form().
    // If there are bundles though, we use form ids that include the bundle name
    // (ENTITY_TYPE_edit_BUNDLE_NAME_form) to enable per bundle alterations
    // as well as alterations based upon the base form id (ENTITY_TYPE_form).
    if (!(count($this->entityInfo['bundles']) == 1 && isset($this->entityInfo['bundles'][$this->entityType]))) {
      foreach ($this->entityInfo['bundles'] as $bundle => $bundle_info) {
        $forms[$this->entityType . '_edit_' . $bundle . '_form']['callback'] = $this->entityType . '_form';
        // Again the wrapper callback is invoked by entity_ui_get_form() anyway.
      }
    }
    return $forms;
  }

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    // By default just show a simple overview for all entities.
    $form['table'] = $this->overviewTable();
    return $form;
  }

  /**
   * Overview form validation callback.
   */
  public function overviewFormValidate() {}

  /**
   * Overview form submit callback.
   */
  public function overviewFormSubmit() {}


  /**
   * Generates the render array for a overview table for arbitrary entities
   * matching the given conditions.
   *
   * @param $conditions
   *   An array of conditions as needed by entity_load().

   * @return Array
   *   A renderable array.
   */
  public function overviewTable($conditions = array()) {
    $entities = entity_load($this->entityType, FALSE, $conditions);
    ksort($entities);

    $rows = array();
    foreach ($entities as $entity) {
      $rows[] = $this->overviewTableRow($conditions, entity_id($this->entityType, $entity), $entity);
    }
    // Assemble the right table header.
    $header = array(t('Label'));
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    // Add operations with the right colspan.
    $field_ui = !empty($this->entityInfo['bundle of']) && module_exists('field_ui');
    $exportable = !empty($this->entityInfo['exportable']);
    $colspan = 3;
    $colspan = $field_ui ? $colspan + 2 : $colspan;
    $colspan = $exportable ? $colspan + 1 : $colspan;
    $header[] = array('data' => t('Operations'), 'colspan' => $colspan);

    $render = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
  }

  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   *   Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $entity_uri = entity_uri($this->entityType, $entity);

    $row[] = array('data' => array(
      '#theme' => 'entity_ui_overview_item',
      '#label' => entity_label($this->entityType, $entity),
      '#name' => !empty($this->entityInfo['exportable']) ? entity_id($this->entityType, $entity) : FALSE,
      '#url' => $entity_uri ? $entity_uri : FALSE,
      '#entity_type' => $this->entityType),
    );

    // Add in any passed additional cols.
    foreach ($additional_cols as $col) {
      $row[] = $col;
    }

    // Add a row for the exportable status.
    if (!empty($this->entityInfo['exportable'])) {
      $row[] = array('data' => array(
        '#theme' => 'entity_status',
        '#status' => $entity->{$this->statusKey},
      ));
    }
    // In case this is a bundle, we add links to the field ui tabs.
    $field_ui = !empty($this->entityInfo['bundle of']) && module_exists('field_ui');
    // For exportable entities we add an export link.
    $exportable = !empty($this->entityInfo['exportable']);
    $colspan = 3;
    $colspan = $field_ui ? $colspan + 2 : $colspan;
    $colspan = $exportable ? $colspan + 1 : $colspan;

    // Add operations depending on the status.
    if (entity_has_status($this->entityType, $entity, ENTITY_FIXED)) {
      $row[] = array('data' => l(t('clone'), $this->path . '/manage/' . $id . '/clone'), 'colspan' => $colspan);
    }
    else {
      $row[] = l(t('edit'), $this->path . '/manage/' . $id);

      if ($field_ui) {
        $row[] = l(t('manage fields'), $this->path . '/manage/' . $id . '/fields');
        $row[] = l(t('manage display'), $this->path . '/manage/' . $id . '/display');
      }

      $row[] = l(t('clone'), $this->path . '/manage/' . $id . '/clone');
      if (empty($this->entityInfo['exportable']) || !entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
        $row[] = l(t('delete'), $this->path . '/manage/' . $id . '/delete', array('query' => drupal_get_destination()));
      }
      elseif (entity_has_status($this->entityType, $entity, ENTITY_OVERRIDDEN)) {
        $row[] = l(t('revert'), $this->path . '/manage/' . $id . '/revert', array('query' => drupal_get_destination()));
      }
      else {
        $row[] = '';
      }
    }
    if ($exportable) {
      $row[] = l(t('export'), $this->path . '/manage/' . $id . '/export');
    }
    return $row;
  }


  /**
   * Builds the operation form.
   *
   * For the export operation a serialized string of the entity is directly
   * shown in the form (no submit function needed).
   */
  public function operationForm($form, &$form_state, $entity, $op) {
    switch ($op) {
      case 'revert':
        $label = entity_label($this->entityType, $entity);
        $confirm_question = t('Are you sure you want to revert the %entity %label?', array('%entity' => $this->entityInfo['label'], '%label' => $label));
        return confirm_form($form, $confirm_question, $this->path);

      case 'delete':
        $label = entity_label($this->entityType, $entity);
        $confirm_question = t('Are you sure you want to delete the %entity %label?', array('%entity' => $this->entityInfo['label'], '%label' => $label));
        return confirm_form($form, $confirm_question, $this->path);

      case 'export':
        if (!empty($this->entityInfo['exportable'])) {
          $export = entity_export($this->entityType, $entity);
          $form['export'] = array(
            '#type' => 'textarea',
            '#title' => t('Export'),
            '#description' => t('For importing copy the content of the text area and paste it into the import page.'),
            '#rows' => 25,
            '#default_value' => $export,
          );
          return $form;
        }

      case 'import':
        $form['import'] = array(
          '#type' => 'textarea',
          '#title' => t('Import'),
          '#description' => t('Paste an exported %entity_type here.', array('%entity_type' => $this->entityInfo['label'])),
          '#rows' => 20,
        );
        $form['overwrite'] = array(
          '#title' => t('Overwrite'),
          '#type' => 'checkbox',
          '#description' => t('If checked, any existing %entity with the same identifier will be replaced by the import.', array('%entity' => $this->entityInfo['label'])),
          '#default_value' => FALSE,
        );
        $form['submit'] = array(
          '#type' => 'submit',
          '#value' => t('Import'),
        );
        return $form;
    }
    drupal_not_found();
    exit;
  }

  /**
   * Operation form validation callback.
   */
  public function operationFormValidate($form, &$form_state) {
    if ($form_state['op'] == 'import') {
      if ($entity = entity_import($this->entityType, $form_state['values']['import'])) {
        // Store the successfully imported entity in $form_state.
        $form_state[$this->entityType] = $entity;
        if (!$form_state['values']['overwrite']) {
          // Check for existing entities with the same identifier.
          $id = entity_id($this->entityType, $entity);
          $entities = entity_load($this->entityType, array($id));
          if (!empty($entities)) {
            $label = entity_label($this->entityType, $entity);
            $vars = array('%entity' => $this->entityInfo['label'], '%label' => $label);
            form_set_error('import', t('Import of %entity %label failed, a %entity with the same machine name already exists. Check the overwrite option to replace it.', $vars));
          }
        }
      }
      else {
        form_set_error('import', t('Import failed.'));
      }
    }
  }

  /**
   * Operation form submit callback.
   */
  public function operationFormSubmit($form, &$form_state) {
    $msg = $this->applyOperation($form_state['op'], $form_state[$this->entityType]);
    drupal_set_message($msg);
    $form_state['redirect'] = $this->path;
  }

  /**
   * Applies an operation to the given entity.
   *
   * Note: the export operation is directly carried out by the operationForm()
   * method.
   *
   * @param string $op
   *   The operation (revert, delete or import).
   * @param $entity
   *   The entity to manipulate.
   *
   * @return
   *   The status message of what has been applied.
   */
  public function applyOperation($op, $entity) {
    $label = entity_label($this->entityType, $entity);
    $vars = array('%entity' => $this->entityInfo['label'], '%label' => $label);
    $id = entity_id($this->entityType, $entity);
    $edit_link = l(t('edit'), $this->path . '/manage/' . $id . '/edit');

    switch ($op) {
      case 'revert':
        entity_delete($this->entityType, $id);
        watchdog($this->entityType, 'Reverted %entity %label to the defaults.', $vars, WATCHDOG_NOTICE, $edit_link);
        return t('Reverted %entity %label to the defaults.', $vars);

      case 'delete':
        entity_delete($this->entityType, $id);
        watchdog($this->entityType, 'Deleted %entity %label.', $vars);
        return t('Deleted %entity %label.', $vars);

      case 'import':
        // First check if there is any existing entity with the same ID.
        $id = entity_id($this->entityType, $entity);
        $entities = entity_load($this->entityType, array($id));
        if ($existing_entity = reset($entities)) {
          // Copy DB id and remove the new indicator to overwrite the DB record.
          $idkey = $this->entityInfo['entity keys']['id'];
          $entity->{$idkey} = $existing_entity->{$idkey};
          unset($entity->is_new);
        }
        entity_save($this->entityType, $entity);
        watchdog($this->entityType, 'Imported %entity %label.', $vars);
        return t('Imported %entity %label.', $vars);

      default:
        return FALSE;
    }
  }

  /**
   * Entity submit builder invoked via entity_ui_form_submit_build_entity().
   *
   * Extracts the form values and updates the entity.
   *
   * The provided implementation makes use of the helper function
   * entity_form_submit_build_entity() provided by core, which already invokes
   * the field API attacher for fieldable entities.
   *
   * @return
   *   The updated entity.
   *
   * @see entity_ui_form_submit_build_entity()
   */
  public function entityFormSubmitBuildEntity($form, &$form_state) {
    entity_form_submit_build_entity($this->entityType, $form_state[$this->entityType], $form, $form_state);
    return $form_state[$this->entityType];
  }
}



/**
 * Form builder function for the overview form.
 *
 * @see EntityDefaultUIController::overviewForm()
 */
function entity_ui_overview_form($form, &$form_state, $entity_type) {
  return entity_ui_controller($entity_type)->overviewForm($form, $form_state);
}

/**
 * Form builder for the entity operation form.
 *
 * @see EntityDefaultUIController::operationForm()
 */
function entity_ui_operation_form($form, &$form_state, $entity_type, $entity, $op) {
  $form_state['op'] = $op;
  return entity_ui_controller($entity_type)->operationForm($form, $form_state, $entity, $op);
}

/**
 * Form wrapper the main entity form.
 *
 * @see entity_ui_form_defaults()
 */
function entity_ui_main_form_defaults($form, &$form_state, $entity = NULL, $op = NULL) {
  if ($op == 'clone') {
    // Clone the entity and make sure it will get saved as a new entity.
    $entity = clone $entity;
    $entity_info = entity_get_info($form_state['entity_type']);
    $entity->{$entity_info['entity keys']['id']} = FALSE;
    $entity->is_new = TRUE;
    // Make sure the status of a cloned exportable is custom.
    if (!empty($entity_info['exportable'])) {
      $status_key = isset($entity_info['entity keys']['status']) ? $entity_info['entity keys']['status'] : 'status';
      $entity->$status_key = ENTITY_CUSTOM;
    }
  }
  return entity_ui_form_defaults($form, $form_state, $form_state['entity_type'], $entity, $op);
}

/**
 * Form wrapper callback for all entity ui forms.
 *
 * This callback makes sure the form state is properly initialized and sets
 * some useful default titles.
 *
 * @see EntityDefaultUIController::hook_forms()
 */
function entity_ui_form_defaults($form, &$form_state, $entity_type, $entity = NULL, $op = NULL) {
  $defaults = array(
    'entity_type' => $entity_type,
  );
  if (isset($entity)) {
    $defaults[$entity_type] = $entity;
  }
  if (isset($op)) {
    $defaults['op'] = $op;
  }
  $form_state += $defaults;
  if (isset($op)) {
    drupal_set_title(entity_ui_get_page_title($op, $entity_type, $entity), PASS_THROUGH);
  }
  // Add in handlers pointing to the controller for the forms implemented by it.
  if (isset($form_state['build_info']['base_form_id']) && $form_state['build_info']['base_form_id'] != $entity_type . '_form') {
    $form['#validate'][] = 'entity_ui_controller_form_validate';
    $form['#submit'][] = 'entity_ui_controller_form_submit';
  }
  return $form;
}

/**
 * Validation callback for forms implemented by the UI controller.
 */
function entity_ui_controller_form_validate($form, &$form_state) {
  // Remove 'entity_ui_' prefix and the '_form' suffix.
  $base = substr($form_state['build_info']['base_form_id'], 10, -5);
  $method = $base . 'FormValidate';
  entity_ui_controller($form_state['entity_type'])->$method($form, $form_state);
}

/**
 * Submit callback for forms implemented by the UI controller.
 */
function entity_ui_controller_form_submit($form, &$form_state) {
  // Remove 'entity_ui_' prefix and the '_form' suffix.
  $base = substr($form_state['build_info']['base_form_id'], 10, -5);
  $method = $base . 'FormSubmit';
  entity_ui_controller($form_state['entity_type'])->$method($form, $form_state);
}

/**
 * Gets the page title for the passed operation.
 */
function entity_ui_get_page_title($op, $entity_type, $entity = NULL) {
  $label = entity_label($entity_type, $entity);
  switch ($op) {
    case 'edit':
      return t('Edit @label', array('@label' => $label));
    case 'clone':
      return t('Clone @label', array('@label' => $label));
    case 'revert':
      return t('Revert @label', array('@label' => $label));
    case 'delete':
      return t('Delete @label', array('@label' => $label));
    case 'export':
      return t('Export @label', array('@label' => $label));
  }
  return entity_ui_get_action_title($op, $entity_type);
}

/**
 * Gets the page/menu title for local action operations.
 */
function entity_ui_get_action_title($op, $entity_type) {
  $info = entity_get_info($entity_type);
  switch ($op) {
    case 'add':
      return t('Add @entity_type', array('@entity_type' => drupal_strtolower($info['label'])));
    case 'import':
      return t('Import @entity_type', array('@entity_type' => drupal_strtolower($info['label'])));
  }
}

/**
 * Submit builder for the main entity form, which extracts the form values and updates the entity.
 *
 * This is a helper function for entities making use of the entity UI
 * controller.
 *
 * @return
 *   The updated entity.
 *
 * @see EntityDefaultUIController::hook_forms()
 * @see EntityDefaultUIController::entityFormSubmitBuildEntity()
 */
function entity_ui_form_submit_build_entity($form, &$form_state) {
  return entity_ui_controller($form_state['entity_type'])->entityFormSubmitBuildEntity($form, $form_state);
}

/**
 * Validation callback for machine names of exportables.
 *
 * We don't allow numeric machine names, as entity_load() treats them as the
 * numeric identifier and they are easily confused with ids in general.
 */
function entity_ui_validate_machine_name($element, &$form_state) {
  if (is_numeric($element['#value'])) {
    form_error($element, t('Machine-readable names must not consist of numbers only.'));
  }
}

/**
 * Returns HTML for an entity on the entity overview listing.
 *
 * @ingroup themeable
 */
function theme_entity_ui_overview_item($variables) {
  $output = $variables['url'] ? l($variables['label'], $variables['url']['path'], $variables['url']['options']) : check_plain($variables['label']);
  if ($variables['name']) {
    $output .= ' <small> (' . t('Machine name') . ': ' . check_plain($variables['name']) . ')</small>';
  }
  return $output;
}
