<?php

/**
 * Filter on province.
 */
class location_handler_filter_location_province extends views_handler_filter {

  var $location_country = FALSE;
  var $location_country_identifier = FALSE;

  function option_definition() {
    $options = parent::option_definition();
    $options['operator'] = array('default' => 'is');
    return $options;
  }

  function admin_summary() {
    return '';
//    $options = $this->operator_options('short');
//    return (!empty($this->options['exposed']) ? t('exposed') : $options[$this->operator]);
  }

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    $country = $this->grovel_country();

    drupal_add_js(drupal_get_path('module', 'location') .'/location_autocomplete.js');

    $ac = $country;
    if (is_array($ac)) {
      $ac = implode(',', $ac);
    }
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('State/Province'),
      '#autocomplete_path' => 'location/autocomplete/'. $ac,
      '#default_value' => $this->value,
      '#size' => 64,
      '#maxlength' => 64,
      // Used by province autocompletion js.
      '#attributes' => array('class' => array('location_auto_province')),
      '#multiple' => TRUE, //$this->options['multiple'],
    );

    // Let location_autocomplete.js find the correct fields to attach.
    if ($this->location_country_identifier) {
      $form['value']['#attributes']['class'][] = 'location_auto_join_' . $this->location_country_identifier;
    }
  }

  function operator_options() {
    if ($this->options['expose']['single']) {
      return array(
        'is' => t('Is'),
        'is not' => t('Is not'),
      );
    }
    else {
      return array(
        'is' => t('Is one of'),
        'is not' => t('Is not one of'),
      );
    }
  }

  function grovel_country() {
    $country = variable_get('location_default_country', 'us');
    if (!empty($this->view->filter))
    foreach ($this->view->filter as $k => $v) {
      if ($v->table == 'location' && $v->field == 'country' && $v->options['relationship'] == $this->options['relationship']) {
        $country = $v->value;
        if (!empty($v->options['expose']['identifier'])) {
          if (isset($this->view->exposed_input[$v->options['expose']['identifier']])) {
            $country = $this->view->exposed_input[$v->options['expose']['identifier']];
          }
          $this->location_country_identifier = $v->options['expose']['identifier'];
        }
      }
    }
    if ($country == '' || $country == 'All' || $country == '  ' || $country == 'xx') {
      // It's set to something nonsensical, reset to the default to prevent malfunctions.
      $country = variable_get('location_default_country', 'us');
    }
    $this->location_country = $country;
    return $country;
  }

  function query() {
    // Normalize values.
    $value = $this->value;
    if (is_array($value)) {
      // At one point during development, provinces was a select box.
      // Right now it's an autocomplete textfield.
      // @@@ Investigate correct fix sometime.
      //$value = array_keys($value);
      if (count($value) == 1) {
        // If multiple is allowed but only one was chosen, use a string instead.
        $value = reset($value);
      }
    }
    if (empty($value)) {
      return;
    }

    $country = $this->grovel_country();

    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";


    if (is_array($value)) {
      // Multiple values
      foreach ($value as $k => $v) {
        // Convert to province codes.
        $value[$k] = location_province_code($country, $v);
      }
      $operator = ($this->operator == 'is') ? 'IN' : 'NOT IN';
      $this->query->add_where($this->options['group'], $field, $value, $operator);
    }
    else {
      // Single value
      // Convert to province code.
      $value = location_province_code($country, $value);
      $operator = ($this->operator == 'is') ? '=' : '!=';
      $this->query->add_where($this->options['group'], $field, $value, $operator);
    }
  }
}
