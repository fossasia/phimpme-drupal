<?php

/**
 * General proximity filter for location latitude/longitude.
 */
class location_views_handler_filter_proximity extends views_handler_filter {
  // This is always multiple, because we can have distance, units etc.
  var $always_multiple = TRUE;

  function option_definition() {
    $options = parent::option_definition();
    $options['operator'] = array('default' => 'mbr');

    $options['identifier'] = array('default' => 'dist');

    $options['origin'] = array('default' => 'user');

    $options['value'] = array(
      'default' => array(
        'latitude' => '',
        'longitude' => '',
        'postal_code' => '',
        'country' => '',
        'php_code' => '',
        'nid_arg' => '',
        'nid_loc_field' => 'node',
        'uid_arg' => '',
        'search_distance' => 100,
        'search_units' => 'mile',
      ),
    );

    $options['expose']['contains']['gmap_macro'] = array('default' => '[gmap ]');
    $options['expose']['contains']['user_location_choose'] = array('default' => FALSE);

    return $options;
  }

  function admin_summary() {
    return '';
  }

  function operator_options() {
    return array(
      'mbr' => t('Proximity (Rectangular)'),
      'dist' => t('Proximity (Circular)'),
    );
  }
  
  function expose_options() {
    parent::expose_options();
    $this->options['expose']['gmap_macro'] = array('default' => '[gmap ]');
    $this->options['expose']['user_location_choose'] = array('default' => FALSE);
  }

  function expose_form_left(&$form, &$form_state) {
    parent::expose_form_left($form, $form_state);
    // @@@ Todo: autohide this.
    $form['expose']['gmap_macro'] = array(
      '#parents' => array('options', 'gmap_macro'),
      '#type' => 'textfield',
      '#title' => t('GMap macro'),
      '#description' => t('The macro to use for the Latitude / Longitude map, if applicable.'),
      '#default_value' => $this->options['expose']['gmap_macro'],
    );

    $form['expose']['user_location_choose'] = array(
      '#type' => 'checkbox',
      '#title' => t("Allow choice of user location"),
      '#default_value' => $this->options['expose']['user_location_choose'],
      '#description' => t("If checked and using a user location origin, the user will be able to choose which of their locations to use.  Otherwise their first location will be used."),
    );
  }

