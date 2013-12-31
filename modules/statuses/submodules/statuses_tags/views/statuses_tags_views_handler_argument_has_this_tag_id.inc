<?php

/**
 * @file
 *   Allow only statuses with a specific tag.
 */

/**
 * Argument handler to select statuses containing a specific tag.
 */
class statuses_tags_views_handler_argument_has_this_tag_id extends views_handler_argument_numeric {
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
    $this->ensure_my_table();
    if (!empty($this->options['break_phrase'])) {
      views_break_phrase($this->argument, $this);
    }
    else {
      $this->value = array($this->argument);
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
    //if ($this->operator == 'or') {
    if (count($this->value) > 2) {
      $operator = empty($this->options['not']) ? 'IN' : 'NOT IN';
      $placeholders = implode(', ', array_fill(0, sizeof($this->value), '%d'));
      $query = "$this->table_alias.$this->real_field IN (SELECT sid FROM {statuses_tags} WHERE rid $operator ($placeholders) $where)";
      $this->query->add_where(0, db_prefix_tables($query), $this->value);
    }
    // If there are exactly 2 users specified, show the exchange between them.
    if (count($this->value) == 2) {
      $operator = empty($this->options['not']) ? 'IN' : 'NOT IN';
      $placeholders = implode(', ', array_fill(0, sizeof($this->value), '%d'));
      $query = "$this->table_alias.$this->real_field IN (SELECT sid FROM {statuses_tags} WHERE rid $operator ($placeholders) $where)";
      $query .= "AND $this->table_alias.pid IN (%d, %d)";
      $this->query->add_where(0, db_prefix_tables($query), array_merge($this->value, $this->value));
    }
    else {
      $operator = empty($this->options['not']) ? '=' : '!=';
      $query = "$this->table_alias.$this->real_field IN (SELECT sid FROM {statuses_tags} WHERE rid $operator %d $where)";
      $this->query->add_where(0, db_prefix_tables($query), $this->argument);
    }
    //}
    /*
     * Default to the "OR" behavior because it's more useful.
     * We can never actually allow either to happen because modern browsers
     * convert "+" to " " so we can't parse for the operator.
     else { //$this->operator == 'and'
     $operator = empty($this->options['not']) ? '=' : '!=';
     if (count($this->value) > 1) {
     //This causes the query to be pretty slow.
     $subquery = array();
     foreach ($this->value as $value) {
     $subquery[] = "$this->table_alias.$this->real_field IN (SELECT sid FROM {statuses_tags} WHERE rid $operator %d $where)";
     }
     $query = '('. implode(' AND ', $subquery) .')';
     $this->query->add_where(0, db_prefix_tables($query), $this->value);
     }
     else {
     $query = "$this->table_alias.$this->real_field IN (SELECT sid FROM {statuses_tags} WHERE rid $operator %d $where)";
     $this->query->add_where(0, db_prefix_tables($query), $this->argument);
     }
     }
     */
  }
  function default_actions($which = NULL) {
    $defaults = parent::default_actions($which);
    unset($defaults['summary asc']);
    unset($defaults['summary desc']);
    return $defaults;
  }
}
