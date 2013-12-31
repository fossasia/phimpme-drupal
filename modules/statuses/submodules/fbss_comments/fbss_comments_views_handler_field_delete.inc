<?php

/**
 * @file
 *   Provides a delete link to Views.
 */

/**
 * Field handler to add a field with a delete link.
 */
class fbss_comments_views_handler_field_delete extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['sid'] = 'sid';
    $this->additional_fields['uid'] = 'uid';
    $this->additional_fields['created'] = 'created';
    $this->additional_fields['comment'] = 'comment';
  }
  function render($values) {
    $comment = new stdClass();
    $comment->cid = $values->{$this->field_alias};
    $comment->sid = $values->{$this->aliases['sid']};
    $comment->uid = $values->{$this->aliases['uid']};
    $comment->created = $values->{$this->aliases['created']};
    $comment->comment = $values->{$this->aliases['comment']};
    if (fbss_comments_can('delete', $comment)) {
      if (module_exists('modalframe')) {
        modalframe_parent_js();
      }
      drupal_add_css(drupal_get_path('module', 'fbss_comments') . '/fbss_comments.css');
      return '<span class="statuses-delete">' . l(t('Delete'), 'statuses/comment/' . $comment->cid . '/delete', array('query' => array('destination' => $_GET['q']))) . '</span>';
    }
  }
}
