<?php

/**
 * @file
 * Province field handler.
 */

class location_handler_field_location_street extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields = array(
      'additional' => 'additional',
    );
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['style'] = array('default' => 'both');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['style'] = array(
      '#title' => t('Display style'),
      '#type' => 'select',
      '#options' => array(
        'both' => t('Both street and additional'),
        'street' => t('Street only'),
        'additional' => t('Additional only'),
      ),
      '#default_value' => $this->options['style'],
    );
  }

  function render($values) {
    $parts = array();
    if ($this->options['style'] != 'additional') {
      $parts[] = check_plain($values->{$this->field_alias});
    }
    if ($this->options['style'] != 'street') {
      $additional = trim($values->{$this->aliases['additional']});
      if (!empty($additional)) {
        $parts[] = check_plain($values->{$this->aliases['additional']});
      }
    }
    // @@@ Better theming?
    return implode('<br />', $parts);
  }
}
