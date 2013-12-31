<?php

/**
 * @file
 * Views 3 support for Location.
 */

/*

TODO:
 * Finish porting!
 * Write "relationships" -- see book.views.inc, upload.views.inc, nodequeue...


*/

/**
 * Implementation of hook_views_handlers().
 */
function location_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'location') .'/handlers',
    ),
    'handlers' => array(
      'location_views_handler_field_latitude' => array(
        'parent' => 'views_handler_field',
      ),
      'location_views_handler_field_longitude' => array(
        'parent' => 'location_views_handler_field_latitude',
      ),
      'location_views_handler_field_coordinates' => array(
        'parent' => 'location_views_handler_field_latitude',
      ),
      'location_handler_field_location_country' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_filter_location_country' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
      'location_handler_argument_location_country' => array(
        'parent' => 'views_handler_argument',
      ),
      'location_handler_field_location_province' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_filter_location_province' => array(
        'parent' => 'views_handler_filter',
      ),
      'location_handler_argument_location_province' => array(
        'parent' => 'views_handler_argument',
      ),
      'location_handler_argument_location_proximity' => array(
        'parent' => 'views_handler_argument',
      ),
      'location_handler_field_location_address' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_field_location_street' => array(
        'parent' => 'views_handler_field',
      ),
      'location_views_handler_filter_proximity' => array(
        'parent' => 'views_handler_filter',
      ),
      'location_handler_field_location_distance' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_sort_location_distance' => array(
        'parent' => 'views_handler_sort',
      ),
//      'location_handler_relationship_location_distance' => array(
//        'parent' => 'views_handler_relationship',
//      ),

//      'location_handler_field_location_coordinates_user' => array(
//        'parent' => 'views_handler_field',
//      ),

    ),
  );
}

/**
 * Implementation of hook_views_data().
 */
