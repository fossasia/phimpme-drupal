<?php

/**
 * @file
 * Contains the entity_views_handler_relationship class.
 */

/**
 * Relationship handler for data selection tables.
 *
 * This handler may only be used in conjunction with data selection based Views
 * tables or other base tables using a query plugin that supports data
 * selection.
 *
 * @see entity_views_field_definition()
 * @ingroup views_field_handlers
 */
class entity_views_handler_relationship extends views_handler_relationship {

  /**
   * Slightly modify the options form provided by the parent handler.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    // This won't work with data selector-based relationships, as we only
    // inspect those *after* the results are known.
    $form['required']['#access'] = FALSE;
    // Notify the user of our restrictions regarding lists of entities, if
    // appropriate.
    if (!empty($this->definition['multiple'])) {
      $form['multiple_note'] = array(
        '#markup' => t('<strong>Note:</strong> This is a multi-valued relationship, which is currently not supported. ' .
            'Only the first related entity will be shown.'),
        '#weight' => -5,
      );
    }
  }

  /**
   * Called to implement a relationship in a query.
   *
   * As we don't add any data to the query itself, we don't have to do anything
   * here. Views just don't thinks we have been called unless we set our
   * $alias property. Otherwise, this override is just here to keep PHP from
   * blowing up by calling inexistent methods on the query plugin.
   */
  public function query() {
    $this->alias = $this->options['id'];
  }

}
