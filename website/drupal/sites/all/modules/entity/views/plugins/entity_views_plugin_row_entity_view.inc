<?php

/**
 * @file
 * Row style plugin for displaying the results as entities.
 */

/**
 * Plugin class for displaying Views results with entity_view.
 */
class entity_views_plugin_row_entity_view extends views_plugin_row {

  protected $entity_type, $entities;

  public function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);

    // Initialize the entity-type used.
    $table_data = views_fetch_data($this->view->base_table);
    $this->entity_type = $table_data['table']['entity type'];
    // Set base table and field information as used by views_plugin_row to
    // select the entity id if used with default query class.
    $info = entity_get_info($this->entity_type);
    if (!empty($info['base table']) && $info['base table'] == $this->view->base_table) {
      $this->base_table = $info['base table'];
      $this->base_field = $info['entity keys']['id'];
    }
  }

  public function option_definition() {
    $options = parent::option_definition();
    $options['view_mode'] = array('default' => 'full');
    return $options;
  }

  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $entity_info = entity_get_info($this->entity_type);
    $options = array();
    if (!empty($entity_info['view modes'])) {
      foreach ($entity_info['view modes'] as $mode => $settings) {
        $options[$mode] = $settings['label'];
      }
    }

    if (count($options) > 1) {
      $form['view_mode'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('View mode'),
        '#default_value' => $this->options['view_mode'],
      );
    }
    else {
      $form['view_mode_info'] = array(
        '#type' => 'item',
        '#title' => t('View mode'),
        '#description' => t('Only one view mode is available for this entity type.'),
        '#markup' => $options ? current($options) : t('Default'),
      );
      $form['view_mode'] = array(
        '#type' => 'value',
        '#value' => $options ? key($options) : 'default',
      );
    }
    return $form;
  }

  public function pre_render(&$values) {
    if (!empty($values)) {
      list($this->entity_type, $this->entities) = $this->view->query->get_result_entities($values, !empty($this->relationship) ? $this->relationship : NULL, isset($this->field_alias) ? $this->field_alias : NULL);
    }
    // Render the entities.
    if ($this->entities) {
      $render = entity_view($this->entity_type, $this->entities, $this->options['view_mode']);
      // Remove the first level of the render array.
      $this->rendered_content = reset($render);
    }
  }

  /**
   * Overridden to return the entity object.
   */
  function get_value($values, $field = NULL) {
    return isset($this->entities[$this->view->row_index]) ? $this->entities[$this->view->row_index] : FALSE;
  }

  public function render($values) {
    if ($entity = $this->get_value($values)) {
      $render = $this->rendered_content[entity_id($this->entity_type, $entity)];
      return drupal_render($render);
    }
  }
}
