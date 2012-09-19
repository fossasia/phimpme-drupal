<?php

/**
 * @file
 *   Lists all terms associated with a status.
 */

/**
 * Lists all terms associated with a status.
 */
class statuses_tags_views_handler_field_all_terms extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['statuses_tags_type'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );
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
    $opt = array('user' => t('Users'));
    if (module_exists('taxonomy')) {
      $opt['term'] = t('Terms');
    }
    $options = $this->options;
    $form['statuses_tags_type'] = array(
      '#title' => t('Tag types'),
      '#type' => 'checkboxes',
      '#description' => t('Which tag types should be shown in this field?'),
      '#required' => TRUE,
      '#options' => $opt,
      '#default_value' => $options['statuses_tags_type'],
    );
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
  function render($values) {
    $options = $this->options;
    $sid = $values->{$this->field_alias};
    $query = "SELECT rid tid, name, type FROM {statuses_tags} WHERE sid = %d";
    if (count($options['statuses_tags_type']) > 1) {
      $query .= " AND (type = '";
      $types = array();
      foreach ($options['statuses_tags_type'] as $type => $val) {
        if ($val) {
          $types[] = $type;
        }
      }
      $query .= implode("' OR type = '", $types) . "')";
    }
    $query .= " ORDER BY type ASC, name ASC";
    $result = db_query($query, $sid);
    $tags = array();
    while ($tag = db_fetch_object($result)) {
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
    }
    return implode(', ', $tags);
  }
}