function location_views_data() {
  // ----------------------------------------------------------------
  // location table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.

  $data['location']['table']['group'] = t('Location');

  // Advertise this table as a possible base table
  $data['location']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Location'),
    'help' => t('Locations are addresses and map coordinates.'),
    'weight' => -10,
  );

  $data['location']['table']['join'] = array(
    // Location links to node through location_instance via lid.
    'node' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // Location links to node_revision through location_instance via lid.
    'node_revision' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // Location links to users through location_instance via lid.
    'users' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
  );

  // ----------------------------------------------------------------
  // location table -- fields

  // lid
  $data['location']['lid'] = array(
    'title' => t('Lid'),
    'help' => t('The location ID of the location.'), // The help that appears on the UI,
    // Information for displaying the lid
    'field' => array(
      'handler' => 'views_handler_field', // @@@
      'click sortable' => TRUE,
    ),
    // Information for accepting a lid as an argument
    /*
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    */
    // Information for accepting a lid as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'allow empty' => TRUE,
    ),
    // Information for sorting on a lid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['location']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'additional' => 'street', style 'additional'
  $data['location']['street'] = array(
    'title' => t('Street'),
    'help' => t('The street address of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_street',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['location']['city'] = array(
    'title' => t('City'),
    'help' => t('The city of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'empty field name' => t('Unknown'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'province' => 'province', style 'name'; 'province_code' => 'province', style 'code'
  $data['location']['province'] = array(
    'title' => t('Province'),
    'help' => t('The province of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_province',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'location_handler_argument_location_province',
      //'name field' => 'name',
    ),
    'filter' => array(
      'handler' => 'location_handler_filter_location_province',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      // TODO: needs handler to sort by name, not code
    ),
  );

  $data['location']['postal_code'] = array(
    'title' => t('Postal Code'),
    'help' => t('The postal code of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'country' => 'country', style 'name'; 'country_code' => 'country', style 'code'.
  $data['location']['country'] = array(
    'title' => t('Country'),
    'help' => t('The country of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_country',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'location_handler_argument_location_country',
      //'name field' => 'name',
    ),
    'filter' => array(
      'handler' => 'location_handler_filter_location_country',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      // TODO: needs handler to sort by name, not code
    ),
  );

  $data['location']['latitude'] = array(
    'title' => t('Latitude'),
    'help' => t('The latitude of the selected location.'),
    'field' => array(
      'handler' => 'location_views_handler_field_latitude',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['location']['longitude'] = array(
    'title' => t('Longitude'),
    'help' => t('The longitude of the selected location.'),
    'field' => array(
      'handler' => 'location_views_handler_field_longitude',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['location']['coordinates'] = array(
    'title' => t('Coordinates'),
    'help' => t("The coordinates of the selected location in 'lat, long' format."),
    'field' => array(
      'field' => 'latitude', // The handler adds the longitude.
      'handler' => 'location_views_handler_field_coordinates',
      'click sortable' => FALSE,
    ),
  );

  $data['location']['distance'] = array(
    'title' => t('Distance / Proximity'),
    'help' => t("The distance from the selected location and either the current user or a specific location."),
    'field' => array(
      'handler' => 'location_handler_field_location_distance',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'location_handler_sort_location_distance',
    ),
    'argument' => array(
      'handler' => 'location_handler_argument_location_proximity',
    ),
    'filter' => array(
      'handler' => 'location_views_handler_filter_proximity',
    ),
//    'relationship' => array(
//      'handler' => 'location_handler_relationship_location_distance',
//    ),
  );

//  $data['location']['coordinates_user'] = array(
//    'title' => t('Coordinates of logged-on user'),
//    'help' => t('This will contain the coordinates of the logged-on user.'),
//    'field' => array(
//      'handler' => 'location_handler_field_location_coordinates_user',
//      'click sortable' => FALSE,
//    ),
//  );

/*
      'latitude' => array(
        'name' => t('Latitude'),
        'sortable' => TRUE,
      ),
      'longitude' => array(
        'name' => t('Longitude'),
        'sortable' => TRUE,
      ),
*/


  $data['location']['address'] = array(
    'title' => t('Address'),
    'help' => t('The entire address block for the location.'),
    'field' => array(
      'field' => 'lid',
      'handler' => 'location_handler_field_location_address',
      'element type' => 'div',
    ),
  );


  $data['location_instance']['table']['group']  = t('Location');

  $data['location_instance']['table']['join'] = array(
    'location' => array(
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'node_revision' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );


  // Tell the base tables about us.
  $data['node']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['node_revision']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['users']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'uid',
    'field' => 'uid',
  );

  return $data;
}

/**
 * Helper function for proximity handlers.
 * Gets a list of available node location and cck location fields.
 *
 * @return
 *   An array of the location field options.
 */
function location_views_proximity_get_location_field_options() {
  // Get the CCK location field options.
  $field_options = array('node' => t('Node location'));
  if (module_exists('location_cck')) {
    $fields = field_info_fields();
    foreach ($fields as $field) {
      if ($field['module'] == 'location_cck') {
        $field_options[$field['field_name']] = t('CCK Location: @name', array('@name' => $field['field_name']));
      }
    }
  }
  return $field_options;
}

/**
 * Helper function for proximity handlers.
 * Gets available arguments for argument related handler options.
 *
 * @param $view
 *   The view object.
 *
 * @return
 *   An array containing arrays of the nid arguments and the uid arguments.
 */
function location_views_proximity_get_argument_options($view) {
  // Get the arguments for this view so we can use nid, uid or Global: Null arguments.
  $uid_argument_options = array();
  $nid_argument_options = array();
  $arguments = $view->display_handler->get_handlers('argument');
  foreach ($arguments as $id => $handler) {
    if ($handler->field == 'null') {
      $uid_argument_options[$id] = $handler->ui_name();
      $nid_argument_options[$id] = $handler->ui_name();
    }
    else if ($handler->field == 'nid') {
      $nid_argument_options[$id] = $handler->ui_name();
    }
    else if ($handler->field == 'uid') {
      $uid_argument_options[$id] = $handler->ui_name();
    }
  }
  return array($nid_argument_options, $uid_argument_options);
}

/**
 * Helper function for proximity handlers.
 * Retrieves the coordinates of the location that this field measures distances against.
 *
 * @param $view
 *   The view object.
 * @param $options
 *   An array of the options (or values in the case of the filter) of the handler.
 *
 * @return
 *   An array with keys 'latitude' and 'longitude' or an empty array.
 */
function location_views_proximity_get_reference_location($view, $options) {
  $coordinates = array();
  switch ($options['origin']) {
    case 'user':
    case 'hybrid':
      global $user;
      $user_locations = isset($user->locations) ? $user->locations : location_load_locations($user->uid, 'uid');
      // This user_location_delta will only possibly be set if we are dealing with the filter.
      $i = (isset($options['user_location_delta']) && !empty($options['user_location_delta'])) ? $options['user_location_delta'] : 0;
      if (isset($user_locations[$i]['latitude']) || !empty($user_locations[$i]['latitude'])) {
        $coordinates['latitude'] = (float) $user_locations[$i]['latitude'];
        $coordinates['longitude'] = (float) $user_locations[$i]['longitude'];
      }
      else if ($options['origin'] == 'hybrid') {
        $coordinates['latitude'] = (float) $options['latitude'];
        $coordinates['longitude'] = (float) $options['longitude'];
      }
      break;
    case 'static':
    case 'latlon_gmap':
      $coordinates['latitude'] = (float) $options['latitude'];
      $coordinates['longitude'] = (float) $options['longitude'];
      break;
    case 'tied':
      if (!empty($view->filter)) {
        foreach ($view->filter as $filter) {
          if ($filter->table == 'location' && $filter->field == 'distance' && $filter->options['relationship'] == $options['relationship']) {
            $filter_options = array_merge($filter->options, $filter->options['value'], $filter->value);
            if ($coords = location_views_proximity_get_reference_location($view, $filter_options)) {
              $coordinates['latitude'] = (float) $coords['latitude'];
              $coordinates['longitude'] = (float) $coords['longitude'];
            }
          }
        }
      }
      break;
    case 'postal':
    case 'postal_default':
      // Force default for country.
      if ($options['origin'] == 'postal_default') {
        $options['country'] = variable_get('location_default_country', 'us');
      }
      // Zip code lookup.
      if (!empty($options['postal_code']) && !empty($options['country'])) {
        $coords = location_latlon_rough($options);
        if ($coords) {
          $coordinates['latitude'] = (float) $coords['lat'];
          $coordinates['longitude'] = (float) $coords['lon'];
        }
      }
      break;
    case 'php':
      ob_start();
      $result = eval($options['php_code']);
      ob_end_clean();
      if ($result && $result['latitude'] && $result['longitude']) {
        $coordinates['latitude'] = (float) $result['latitude'];
        $coordinates['longitude'] = (float) $result['longitude'];
      }
      break;
    case 'nid_arg':
      if ($nodehandler = $view->display_handler->get_handler('argument', $options['nid_arg'])) {
        $nid = $nodehandler->get_value();
        if ($nid && is_numeric($nid) && $tempnode = node_load($nid)) {
          $field_name = $options['nid_loc_field'];
          if ($field_name == 'node') {
            $coordinates['latitude'] = (float) $tempnode->location['latitude'];
            $coordinates['longitude'] = (float) $tempnode->location['longitude'];
          }
          else {
            if (isset($tempnode->$field_name)) {
              $cck_location = $tempnode->$field_name;
              if (isset($cck_location[LANGUAGE_NONE][0]['longitude']) && isset($cck_location[LANGUAGE_NONE][0]['latitude'])) {
                $coordinates['latitude'] = (float) $cck_location[LANGUAGE_NONE][0]['latitude'];
                $coordinates['longitude'] = (float) $cck_location[LANGUAGE_NONE][0]['longitude'];
              }
              else if (isset($cck_location[0]['longitude']) && isset($cck_location[0]['latitude'])) {
                $coordinates['latitude'] = (float) $cck_location[0]['latitude'];
                $coordinates['longitude'] = (float) $cck_location[0]['longitude'];
              }
            }
          }
        }
      }
      break;
    case 'uid_arg':
      if ($userhandler = $view->display_handler->get_handler('argument', $options['uid_arg'])) {
        $uid = $userhandler->get_value();
        if ($uid && is_numeric($uid) && $tempuser = user_load(array('uid' => $uid))) {
          $coordinates['latitude'] = (float) $tempuser->location['latitude'];
          $coordinates['longitude'] = (float) $tempuser->location['longitude'];
        }
      }
      break;
    case 'distance_arg':
      foreach ($view->argument as $argument) {
        if ($argument->field == 'distance') {
          list($coords, $search_distance) = explode('_', $view->args[$argument->position]);
          list($lat, $lon) = explode(',', $coords);
          break;
        }
      }
      $coordinates['latitude'] = (float) $lat;
      $coordinates['longitude'] = (float) $lon;
      break;

  }

  return $coordinates;
}
