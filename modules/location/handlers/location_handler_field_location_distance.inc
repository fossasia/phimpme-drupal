<?php

/**
 * @file
 * Coordinates field handler.
 */

class location_handler_field_location_distance extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['origin'] = array('default' => 'user');
    $options['units'] = array('default' => 'km');
    $options['latitude'] = array('default' => '');
    $options['longitude'] = array('default' => '');
    $options['postal_code'] = array('default' => '');
    $options['country'] = array('default' => '');
    $options['php_code'] = array('default' => '');
    $options['nid_arg'] = array('default' => '');
    $options['nid_loc_field'] = array('default' => 'node');
    $options['uid_arg'] = array('default' => '');
    return $options;
  }

  function has_extra_options() {
    return TRUE;
  }

  function extra_options_form(&$form, &$form_state) {
    $form['units'] = array(
      '#type' => 'radios',
      '#title' => t('Units'),
      '#options' => array(
        'km' => t('Kilometers'),
        'mi' => t('Miles'),
      ),
      '#default_value' => $this->options['units'],
    );
    $form['origin'] = array(
      '#type' => 'select',
      '#title' => t('Origin'),
      '#options' => array(
        'user' => t("User's Latitude / Longitude (blank if unset)"),
        'hybrid' => t("User's Latitude / Longitude (fall back to static if unset)"),
        'static' => t('Static  Latitude / Longitude'),
        'tied' => t("Use Distance / Proximity filter"),
        'postal' => t('Postal Code / Country'),
        'postal_default' => t('Postal Code (assume default country)'),
        'php' => t('Use PHP code to determine latitude/longitude'),
        'nid_arg' => t("Node's Latitude / Longitude from views nid argument"),
        'uid_arg' => t("User's Latitude / Longitude from views uid argument"),
        'distance_arg' => t("Latitude / Longitude from views argument"),
      ),
      '#description' => t("This will be the way the latitude/longitude of origin is determined. When using the user's latitude / longitude, if a user has multiple locations the first will be used."),
      '#default_value' => $this->options['origin'],
    );

    $form['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#default_value' => $this->options['latitude'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('hybrid', 'static')),
    );
    $form['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#default_value' => $this->options['longitude'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('hybrid', 'static')),
    );

    $form['postal_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal code'),
      '#default_value' => $this->options['postal_code'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('postal', 'postal_default')),
      '#maxlength' => 16
    );

    $form['country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => array('' => '') + location_get_iso3166_list(),
      '#default_value' => $this->options['country'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('postal')),
    );

    $form['php_code'] = array(
      '#type' => 'textarea',
      '#title' => t('PHP code for latitude, longitude'),
      '#default_value' => $this->options['php_code'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('php')),
      '#description' => t("Enter PHP code that returns a latitude/longitude.  Do not use &lt;?php ?&gt;. You must return only an array with float values set for the 'latitude' and 'longitude' keys."),
    );

    list($nid_argument_options, $uid_argument_options) = location_views_proximity_get_argument_options($this->view);
    $nid_loc_field_options = location_views_proximity_get_location_field_options();

    $form['nid_arg'] = array(
      '#type' => 'select',
      '#title' => t('Node ID argument to use'),
      '#options' => $nid_argument_options,
      '#default_value' => $this->options['nid_arg'],
      '#description' => empty($nid_argument_options) ? t("Select which of the view's arguments to use as the node ID.  The latitude / longitude of the first location of that node will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that node ID. You must have added arguments to the view to use this option.") : t("Select which of the view's arguments to use as the node ID.  The latitude / longitude of the first location of that node will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that node ID."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('nid_arg')),
    );
    $form['nid_loc_field'] = array(
      '#type' => 'select',
      '#title' => t('Location to use'),
      '#options' => $nid_loc_field_options,
      '#default_value' => $this->options['nid_loc_field'],
      '#description' => t("Select which of the node's locations to use as the origin.  Either the node locations or a CCK location field.  If the location supports multiple entries the first one will be used."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('nid_arg')),
    );
    $form['uid_arg'] = array(
      '#type' => 'select',
      '#title' => t('User ID argument to use'),
      '#options' => $uid_argument_options,
      '#default_value' => $this->options['uid_arg'],
      '#description' => empty($uid_argument_options) ? t("Select which of the view's arguments to use as the user ID.  The latitude / longitude of the first location of that user will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that user ID. You must have added arguments to the view to use this option.") : t("Select which of the view's arguments to use as the user ID.  The latitude / longitude of the first location of that user will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that user ID."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('uid_arg')),
    );
  }

  function click_sort($order) {
    $location = location_views_proximity_get_reference_location($this->view, $this->options);

    if ($location) {
      $this->query->add_orderby(NULL, earth_distance_sql($location['longitude'], $location['latitude'], $this->table_alias), $order, $this->field_alias);
    }
  }

  function render($values) {
    if (empty($values->{$this->field_alias}) || $values->{$this->field_alias} == 'Unknown') {
      // Unset location, empty display.
      return;
    }
    $dist = (float)$values->{$this->field_alias};
    if ($this->options['units'] == 'km') {
      $dist = $dist / 1000.0;
      return theme('location_distance', array('distance' => $dist, 'units' => 'km'));
    }
    else {
      $dist = $dist / 1609.347;
      return theme('location_distance', array('distance' => $dist, 'units' => 'mi'));
    }
  }

  function query() {
    $this->ensure_my_table();

    $coordinates = location_views_proximity_get_reference_location($this->view, $this->options);

    if (!empty($coordinates)) {
      $this->field_alias = $this->query->add_field(NULL, earth_distance_sql($coordinates['longitude'], $coordinates['latitude'], $this->table_alias), $this->table_alias .'_'. $this->field);
    }
    else {
      $this->field_alias = $this->query->add_field(NULL, "'Unknown'", $this->table_alias .'_'. $this->field);
    }
  }
}
