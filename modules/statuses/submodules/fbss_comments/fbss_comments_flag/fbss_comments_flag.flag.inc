<?php

/**
 @file
 *   Extends the Flag module for Statuses Comments.
 */

/**
 * Make sure the flag_flag class is loaded before we extend it.
 * It won't be loaded yet if the Flag module hasn't loaded yet, because Flag
 * doesn't provide the kind of fancy autoloading that Views does.
 */
if (!class_exists('flag_flag')) {
  module_load_include('inc', 'flag');
}

/**
 * Adds a new flag type.
 */
class fbss_comments_flag extends flag_flag {
  function _load_content($content_id) {
    return is_numeric($content_id) ? fbss_comments_load($content_id) : NULL;
  }
  function applies_to_content_object($comment) {
    return !empty($comment) && !empty($comment->comment);
  }
  function get_content_id($comment) {
    return $comment->cid;
  }
  function get_labels_token_types() {
    return array('fbss_comment');
  }
  function get_views_info() {
    return array(
      'views table' => 'fbss_comments',
      'join field' => 'cid',
      'title field' => 'comment',
      'title' => t('Statuses Comments'),
      'help' => t('Display information about the flag set on a status comment.'),
      'counter title' => t('Statuses Comments flag counter'),
      'counter help' => t('Include this to gain access to the flag counter field.'),
    );
  }
  function applies_to_content_id_array($content_ids) {
    $passed = array();
    foreach ($content_ids as $content_id) {
      $passed[$content_id] = TRUE;
    }
    return $passed;
  }
  function get_relevant_action_objects($content_id) {
    return array(
      'fbss_comment' => $this->fetch_content($content_id),
    );
  }
  function replace_tokens($label, $contexts, $content_id) {
    if ($content_id && ($comment = $this->fetch_content($content_id))) {
      $contexts['fbss_comment'] = $comment;
    }
    return parent::replace_tokens($label, $contexts, $content_id);
  }
  function get_flag_action($content_id) {
    $flag_action = parent::get_flag_action($content_id);
    $comment = $this->fetch_content($content_id);
    $flag_action->content_title = $comment->comment;
    $flag_action->content_url = 'statuses/' . $comment->sid;
    return $flag_action;
  }
  function rules_get_event_arguments_definition() {
    return array(
      'account' => array(
        'type' => 'fbss_comment',
        'label' => t('Flagged status comment'),
        'handler' => 'flag_rules_get_event_argument',
      ),
    );
  }
  function rules_get_element_argument_definition() {
    return array('type' => 'fbss_comment', 'label' => t('Flagged status comment'));
  }
}
