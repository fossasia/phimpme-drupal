<?php

/**
 * @file
 * Contains the entity_views_handler_field_field class.
 */

/**
 * A handler to provide proper displays for Field API fields.
 *
 * Overrides the default Views handler to retrieve the data from an entity via
 * data selection.
 *
 * This handler may only be used in conjunction with data selection based Views
 * tables or other base tables using a query plugin that supports data
 * selection.
 *
 * @see entity_views_field_definition()
 * @ingroup views_field_handlers
 */
class entity_views_handler_field_field extends views_handler_field_field {

  /**
   * Stores the entity type of the result entities.
   */
  public $entity_type;

  /**
   * Stores the result entities' metadata wrappers.
   */
  public $wrappers = array();

  /**
   * The entity for which this field is currently rendered.
   */
  public $entity;

  /**
   * Return TRUE if the user has access to view this field.
   */
  public function access() {
    return field_access('view', $this->field_info, $this->definition['entity type']);
  }

  /**
   * Overridden to add the field for the entity ID (if necessary).
   */
  public function query($use_groupby = FALSE) {
    EntityFieldHandlerHelper::query($this);
  }

  /**
   * Adds a click-sort to the query.
   */
  public function click_sort($order) {
    EntityFieldHandlerHelper::click_sort($this, $order);
  }

  /**
   * Override so it doesn't do any harm (or, anything at all).
   */
  public function post_execute(&$values) { }

  /**
   * Load the entities for all rows that are about to be displayed.
   */
  public function pre_render(&$values) {
    parent::pre_render($values);
    EntityFieldHandlerHelper::pre_render($this, $values, TRUE);
  }

  /**
   * Overridden to get the items our way.
   */
  public function get_items($values) {
    $items = array();
    // Set the entity type for the parent handler.
    $values->_field_data[$this->field_alias]['entity_type'] = $this->entity_type;
    // We need special handling for lists of entities as the base.
    $entities = EntityFieldHandlerHelper::get_value($this, $values, 'entity object');
    if (!is_array($entities)) {
      $entities = array($entities);
    }
    foreach ($entities as $entity) {
      // Only try to render the field if it is even present on this bundle.
      // Otherwise, field_view_field() will trigger a fatal.
      list (, , $bundle) = entity_extract_ids($this->entity_type, $entity);
      if (field_info_instance($this->entity_type, $this->definition['field_name'], $bundle)) {
        // Set the currently rendered entity.
        $values->_field_data[$this->field_alias]['entity'] = $this->entity = $entity;
        $items = array_merge($items, $this->set_items($values, $this->view->row_index));
      }
    }
    return $items;
  }

  /**
   * Overridden to get the entity and put it where it is assumed to be.
   */
  public function get_value($values, $field = NULL) {
    if (!$this->entity) {
      return NULL;
    }
    if ($field == 'entity') {
      return $this->entity;
    }
    return parent::get_value($values, $field);
  }
}
