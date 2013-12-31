<?php

/**
 * @file
 * Contains the entity_views_handler_field_entity class.
 */

/**
 * A handler to provide proper displays for entities retrieved via data selection.
 *
 * This handler may only be used in conjunction with data selection based Views
 * tables or other base tables using a query plugin that supports data
 * selection.
 *
 * @see entity_views_field_definition()
 * @ingroup views_field_handlers
 */
class entity_views_handler_field_entity extends views_handler_field {

  /**
   * Stores the entity type of the result entities.
   */
  public $entity_type;

  /**
   * Stores the result entities' metadata wrappers.
   */
  public $wrappers = array();

  /**
   * The entity type of the entity displayed by this field.
   */
  public $field_entity_type;

  /**
   * Stores the current value when rendering list fields.
   */
  public $current_value;

  /**
   * Initialize the entity type with the field's entity type.
   */
  public function init(&$view, &$options) {
    parent::init($view, $options);
    $this->field_entity_type = entity_property_extract_innermost_type($this->definition['type']);
  }

  /**
   * Overridden to add the field for the entity ID (if necessary).
   */
  public function query() {
    EntityFieldHandlerHelper::query($this);
  }

  /**
   * Adds a click-sort to the query.
   */
  public function click_sort($order) {
    EntityFieldHandlerHelper::click_sort($this, $order);
  }

  /**
   * Load the entities for all rows that are about to be displayed.
   */
  public function pre_render(&$values) {
    EntityFieldHandlerHelper::pre_render($this, $values);
  }

  /**
   * Overridden to use a metadata wrapper.
   */
  public function get_value($values, $field = NULL) {
    return EntityFieldHandlerHelper::get_value($this, $values, $field);
  }

  public function option_definition() {
    $options = parent::option_definition();
    $options += EntityFieldHandlerHelper::option_definition($this);

    $options['display'] = array('default' => 'label');
    $options['link_to_entity']['default'] = TRUE;
    $options['view_mode'] = array('default' => 'default');

    return $options;
  }

  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    EntityFieldHandlerHelper::options_form($this, $form, $form_state);
    // We want a different form field at a different place.
    unset($form['link_to_entity']);

    $options = array(
      'label' => t('Show entity label'),
      'id' => t('Show entity ID'),
      'view' => t('Show complete entity'),
    );
    $form['display'] = array(
      '#type' => 'select',
      '#title' => t('Display'),
      '#description' => t('Decide how this field will be displayed.'),
      '#options' => $options,
      '#default_value' => $this->options['display'],
    );
    $form['link_to_entity'] = array(
      '#type' => 'checkbox',
      '#title' => t('Link to entity'),
      '#description' => t('Link this field to the entity.'),
      '#default_value' => $this->options['link_to_entity'],
      '#dependency' => array('edit-options-display' => array('label', 'id')),
    );

    // Stolen from entity_views_plugin_row_entity_view.
    $entity_info = entity_get_info($this->field_entity_type);
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
        '#dependency' => array('edit-options-display' => array('view')),
      );
    }
    else {
      $form['view_mode'] = array(
        '#type' => 'value',
        '#value' => $options ? key($options) : 'default',
      );
    }
  }

  public function render($values) {
    return EntityFieldHandlerHelper::render($this, $values);
  }

  /**
   * Render a value as a link to the entity if applicable.
   *
   * @param $value
   *   The value to render.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   */
  public function render_entity_link($entity, $values) {
    $type = $this->field_entity_type;
    if (!is_object($entity) && isset($entity) && $entity !== FALSE) {
      $entity = entity_load_single($type, $entity);
    }
    if (!$entity) {
      return '';
    }
    $render = $this->render_single_value($entity, $values);
    if (!$this->options['link_to_entity']) {
      return $render;
    }
    if (is_object($entity) && ($url = entity_uri($type, $entity))) {
      return l($render, $url['path'], array('html' => TRUE) + $url['options']);
    }
    return $render;
  }

  /**
   * Render a single field value.
   */
  public function render_single_value($entity, $values) {
    $type = $this->field_entity_type;
    if (!is_object($entity) && isset($entity) && $entity !== FALSE) {
      $entity = entity_load_single($type, $entity);
    }
    if (!$entity) {
      return '';
    }

    if ($this->options['display'] === 'view') {
      $entity_view = entity_view($type, array($entity), $this->options['view_mode']);
      return render($entity_view);
    }

    if ($this->options['display'] == 'label') {
      $value = entity_label($type, $entity);
    }
    // Either $options[display] == 'id', or we have no label.
    if (empty($value)) {
      $value = entity_id($type, $entity);
    }
    $value = $this->sanitize_value($value);

    return $value;
  }

}
