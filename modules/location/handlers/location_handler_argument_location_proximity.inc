<?php

/**
 * @file
 * Location proximity argument handler.
 */

/**
 * Argument handler to accept proximity
 */
class location_handler_argument_location_proximity extends views_handler_argument {
  function option_definition() {
    $options = parent::option_definition();
    // As only us and uk use miles, make km the default otherwise.
    $country = variable_get('location_default_country', 'us');
    $options['search_units'] = array('default' => ($country == 'us' || $country == 'uk' ? 'mile' : 'km'));
    $options['search_method'] = array('default' => 'mbr');
    $options['type'] = array('default' => 'postal');
    return $options;
  }

  /**
   * Add a form elements to select options for this argument.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['type'] = array(
      '#title' => t('Coordinate Type'),
      '#type' => 'select',
      '#options' => array(
        'postal' => t('Postal Code (Zipcode)'),
        'latlon' => t('Decimal Latitude and Longitude coordinates, comma delimited'),
      ),
      '#default_value' => $this->options['type'],
      '#description' => t('Type of center point.') . '<br />' . t('Postal code argument format: country_postcode_distance or postcode_distance') . '<br />' . t('Lat/Lon argument format: lat,lon_distance') . '<br />' . t('where distance is either a number or a comma delimited pair of decimal degrees'),
    );

    // Units used.
    $form['search_units'] = array(
      '#type' => 'select',
      '#title' => t('Distance unit'),
      '#options' => array(
        'km' => t('Kilometers'),
        'm' => t('Meters'),
        'mile' => t('Miles'),
        'dd' => t('Decimal degrees'),
      ),
      '#default_value' => $this->options['search_units'],
      '#description' => t('Select the unit of distance. Decimal degrees should be comma delimited.'),
    );

    $form['search_method'] = array(
      '#title' => t('Method'),
      '#type' => 'select',
      '#options' => array(
        'dist' => t('Circular Proximity'),
        'mbr' => t('Rectangular Proximity'),
      ),
      '#default_value' => $this->options['search_method'],
      '#description' => t('Method of determining proximity. Please note that Circular Proximity does not work with Decimal degrees.'),
     );
  }

  function calculate_coords() {
    if (!empty($this->value['latitude']) && !empty($this->value['longitude'])) {
      // If there are already coordinates, there's no work for us.
      return TRUE;
    }
    // @@@ Switch to mock location object and rely on location more?

    if ($this->options['type'] == 'postal') {
      if (!isset($this->value['country'])) {
        $this->value['country'] = variable_get('location_default_country', 'us');
      }
      // Zip code lookup.
      if (!empty($this->value['postal_code']) && !empty($this->value['country'])) {
        location_load_country($this->value['country']);
        $coord = location_get_postalcode_data($this->value);
        if ($coord) {
          $this->value['latitude'] = $coord['lat'];
          $this->value['longitude'] = $coord['lon'];
        }
        else {
          $coord = location_latlon_rough($this->value);
          if ($coord) {
            $this->value['latitude'] = $coord['lat'];
            $this->value['longitude'] = $coord['lon'];
          }
          else {
            return FALSE;
          }
        }
      }
      else {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Set up the query for this argument.
   *
   * The argument sent may be found at $this->argument.
   */
  function query($group_by = FALSE) {
    // Get and process argument.
    if ($this->options['type'] == 'postal') {
      foreach ($this->view->argument as $argument) {
        if ($argument->field == 'distance') {
          $arg_parts = explode('_', $this->view->args[$argument->position]);
          if (count($arg_parts) == 3) {
            $this->value['country'] = drupal_strtolower($arg_parts[0]);
            $this->value['postal_code'] = $arg_parts[1];
            $this->value['search_distance'] = $arg_parts[2];
          }
          else {
            $this->value['postal_code'] = $arg_parts[0];
            $this->value['search_distance'] = $arg_parts[1];
          }
          break;
        }
      }
    }
    else if ($this->options['type'] == 'latlon') {
      foreach ($this->view->argument as $argument) {
        if ($argument->field == 'distance') {
          list($coords, $this->value['search_distance']) = explode('_', $this->view->args[$argument->position]);
          list($this->value['latitude'], $this->value['longitude']) = explode(',', $coords);
          break;
        }
      }
    }

    // Coordinates available?
    if (!$this->calculate_coords()) {
      // Distance set?
      if (!empty($this->value['search_distance'])) {
        // Hmm, distance set but unable to resolve coordinates.
        // Force nothing to match.
        $this->query->add_where(0, "1 = 0");
      }
      return;
    }

    $this->ensure_my_table();

    $lat = $this->value['latitude'];
    $lon = $this->value['longitude'];

    // search_distance
    if ($this->options['search_units'] == 'dd') {
      list($lat_distance, $lon_distance) = explode(',', $this->value['search_distance']);
      $latrange[0] = $lat - $lat_distance;
      $latrange[1] = $lat + $lat_distance;
      $lonrange[0] = $lon - $lon_distance;
      $lonrange[1] = $lon + $lon_distance;
    }
    else {
      $distance = $this->value['search_distance'];
      if ($this->options['search_units'] == 'm') {
        $distance_meters = $distance;
      }
      else {
        $distance_meters = _location_convert_distance_to_meters($distance, $this->options['search_units']);
      }

      $latrange = earth_latitude_range($lon, $lat, $distance_meters);
      $lonrange = earth_longitude_range($lon, $lat, $distance_meters);
    }

    // Add MBR check (always).
    // In case we go past the 180/-180 mark for longitude.
    if ($lonrange[0] > $lonrange[1]) {
      $where = "$this->table_alias.latitude > :minlat AND $this->table_alias.latitude < :maxlat AND (($this->table_alias.longitude < 180 AND $this->table_alias.longitude > :minlon) OR ($this->table_alias.longitude < :maxlon AND $this->table_alias.longitude > -180))";
    }
    else {
      $where = "$this->table_alias.latitude > :minlat AND $this->table_alias.latitude < :maxlat AND $this->table_alias.longitude > :minlon AND $this->table_alias.longitude < :maxlon";
    }
    $this->query->add_where_expression(0, $where, array(':minlat' => $latrange[0], ':maxlat' => $latrange[1], ':minlon' => $lonrange[0], ':maxlon' => $lonrange[1]));

    if ($this->options['search_method'] == 'dist') {
      // Add radius check.
      $this->query->add_where_expression(0, earth_distance_sql($lon, $lat, $this->table_alias) . ' < :distance', array(':distance' => $distance_meters));
    }
  }
}
