<?php

/**
 * @file
 *   Lists all terms associated with a status.
 */

/**
 * Lists all terms associated with a status.
 */
class statuses_tags_views_handler_field_name extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['statuses_tags_link'] = array(
      'default' => 1,
      'translatable' => FALSE,
    );
    $options['statuses_tags_show_symbol'] = array(
      'default' => 1,
      'translatable' => FALSE,
    );
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = $this->options;
    $form['statuses_tags_link'] = array(
      '#title' => t('Show tags as links'),
      '#type' => 'checkbox',
      '#default_value' => $options['statuses_tags_link'],
    );
    $form['statuses_tags_show_symbol'] = array(
      '#title' => t('Show identifying symbol (# or @) in front of tag'),
      '#type' => 'checkbox',
      '#default_value' => $options['statuses_tags_show_symbol'],
    );
  }
  function construct() {
    parent::construct();
    $this->additional_fields['rid'] = 'rid';
    $this->additional_fields['type'] = 'type';
  }
  function render($values) {
    $options = $this->options;
    $tag = new stdClass();
    $tag->name = $values->{$this->field_alias};
    $tag->tid = $values->{$this->aliases['rid']};
    $tag->type = $values->{$this->aliases['type']};

    $prefix = '';
    if ($options['statuses_tags_show_symbol']) {
      if ($tag->type == 'term') {
        $prefix = '#';
      }
      elseif ($tag->type == 'user') {
        $prefix = '@';
      }
    }
    if ($options['statuses_tags_link']) {
      if ($tag->type == 'term') {
        $tags[] = $prefix . l($tag->name, _statuses_tags_resolve($tag));
      }
      elseif ($tag->type == 'user') {
        $tags[] = $prefix . theme('username', array('account' => _statuses_user_load($tag->tid)));
      }
      else {
        $tags[] = check_plain($tag->name);
      }
    }
    else {
      $tags[] = $prefix . check_plain($tag->name);
    }
    return implode(', ', $tags);
  }
}
