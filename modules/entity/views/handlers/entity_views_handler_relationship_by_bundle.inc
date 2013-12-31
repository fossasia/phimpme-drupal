<?php
/**
 * @file
 * Contains the entity_views_handler_relationship_by_bundle class.
 */

/**
 * Relationship handler for entity relationships that may limit the join to one or more bundles.
 *
 * This handler is only applicable for entities that are using bundle entities,
 * i.e. entities having the 'bundle of' entity info key set.
 *
 * For example, this allows a relationship from users to profiles of a certain
 * profile type.
 *
 * @see entity_crud_hook_entity_info()
 * @ingroup views_field_handlers
 */
class entity_views_handler_relationship_by_bundle extends views_handler_relationship {
  function option_definition() {
    $options = parent::option_definition();
    $options['bundle_types'] = array('default' => array());

    return $options;
  }

  /**
   * Add an entity type option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Get the entity type and info from the table data for the base on the
    // right hand side of the relationship join.
    $table_data = views_fetch_data($this->definition['base']);
    $entity_type = $table_data['table']['entity type'];
    $entity_info = entity_get_info($entity_type);

    // Get the info of the bundle entity.
    foreach (entity_get_info() as $type => $info) {
      if (isset($info['bundle of']) && $info['bundle of'] == $entity_type) {
        $entity_bundle_info = $info;
        break;
      }
    }

    $plural_label = isset($entity_bundle_info['plural label']) ? $entity_bundle_info['plural label'] : $entity_bundle_info['label'] . 's';
    $bundle_options = array();
    foreach ($entity_info['bundles'] as $name => $info) {
      $bundle_options[$name] = $info['label'];
    }

    $form['bundle_types'] = array(
      '#title' => $plural_label,
      '#type' => 'checkboxes',
      '#description' => t('Restrict this relationship to one or more @bundles.', array('@bundles' => strtolower($entity_bundle_info['plural label']))),
      '#options' => $bundle_options,
      '#default_value' => $this->options['bundle_types'],
    );
  }

  /**
   * Make sure only checked bundle types are left.
   */
  function options_submit(&$form, &$form_state) {
    $form_state['values']['options']['bundle_types'] = array_filter($form_state['values']['options']['bundle_types']);
    parent::options_submit($form, $form_state);
  }

  /**
   * Called to implement a relationship in a query.
   *
   * Mostly the same as the parent method, except we add an extra clause to
   * the join.
   */
  function query() {
    $table_data = views_fetch_data($this->definition['base']);
    $base_field = empty($this->definition['base field']) ? $table_data['table']['base']['field'] : $this->definition['base field'];
    $this->ensure_my_table();

    $def = $this->definition;
    $def['table'] = $this->definition['base'];
    $def['field'] = $base_field;
    $def['left_table'] = $this->table_alias;
    $def['left_field'] = $this->field;
    if (!empty($this->options['required'])) {
      $def['type'] = 'INNER';
    }

    // Add an extra clause to the join if there are bundle types selected.
    if ($this->options['bundle_types']) {
      $entity_info = entity_get_info($table_data['table']['entity type']);
      $def['extra'] = array(
        array(
          // The table and the IN operator are implicit.
          'field' => $entity_info['bundle keys']['bundle'],
          'value' => $this->options['bundle_types'],
        ),
      );
    }

    if (!empty($def['join_handler']) && class_exists($def['join_handler'])) {
      $join = new $def['join_handler'];
    }
    else {
      $join = new views_join();
    }

    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;

    // Use a short alias for this.
    $alias = $def['table'] . '_' . $this->table;
    $this->alias = $this->query->add_relationship($alias, $join, $this->definition['base'], $this->relationship);
  }
}
