<?php

/**
 * @file
 * Contains the entity_views_handler_field_duration class.
 */

/**
 * A handler to provide proper displays for duration properties retrieved via data selection.
 *
 * This handler may only be used in conjunction with data selection based Views
 * tables or other base tables using a query plugin that supports data
 * selection.
 *
 * @see entity_views_field_definition()
 * @ingroup views_field_handlers
 */
class entity_views_handler_field_duration extends views_handler_field {

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

  public function option_definition() {
    $options = parent::option_definition();
    $options += EntityFieldHandlerHelper::option_definition($this);

    $options['format_interval'] = array('default' => TRUE);
    $options['granularity'] = array('default' => 2);
    $options['prefix'] = array('default' => '', 'translatable' => TRUE);
    $options['suffix'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    EntityFieldHandlerHelper::options_form($this, $form, $form_state);

    $form['format_interval'] = array(
      '#type' => 'checkbox',
      '#title' => t('Format interval'),
      '#description' => t('If checked, the value will be formatted as a time interval. Otherwise, just the number of seconds will be displayed.'),
      '#default_value' => $this->options['format_interval'],
    );
    $form['granularity'] = array(
      '#type' => 'textfield',
      '#title' => t('Granularity'),
      '#default_value' => $this->options['granularity'],
      '#description' => t('Specify how many different units to display.'),
      '#dependency' => array('edit-options-format-interval' => array(TRUE)),
      '#size' => 2,
    );
    $form['prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
      '#default_value' => $this->options['prefix'],
      '#description' => t('Text to put before the duration text.'),
    );
    $form['suffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Suffix'),
      '#default_value' => $this->options['suffix'],
      '#description' => t('Text to put after the duration text.'),
    );
  }

  /**
   * Render the field.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  public function render($values) {
    return EntityFieldHandlerHelper::render($this, $values);
  }

  /**
   * Render a single field value.
   */
  public function render_single_value($value, $values) {
    if ($this->options['format_interval']) {
      $value = format_interval($value, (int) $this->options['granularity']);
    }
    return $this->sanitize_value($this->options['prefix'], 'xss') .
        $this->sanitize_value($value) .
        $this->sanitize_value($this->options['suffix'], 'xss');
  }

}
