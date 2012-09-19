<?php

/**
 * @file
 *   Provides a "comment count" view field handler.
 */

/**
 * Implements the "comment count" field.
 */
class fbss_comments_views_handler_field_cc extends views_handler_field {
  function render($values) {
    $sid = $values->{$this->field_alias};
    $count = db_query("SELECT COUNT(cid) FROM {fbss_comments} WHERE sid = :sid", array(':sid' => $sid))->fetchField();
    return $count;
  }
}
