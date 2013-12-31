<?php

/**
 * @file
 *   Handler for the Private field.
 */

/**
 * Displays "Private" if the status is private.
 */
class fbss_privacy_views_handler_field extends views_handler_field {
  function render($values) {
    if ($values->{$this->field_alias}) {
      drupal_add_css(drupal_get_path('module', 'fbss_privacy') . '/fbss_privacy.css');
      return '<span class="statuses-private-text">' . t('(Private)') . '</span>';
    }
  }
}
