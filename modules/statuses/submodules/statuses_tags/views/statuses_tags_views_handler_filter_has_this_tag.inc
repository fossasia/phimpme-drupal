<?php

/**
 * @file
 *   Filters to statuses containing a specific tag.
 */

/**
 * Filter handler to select statuses containing a specific tag.
 */
class statuses_tags_views_handler_filter_has_this_tag extends views_handler_filter_string {
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
      '#value' => t('Warning: this filter can be slow.'),
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
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);
    if (variable_get('statuses_tags_vid', -1) != -1) {
      $form['value']['#autocomplete_path'] = 'taxonomy/autocomplete/' . variable_get('statuses_tags_vid', -1);
    }
  }
  function query() {
    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";
    $upper = $this->case_transform();
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
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field, $upper, $where);
    }
  }
  function op_equal($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) $this->operator $upper('%s') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_contains($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) LIKE $upper('%%%s%%') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_word($field, $upper, $extra_where) {
    $where = array();
    preg_match_all('/ (-?)("[^"]+"|[^" ]+)/i', ' ' . $this->value, $matches, PREG_SET_ORDER);
    foreach ($matches as $match) {
      $phrase = FALSE;
      if ($match[2]{0} == '"') {
        $match[2] = drupal_substr($match[2], 1, -1);
        $phrase = TRUE;
      }
      $words = trim($match[2], ',?!();:-');
      $words = $phrase ? array($words) : preg_split('/ /', $words, -1, PREG_SPLIT_NO_EMPTY);
      foreach ($words as $word) {
        $where[] = "$upper(name) LIKE $upper('%%%s%%')";
        $values[] = trim($word, " ,!?");
      }
    }
    if (!$where) {
      return;
    }
    if ($this->operator == 'word') {
      $where = '(' . implode(' OR ', $where) . ')';
    }
    else {
      $where = implode(' AND ', $where);
    }
    $where .= " $extra_where";
    $this->query->add_where($this->options['group'], db_prefix_tables("$field IN (SELECT sid FROM {statuses_tags} WHERE $where)"), $values);
  }

  function op_starts($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) LIKE $upper('%s%%') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_not_starts($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) NOT LIKE $upper('%s%%') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_ends($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) LIKE $upper('%%%s') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_not_ends($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) NOT LIKE $upper('%%%s') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }

  function op_not($field, $upper, $where) {
    $query = "$field IN (SELECT sid FROM {statuses_tags} WHERE $upper(name) NOT LIKE $upper('%%%s%%') $where)";
    $this->query->add_where($this->options['group'], db_prefix_tables($query), $this->value);
  }
}