  function value_form(&$form, &$form_state) {
    $form['origin'] = array(
      '#type' => 'select',
      '#title' => t('Origin'),
      '#options' => array(
        'user' => t("User's Latitude / Longitude (blank if unset)"),
        'hybrid' => t("User's Latitude / Longitude (fall back to static if unset)"),
        'static' => t('Static  Latitude / Longitude'),
        'postal' => t('Postal Code / Country'),
        'postal_default' => t('Postal Code (assume default country)'),
        'php' => t('Use PHP code to determine latitude/longitude'),
        'nid_arg' => t("Node's Latitude / Longitude from views nid argument"),
        'uid_arg' => t("User's Latitude / Longitude from views uid argument"),
      ),
      '#description' => t('This will be the way the latitude/longitude of origin is determined.  If this filter is exposed, this will determine the default values. NOTE: The PHP code, nid argument and uid argument options will not be available when the filter is exposed and the use of map is only available when the filter is exposed.'),
      '#default_value' => $this->options['origin'] ? $this->options['origin'] : 'user',
    );
    if (module_exists('gmap')) {
      $form['origin']['#options']['latlon_gmap'] = t('Latitude / Longitude input (use map)');
    }

    // [11:44] <merlinofchaos> If you load the page from scratch, $input for your identifier will be empty.
    // [11:44] <merlinofchaos> So what's going on here is that for $_GET forms, there's no form id, no op button or anything, so they always appear to submit.
    // [11:45] <merlinofchaos> FAPI doesn't quite get along with that; sometimes it handles the input being empty right and sometimes it doesn't.
    // [11:45] <Bdragon> But if I set #default_value to a static string, it doesn't work either
    // [11:45] <merlinofchaos> Right, fapi thinks the empty input is actually input, thus it overrides the default value.
    // [11:45] <Bdragon> Ahh, hmm...
    // [11:46] <Bdragon> So where would I go to clean it up?
    // [11:55] <merlinofchaos> Bdragon: See views_handler_filter_string.inc line 174
    // [11:55] <merlinofchaos> Bdragon: That's how i address this problem.
    // [11:58] <Bdragon> Hmm, OK
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!isset($form_state['input'][$identifier])) {
        // We need to pretend the user already inputted the defaults, because
        // fapi will malfunction otherwise.
        $form_state['input'][$identifier] = $this->value;
      }
    }

    $form['value'] = array(
      '#tree' => TRUE,
    );

    $form['value']['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#default_value' => $this->value['latitude'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('hybrid', 'static', 'latlon_gmap')),
      '#weight' => 1,
    );
    $form['value']['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#default_value' => $this->value['longitude'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('hybrid', 'static', 'latlon_gmap')),
      '#weight' => 2,
    );

    $form['value']['postal_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal code'),
      '#default_value' => $this->value['postal_code'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('postal', 'postal_default')),
      '#weight' => 3,
      '#maxlength' => 16
    );

    $form['value']['country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => array('' => '') + location_get_iso3166_list(),
      '#default_value' => $this->value['country'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('postal')),
      '#weight' => 4,
    );

    $form['value']['php_code'] = array(
      '#type' => 'textarea',
      '#title' => t('PHP code for latitude, longitude'),
      '#default_value' => $this->value['php_code'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('php')),
      '#description' => t("Enter PHP code that returns a latitude/longitude.  Do not use &lt;?php ?&gt;. You must return only an array with float values set for the 'latitude' and 'longitude' keys."),
      '#weight' => 5,
    );

    list($nid_argument_options, $uid_argument_options) = location_views_proximity_get_argument_options($this->view);
    $nid_loc_field_options = location_views_proximity_get_location_field_options();

    $form['value']['nid_arg'] = array(
      '#type' => 'select',
      '#title' => t('Node ID argument to use'),
      '#options' => $nid_argument_options,
      '#default_value' => $this->value['nid_arg'],
      '#description' => empty($nid_argument_options) ? t("Select which of the view's arguments to use as the node ID.  The latitude / longitude of the first location of that node will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that node ID. You must have added arguments to the view to use this option.") : t("Select which of the view's arguments to use as the node ID.  The latitude / longitude of the first location of that node will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that node ID."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('nid_arg')),
      '#weight' => 6,
    );
    $form['value']['nid_loc_field'] = array(
      '#type' => 'select',
      '#title' => t('Location to use'),
      '#options' => $nid_loc_field_options,
      '#default_value' => $this->value['nid_loc_field'],
      '#description' => t("Select which of the node's locations to use as the origin.  Either the node locations or a CCK location field.  If the location supports multiple entries the first one will be used."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('nid_arg')),
      '#weight' => 7,
    );
    $form['value']['uid_arg'] = array(
      '#type' => 'select',
      '#title' => t('User ID argument to use'),
      '#options' => $uid_argument_options,
      '#default_value' => $this->value['uid_arg'],
      '#description' => empty($uid_argument_options) ? t("Select which of the view's arguments to use as the user ID.  The latitude / longitude of the first location of that user will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that user ID. You must have added arguments to the view to use this option.") : t("Select which of the view's arguments to use as the user ID.  The latitude / longitude of the first location of that user will be used as the origin. Use the 'Global: Null' argument if you don't want to also restrict results to that user ID."),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('edit-options-origin' => array('uid_arg')),
      '#weight' => 8,
    );

    $form['value']['search_distance'] = array(
      '#type' => 'textfield',
      '#title' => t('Distance'),
      '#default_value' => $this->value['search_distance'],
      '#weight' => 9,
    );

    $form['value']['search_units'] = array(
      '#type' => 'select',
      '#options' => array(
        'mile' => t('Miles'),
        'km' => t('Kilometers'),
      ),
      '#default_value' => $this->value['search_units'],
      '#weight' => 10,
    );
  }

  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);
    $key = $this->options['expose']['identifier'];
    $origin = $this->options['origin'];

    // Strip dependencies off on exposed form.
    foreach (element_children($form[$key]) as $el) {
      if (!empty($form[$key][$el]['#dependency'])) {
        $form[$key][$el]['#dependency'] = array();
      }
    }

    if ($origin == 'latlon_gmap' && module_exists('gmap')) {
      // Bad things happen if we try to show a gmap in the views live preview.
      if (!empty($form_state['view']->live_preview)) {
        $form[$key]['proximity_map'] = array(
          '#markup' => t('Gmap location selection is not available during live preview.'),
          '#weight' => 0,
        );
      }
      else {
        $form[$key]['proximity_map'] = array(
          '#markup' => gmap_set_location($this->options['expose']['gmap_macro'], $form[$key], array('latitude' => 'latitude', 'longitude' => 'longitude')),
          '#weight' => 0,
        );
      }
    }
    else if (($origin == 'user' || $origin == 'hybrid') && $this->options['expose']['user_location_choose']) {
      global $user;
      $user_locations = isset($user->locations) ? $user->locations : location_load_locations($user->uid, 'uid');
      $location_options = array();
      if (!empty($user_locations)) {
        foreach ($user_locations as $i => $location) {
          if (isset($location['latitude']) && isset($location['longitude'])) {
            if (!empty($location['name'])) {
              $location_options[$i] = t(check_plain($location['name']));
            }
            else {
              $location_options[$i] = t('Location #@num', array('@num' => $i + 1));
            }
          }
        }
      }

      $form[$key]['user_location_delta'] = array(
        '#type' => 'select',
        '#title' => t('Location'),
        '#options' => $location_options,
        '#description' => t('Select which of your locations to use.'),
        '#weight' => 0,
      );
    }

    // Remove unneeded fields when exposing the form.
    // It's shorter than redefining value_form.
    if ($origin != 'static' && $origin != 'latlon_gmap') {
      unset($form[$key]['latitude']);
      unset($form[$key]['longitude']);
    }
    if ($origin != 'postal' && $origin != 'postal_default') {
      unset($form[$key]['postal_code']);
    }
    if ($origin != 'postal') {
      unset($form[$key]['country']);
    }

    // And we definitely do not want to expose the php code option when exposing the filter
    unset($form[$key]['php_code']);
    // The nid/uid arg options are not useful on an exposed form.
    unset($form[$key]['nid_arg']);
    unset($form[$key]['nid_loc_field']);
    unset($form[$key]['uid_arg']);

    unset($form['origin']);
  }

  function query() {
    if (empty($this->value)) {
      return;
    }

    // We need to merge with $this->options['value'] for filter values
    // and $this->value for exposed filter values.
    $options = array_merge($this->options, $this->options['value'], $this->value);
    $coordinates = location_views_proximity_get_reference_location($this->view, $options);

    // If we don't have any coordinates or distance, there's nothing to filter
    // by, so don't modify the query at all.
    if (empty($coordinates) || empty($this->value['search_distance'])) {
      return;
    }

    $this->ensure_my_table();

    $lat = $coordinates['latitude'];
    $lon = $coordinates['longitude'];

    $distance_meters = _location_convert_distance_to_meters($this->value['search_distance'], $this->value['search_units']);
    $latrange = earth_latitude_range($lon, $lat, $distance_meters);
    $lonrange = earth_longitude_range($lon, $lat, $distance_meters);

    // Add MBR check (always).
    // In case we go past the 180/-180 mark for longitude.
    if ($lonrange[0] > $lonrange[1]) {
      $where = "$this->table_alias.latitude > :minlat AND $this->table_alias.latitude < :maxlat AND (($this->table_alias.longitude < 180 AND $this->table_alias.longitude > :minlon) OR ($this->table_alias.longitude < :maxlon AND $this->table_alias.longitude > -180))";
    }
    else {
      $where = "$this->table_alias.latitude > :minlat AND $this->table_alias.latitude < :maxlat AND $this->table_alias.longitude > :minlon AND $this->table_alias.longitude < :maxlon";
    }
    $this->query->add_where_expression($this->options['group'], $where, array(':minlat' => $latrange[0], ':maxlat' => $latrange[1], ':minlon' => $lonrange[0], ':maxlon' => $lonrange[1]));

    if ($this->operator == 'dist') {
      // Add radius check.
      $this->query->add_where_expression($this->options['group'], earth_distance_sql($lon, $lat, $this->table_alias) .' < :distance', array(':distance' => $distance_meters));
    }
  }
}
