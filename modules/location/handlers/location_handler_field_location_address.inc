<?php

/**
 * @file
 * Field handler to display a complete address.
 */

class location_handler_field_location_address extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['hide'] = array('default' => array());
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['hide'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Hide fields from display'),
      '#options' => location_field_names(TRUE),
      '#default_value' => $this->options['hide'],
    );
  }

  function render($values) {
    if ($values->{$this->field_alias}) {
      $location = location_load_location($values->{$this->field_alias});
      if ($location['lid']) {
        return theme('location', array('location' => $location, 'hide' => $this->options['hide']));
      }
    }
  }
}
