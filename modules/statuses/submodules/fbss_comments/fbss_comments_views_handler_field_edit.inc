<?php

/**
 * @file
 *   Provides an edit link to Views.
 */

/**
 * Field handler to add a field with an edit link.
 */
class fbss_comments_views_handler_field_edit extends views_handler_field {
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
    if (fbss_comments_can('edit', $comment)) {
      if (module_exists('modalframe')) {
        modalframe_parent_js();
      }
      drupal_add_css(drupal_get_path('module', 'fbss_comments') . '/fbss_comments.css');
      return '<span class="statuses-edit">' . l(t('Edit'), 'statuses/comment/' . $comment->cid . '/edit', array('query' => array('destination' => $_GET['q']))) . '</span>';
    }
  }
}
