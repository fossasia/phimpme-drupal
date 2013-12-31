<?php

/**
 * @file
 * Latitude field handler.
 */

class location_views_handler_field_latitude extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['style'] = array('default' => 'dms');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['style'] = array(
      '#title' => t('Display style'),
      '#type' => 'select',
      '#options' => array(
        'dd' => t('Decimal degrees'),
        'dms' => t('Degrees, minutes, seconds'),
      ),
      '#default_value' => $this->options['style'],
    );
  }

  function render($values) {
    if ($this->options['style'] == 'dd') {
      return check_plain($values->{$this->field_alias});
    }
    else {
      return theme('location_latitude_dms', array('latitude' => $values->{$this->field_alias}));
    }
  }
}
