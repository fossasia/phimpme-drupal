<?php

/**
 * @file
 * Contains the entity_views_handler_field_options class.
 */

/**
 * A handler to provide proper displays for values chosen from a set of options.
 *
 * This handler may only be used in conjunction with data selection based Views
 * tables or other base tables using a query plugin that supports data
 * selection.
 *
 * @see entity_views_field_definition()
 * @ingroup views_field_handlers
 */
class entity_views_handler_field_options extends views_handler_field {

  /**
   * Stores the entity type of the result entities.
   */
  public $entity_type;

  /**
   * Stores the result entities' metadata wrappers.
   */
  public $wrappers = array();

  /**
   * Stores the current value when rendering list fields.
   */
  public $current_value;

  /**
   * The key / name mapping for the options.
   */
  public $option_list;

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
    parent::pre_render($values);
    EntityFieldHandlerHelper::pre_render($this, $values);
  }

  /**
   * Overridden to use a metadata wrapper.
   */
  public function get_value($values, $field = NULL) {
    return EntityFieldHandlerHelper::get_value($this, $values, $field);
  }

  /**
   * Specifies the options this handler uses.
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options += EntityFieldHandlerHelper::option_definition($this);
    $options['format_name'] = array('default' => TRUE);
    return $options;
  }

  /**
   * Returns an option form for setting this handler's options.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    EntityFieldHandlerHelper::options_form($this, $form, $form_state);

    $form['format_name'] = array(
      '#title' => t('Use human-readable name'),
      '#type' => 'checkbox',
      '#description' => t("If this is checked, the values' names will be displayed instead of their internal identifiers."),
      '#default_value' => $this->options['format_name'],
      '#weight' => -5,
    );
  }

  public function render($values) {
    return EntityFieldHandlerHelper::render($this, $values);
  }

  /**
   * Render a single field value.
   */
  public function render_single_value($value, $values) {
    if (!isset($this->option_list)) {
      $this->option_list = array();
      $callback = $this->definition['options callback'];
      if (is_callable($callback['function'])) {
        // If a selector is used, get the name of the selected field.
        $field_name = EntityFieldHandlerHelper::get_selector_field_name($this->real_field);
        $this->option_list = call_user_func($callback['function'], $field_name, $callback['info'], 'view');
      }
    }
    if ($this->options['format_name'] && isset($this->option_list[$value])) {
      $value = $this->option_list[$value];
    }

    return $this->sanitize_value($value);
  }

}
