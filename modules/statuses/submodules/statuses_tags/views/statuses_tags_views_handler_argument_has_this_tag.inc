<?php

/**
 * @file
 *   Allow only statuses with a specific tag.
 */

/**
 * Argument handler to select statuses containing a specific tag.
 */
class statuses_tags_views_handler_argument_has_this_tag extends views_handler_argument_string {
  function option_definition() {
    $options = parent::option_definition();
    $options['statuses_tags_type'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );
    return $options;
  }
  function options_form(&$form, &$form_state) {
    $options = parent::options_form($form, $form_state);
    $opt = array('user' => t('Users'));
    if (module_exists('taxonomy')) {
      $opt['term'] = t('Terms');
    }
    $options = $this->options;
    $form['warning'] = array(
      '#value' => t('Warning: this argument can be slow.'),
      '#weight' => -100,
    );
    $form['statuses_tags_type'] = array(
      '#title' => t('Tag types'),
      '#type' => 'checkboxes',
      '#required' => TRUE,
      '#options' => $opt,
      '#default_value' => $options['statuses_tags_type'],
    );
  }
  function query() {
    $argument = $this->argument;
    if (!empty($this->options['transform_dash'])) {
      $argument = strtr($argument, '-', ' ');
    }
    if (!empty($this->definition['many to one'])) {
      if (!empty($this->options['glossary'])) {
        $this->helper->formula = TRUE;
      }
      $this->value = array($argument);
      $this->helper->ensure_my_table();
      $this->helper->add_filter();
      return;
    }
    $this->ensure_my_table();
    if (empty($this->options['glossary'])) {
      $field = "$this->table_alias.$this->real_field";
    }
    else {
      $field = $this->get_formula();
    }
    $where = '';
    $options = $this->options['statuses_tags_type'];
    if (count($options) > 0) {
      $where .= " AND (type = '";
      $types = array();
      foreach ($options as $type => $val) {
        if ($val) {
          $types[] = $type;
        }
      }
      $where .= implode("' OR type = '", $types) . "')";
    }
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE name = '%s' $where)";
    $this->query->add_where(0, db_prefix_tables($query), $argument);
  }
  function default_actions($which = NULL) {
    $defaults = parent::default_actions($which);
    unset($defaults['summary asc']);
    unset($defaults['summary desc']);
    return $defaults;
  }
}
