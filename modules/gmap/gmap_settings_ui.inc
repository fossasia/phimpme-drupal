<?php

/**
 * @file
 * GMap settings form.
 */

function gmap_admin_settings($form, &$form_state) {
  // Reset the icondata cache.
  gmap_get_icondata(TRUE);

  //note the same google api key variable name as in the googlemap module is used
  //note the name of the variable for center of the map is latlong although the format is actually longitude, latitude

  $form['initialization'] = array(
    '#type' => 'fieldset',
    '#title' => t('Google Map Initialize'),
  );

  if (!module_exists('keys_api') && !module_exists('keys')) {
    $form['initialization']['googlemap_api_key'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('googlemap_api_key', ''),
      '#size' => 50,
      '#maxlength' => 255,
    );
  }
  else {
    $form['initialization']['googlemap_api_key'] = array(
      '#type' => 'item',
      '#title' => t('Google Maps API Key'),
      '#value' => t('Managed by <a href="@url">Keys</a>.', array('@url' => url('admin/settings/keys'))),
    );
  }

  $form['initialization']['googlemap_api_key']['#title'] = t('Google Maps API Key');
  $form['initialization']['googlemap_api_key']['#description'] = t('Your personal Googlemaps API key.  You must get this for each separate website at <a href="http://code.google.com/apis/maps/signup.html">Google Map API website</a>.');

  $form['initialization']['rebuild'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regenerate marker cache'),
    '#description' => t('If you are having problems with markers, or have modified the .ini files in the markers folder, click here to rebuild the marker cache file.'),
  );

  $form['initialization']['rebuild']['rebuild_marker_js'] = array(
    '#type' => 'submit',
    '#value' => t('Regenerate'),
    '#submit' => array('_gmap_rebuild_marker_js_submit'),
  );

  $defaults = gmap_defaults();

  $form['gmap_default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default map settings'),
    // This will store all the defaults in one variable.
    '#tree' => TRUE,
  );
  $form['gmap_default']['map'] = array(
    '#type' => 'gmap',
    '#map' => 'settings_default_map',
    '#gmap_settings' => array(
      'behavior' => array(
        'nodrag' => FALSE,
        'nokeyboard' => FALSE,
      ),
    ),
  );

  // Allow previewable behaviors to affect the preview map.
  $m = array();
  $behaviors = gmap_module_invoke('behaviors', $m);
  foreach ($behaviors as $k => $v) {
    if (isset($v['previewable']) && $v['previewable']) {
      $form['gmap_default']['map']['#gmap_settings']['behavior'][$k] = $defaults['behavior'][$k];
    }
  }

  $form['gmap_default']['width'] = array(
    '#type' => 'gmap_dimension',
    '#title' => t('Default width'),
    '#default_value' => $defaults['width'],
    '#size' => 25,
    '#maxlength' => 25,
    '#description' => t('The default width of a Google map, as a CSS length or percentage. Examples: <em>50px</em>, <em>5em</em>, <em>2.5in</em>, <em>95%</em>'),
  );
  gmap_widget_setup($form['gmap_default']['width'], 'width', 'settings_default_map');
  $form['gmap_default']['height'] = array(
    '#type' => 'gmap_dimension',
    '#title' => t('Default height'),
    '#default_value' => $defaults['height'],
    '#size' => 25,
    '#maxlength' => 25,
    '#description' => t('The default height of a Google map, as a CSS length or percentage. Examples: <em>50px</em>, <em>5em</em>, <em>2.5in</em>, <em>95%</em>'),
  );
  gmap_widget_setup($form['gmap_default']['height'], 'height', 'settings_default_map');
  $form['gmap_default']['latlong'] = array(
    '#type' => 'gmap_latlon',
    '#map' => 'settings_default_map',
    '#title' => t('Default center'),
    '#default_value' => $defaults['latlong'],
    '#size' => 50,
    '#maxlength' => 255,
    '#description' => t('The default center coordinates of Google map, expressed as a decimal latitude and longitude, separated by a comma.'),
  );
  $form['gmap_default']['zoom'] = array(
    '#type' => 'select',
    '#title' => t('Default zoom'),
    '#default_value' => $defaults['zoom'],
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('The default zoom level of a Google map.'),
  );
  gmap_widget_setup($form['gmap_default']['zoom'], 'zoom', 'settings_default_map');
  $form['gmap_default']['maxzoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum initial zoom'),
    '#default_value' => $defaults['maxzoom'],
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('The maximum initial zoom (affects things such as the zoom of the node location block.)'),
  );
  // This one isn't a gmap widget.

  $form['gmap_default']['styles']['line_default'] = array(
    '#type' => 'gmap_style',
    '#gmap_style_type' => 'line',
    '#title' => t('Line default style'),
    '#description' => t('Lines without a specific style defined will fall back to this style'),
    '#default_value' => $defaults['styles']['line_default'],
  );
  $form['gmap_default']['styles']['poly_default'] = array(
    '#type' => 'gmap_style',
    '#gmap_style_type' => 'poly',
    '#title' => t('Polygon default style'),
    '#description' => t('Polygons without a specific style defined will fall back to this style'),
    '#default_value' => $defaults['styles']['poly_default'],
  );
  $form['gmap_default']['styles']['highlight_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Highlight color'),
    '#size' => 6,
    '#maxlength' => 6,
    '#field_prefix' => '#',
    '#description' => t('This sets the color of the marker highlight when the "highlight : Highlight marker on rollover" behaviour is enabled. Hex color value. Example: #00AA33'),
    '#default_value' => $defaults['styles']['highlight_color'],
  );

  $form['gmap_default']['controltype'] = array(
    '#type' => 'select',
    '#title' => t('Default control type'),
    '#options' => array('None' => t('None'), 'Micro' => t('Micro'), 'Small' => t('Small'), 'Large' => t('Large')),
    '#default_value' => $defaults['controltype'],
  );
  gmap_widget_setup($form['gmap_default']['controltype'], 'controltype', 'settings_default_map');

  $form['gmap_default']['mtc'] = array(
    '#type' => 'select',
    '#title' => t('Map Type Control'),
    '#options' => array(
      'none' => t('None'),
      'standard' => t('Standard (GMapTypeControl)'),
      'hier' => t('Hierarchical (GHierarchicalMapTypeControl)'),
      'menu' => t('Dropdown (GMenuMapTypeControl)'),
    ),
    '#default_value' => $defaults['mtc'],
  );

  $form['gmap_default']['baselayers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enabled map types ("base layers")'),
  );

  $baselayers = array();

  gmap_module_invoke('baselayers', $baselayers);

  $options = array();
  foreach ($baselayers as $name => $layers) {
    $options[$name] = array();
    foreach ($layers as $k => $v) {
      // @@@TODO: Only show the enabled ones?
      $options[$name][$k] = $v['title'];
    }
  }

  $form['gmap_default']['baselayers']['maptype'] = array(
    '#type' => 'select',
    '#tree' => FALSE,
    '#parents' => array('gmap_default', 'maptype'),
    '#title' => t('Default map type'),
    '#default_value' => $defaults['maptype'],
    '#options' => $options,
  );
  gmap_widget_setup($form['gmap_default']['baselayers']['maptype'], 'maptype', 'settings_default_map');

  foreach ($baselayers as $name => $layers) {
    $form['gmap_default']['baselayers'][$name] = array(
      '#type' => 'fieldset',
      '#title' => $name,
    );
    foreach ($layers as $key => $layer) {
      $form['gmap_default']['baselayers'][$name][$key] = array(
        '#type' => 'checkbox',
        '#title' => $layer['title'],
        '#parents' => array('gmap_default', 'baselayers', $key),

        '#description' => $layer['help'],
        '#default_value' => isset($defaults['baselayers'][$key]) ? $defaults['baselayers'][$key] : $layer['default'],
      );
    }
  }

  $form['gmap_default']['behavior'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map Behavior flags'),
    '#tree' => TRUE,
    '#description' => t('Behavior flags modify how a map behaves. Grayed out flags are not settable here, but may be set on a map by map basis via code or a macro. Changes to behaviors will not affect the preview map shown above until changes are saved.'),
  );
  $m = array();
  $behaviors = gmap_module_invoke('behaviors', $m);
  foreach ($behaviors as $k => $v) {
    $form['gmap_default']['behavior'][$k] = array(
      '#type' => 'checkbox',
      '#title' => t('@name : @title', array('@name' => $k, '@title' => $v['title'])),
      '#default_value' => isset($defaults['behavior'][$k]) ? $defaults['behavior'][$k] : $v['default'],
      '#description' => isset($v['help']) ? $v['help'] : '',
    );
    if (isset($v['internal']) && $v['internal']) {
      $form['gmap_default']['behavior'][$k]['#disabled'] = TRUE;
      // Compensate for behaviors becoming internal after acquiring the wrong value.
      $form['gmap_default']['behavior'][$k]['#value'] = $v['default'];
    }
  }

  $form['gmap_default']['markermode'] = array(
    '#type' => 'radios',
    '#title' => t('Marker action'),
    '#description' => t('Perform this action when a marker is clicked.'),
    '#options' => array(t('Do nothing'), t('Open info window'), t('Open link')),
    '#default_value' => isset($defaults['markermode']) ? $defaults['markermode'] : 0,
  );

  $form['gmap_default']['line_colors'] = array(
    '#tree' => TRUE,
  );
  $form['gmap_default']['line_colors'][0] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 1 Color'),
    '#default_value' => $defaults['line_colors'][0],
    '#size' => 12,
    '#maxlength' => 7,
  );
  $form['gmap_default']['line_colors'][1] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 2 Color'),
    '#default_value' => $defaults['line_colors'][1],
    '#size' => 12,
    '#maxlength' => 7,
  );
  $form['gmap_default']['line_colors'][2] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 3 Color'),
    '#default_value' => $defaults['line_colors'][2],
    '#size' => 12,
    '#maxlength' => 7,
  );

  $opts = variable_get('gmap_markermanager', array());
  if (!isset($opts['gmap']) || !is_array($opts['gmap'])) {
    $opts['gmap'] = array();
  }
  if (!isset($opts['gmarkermanager']) || !is_array($opts['gmarkermanager'])) {
    $opts['gmarkermanager'] = array();
  }
  if (!isset($opts['markermanager']) || !is_array($opts['markermanager'])) {
    $opts['markermanager'] = array();
  }
  if (!isset($opts['markerclusterer']) || !is_array($opts['markerclusterer'])) {
    $opts['markerclusterer'] = array();
  }
  if (!isset($opts['clusterer']) || !is_array($opts['clusterer'])) {
    $opts['clusterer'] = array();
  }
  if (!isset($opts['clustermarker']) || !is_array($opts['clustermarker'])) {
    $opts['clustermarker'] = array();
  }

  $opts['gmap'] = array_merge(array(
    // None.
  ), $opts['gmap']);

  $opts['gmarkermanager'] = array_merge(array(
    'borderPadding' => 256,
    'maxZoom' => 4,
    'trackMarkers' => FALSE,
    'markerMinZoom' => 4,
    'markerMaxZoom' => 0,
  ), $opts['gmarkermanager']);

  $opts['markermanager'] = array_merge(array(
    'filename' => 'markermanager_packed.js',
    'borderPadding' => 256,
    'maxZoom' => 4,
    'trackMarkers' => FALSE,
    'markerMinZoom' => 4,
    'markerMaxZoom' => 0,
  ), $opts['markermanager']);
  
  $opts['markerclusterer'] = array_merge(array(
    'filename' => 'markerclusterer_packed.js',
    'gridSize' => 60,
    'maxZoom' => 17,
    // @@@ Some way of representing MarkerStyleOptions?
  ), $opts['markerclusterer']);

  $opts['clusterer'] = array_merge(array(
    'filename' => 'Clusterer2.js',
    'marker' => 'cluster',
    'max_nocluster' => 150,
    'cluster_min' => 5,
    'max_lines' => 10,
    'popup_mode' => 'orig',
  ), $opts['clusterer']);

  $opts['clustermarker'] = array_merge(array(
    'filename' => 'ClusterMarker.js',
    'borderPadding' => 256,
    'clusteringEnabled' => TRUE,
    'clusterMarkerIcon' => '',
    'clusterMarkerTitle' => '',
    'fitMapMaxZoom' => 0,
    'intersectPadding' => 0,
  ), $opts['clustermarker']);

  $form['gmap_markermanager'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker manager'),
    '#tree' => TRUE,
  );
  $form['gmap_markermanager']['gmap_mm_type'] = array(
    '#type' => 'radios',
    '#tree' => FALSE,
    '#required' => TRUE,
    '#options' => array(
      'gmap' => t('No manager (use GMap API directly)'),
      'gmarkermanager' => t("Google's GMarkerManager"),
      'markermanager' => t('GMaps Utility Library MarkerManager'),
      'markerclusterer' => t('GMaps Utility Library MarkerClusterer'),
      'clusterer' => t("Jef Poskanzer's Clusterer"),
      'clustermarker' => t("Martin Pearman's ClusterMarker"),
    ),
    '#default_value' => variable_get('gmap_mm_type', 'gmap'),
    '#description' => t('If you are planning on using many markers on a single map, you may want to consider using a marker manager to speed up map rendering.'),
  );

  // No Manager
  $form['gmap_markermanager']['gmap'] = array(
    '#type' => 'fieldset',
    '#title' => t('Unmanaged marker settings'),
    '#description' => t('There are no settings for unmanaged markers.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // Google Maps API GMarkerManager
  $form['gmap_markermanager']['gmarkermanager'] = array(
    '#type' => 'fieldset',
    '#title' => t('GMarkerManager settings'),
    '#description' => t('GMarkerManager is a new part of the official Google Maps API that provides a marker manager.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmap_markermanager']['gmarkermanager']['borderPadding'] = array(
    '#type' => 'textfield',
    '#title' => t('Border padding'),
    '#description' => t('Markers located less than this number of pixels from the viewport will be added to the map by the manager (even if they would be fully invisible.)'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => $opts['gmarkermanager']['borderPadding'],
  );
  $form['gmap_markermanager']['gmarkermanager']['maxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above, the marker manager will disable itself for additional speed.'),
    '#default_value' => $opts['gmarkermanager']['maxZoom'],
  );
  $form['gmap_markermanager']['gmarkermanager']['trackMarkers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track markers'),
    '#description' => t('If enabled, the marker manager will track marker movements. Leave off unless you need to move markers around with setPoint.'),
    '#default_value' => $opts['gmarkermanager']['trackMarkers'],
  );
  $form['gmap_markermanager']['gmarkermanager']['defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker defaults'),
    '#description' => t('Default marker-specific settings for GMarkerManager. Markers will appear when the current zoom level is between minZoom and maxZoom.'),
  );
  $form['gmap_markermanager']['gmarkermanager']['defaults']['markerMinZoom'] = array(
    '#type' => 'select',
    '#title' => t('Minimum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be shown. Choose 0 to show markers at all zoom levels by default.'),
    '#default_value' => $opts['gmarkermanager']['markerMinZoom'],
    '#parents' => array('gmap_markermanager', 'gmarkermanager', 'markerMinZoom'),
  );
  $form['gmap_markermanager']['gmarkermanager']['defaults']['markerMaxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be hidden. Choose 0 to disable by default.'),
    '#default_value' => $opts['gmarkermanager']['markerMaxZoom'],
    '#parents' => array('gmap_markermanager', 'gmarkermanager', 'markerMaxZoom'),
  );

  // GMaps Utility Library -- MarkerManager
  $form['gmap_markermanager']['markermanager'] = array(
    '#type' => 'fieldset',
    '#title' => t('MarkerManager settings'),
    '#description' => t('MarkerManager is the improved open source version of GMarkerManager. To use, you must download it from <a href="@url">here</a> and place it in the <em>thirdparty</em> folder.', array('@url' => 'http://gmaps-utility-library.googlecode.com/svn/trunk/markermanager/release/src/')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmap_markermanager']['markermanager']['filename'] = array(
    '#type' => 'textfield',
    '#title' => t('Filename'),
    '#description' => t('Name of downloaded file in the thirdparty folder. Default: %default', array('%default' => 'markermanager_packed.js')),
    '#default_value' => $opts['markermanager']['filename'],
  );
  $form['gmap_markermanager']['markermanager']['borderPadding'] = array(
    '#type' => 'textfield',
    '#title' => t('Border padding'),
    '#description' => t('Markers located less than this number of pixels from the viewport will be added to the map by the manager (even if they would be fully invisible.)'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => $opts['markermanager']['borderPadding'],
  );
  $form['gmap_markermanager']['markermanager']['maxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above, the marker manager will disable itself for additional speed.'),
    '#default_value' => $opts['markermanager']['maxZoom'],
  );
  $form['gmap_markermanager']['markermanager']['trackMarkers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track markers'),
    '#description' => t('If enabled, the marker manager will track marker movements. Leave off unless you need to move markers around with setPoint.'),
    '#default_value' => $opts['markermanager']['trackMarkers'],
  );
  $form['gmap_markermanager']['markermanager']['defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker defaults'),
    '#description' => t('Default marker-specific settings for MarkerManager. Markers will appear when the current zoom level is between minZoom and maxZoom.'),
  );
  $form['gmap_markermanager']['markermanager']['defaults']['markerMinZoom'] = array(
    '#type' => 'select',
    '#title' => t('Minimum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be shown. Choose 0 to show markers at all zoom levels by default.'),
    '#default_value' => $opts['markermanager']['markerMinZoom'],
    '#parents' => array('gmap_markermanager', 'markermanager', 'markerMinZoom'),
  );

  $form['gmap_markermanager']['markermanager']['defaults']['markerMaxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be hidden. Choose 0 to disable by default.'),
    '#default_value' => $opts['markermanager']['markerMaxZoom'],
    '#parents' => array('gmap_markermanager', 'markermanager', 'markerMaxZoom'),
  );

  // GMaps Utility Library -- MarkerClusterer
  $form['gmap_markermanager']['markerclusterer'] = array(
    '#type' => 'fieldset',
    '#title' => t('MarkerClusterer settings'),
    '#description' => t('MarkerClusterer creates and manages per-zoom-level clusters for large amounts of markers (hundreds or thousands.) To use, you must download it from <a href="@url">here</a> and place it in the <em>thirdparty</em> folder.', array('@url' => 'http://gmaps-utility-library-dev.googlecode.com/svn/tags/markerclusterer/1.0/src/')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmap_markermanager']['markerclusterer']['filename'] = array(
    '#type' => 'textfield',
    '#title' => t('Filename'),
    '#description' => t('Name of downloaded file in the thirdparty folder. Default: %default', array('%default' => 'markerclusterer_packed.js')),
    '#default_value' => $opts['markerclusterer']['filename'],
  );
  $form['gmap_markermanager']['markerclusterer']['gridSize'] = array(
    '#type' => 'textfield',
    '#title' => t('Grid Size'),
    '#description' => t('The grid size of a cluster in pixels. Each cluster will be a square. If you want the algorithm to run faster, you can set this value larger.'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => $opts['markerclusterer']['gridSize'],
  );
  $form['gmap_markermanager']['markerclusterer']['maxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At this zoom and above, all markers will be shown without clustering.'),
    '#default_value' => $opts['markerclusterer']['maxZoom'],
  );

  // Jef Poskanzer's Clusterer
  $form['gmap_markermanager']['clusterer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clusterer settings'),
    '#description' => t('Clusterer is a marker manager written by Jef Poskanzer of acme.com. To use, you must place Clusterer2.js (available <a href="@url">here</a>) into the "thirdparty" folder.', array('@url' => 'http://acme.com/javascript/Clusterer2.js')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmap_markermanager']['clusterer']['filename'] = array(
    '#type' => 'textfield',
    '#title' => t('Filename'),
    '#description' => t('Name of downloaded file in the thirdparty folder. Default: %default', array('%default' => 'Clusterer2.js')),
    '#default_value' => $opts['clusterer']['filename'],
  );
  $form['gmap_markermanager']['clusterer']['marker'] = array(
    '#type' => 'gmap_markerchooser',
    '#title' => t('Marker for clusters'),
    '#description' => t('The marker to use when creating a cluster.'),
    '#default_value' => $opts['clusterer']['marker'],
  );
  $form['gmap_markermanager']['clusterer']['max_nocluster'] = array(
    '#type' => 'textfield',
    '#title' => t('Activate on'),
    '#field_suffix' => t('or more markers'),
    '#description' => t("Clustering is enabled when more than the specified number of markers are visible at the same time."),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => $opts['clusterer']['max_nocluster'],
  );
  $form['gmap_markermanager']['clusterer']['cluster_min'] = array(
    '#type' => 'textfield',
    '#title' => t('Cluster on'),
    '#field_suffix' => t('or more markers'),
    '#description' => t("Minimal number of markers per cluster"),
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => $opts['clusterer']['cluster_min'],
  );
  $form['gmap_markermanager']['clusterer']['max_lines'] = array(
    '#type' => 'textfield',
    '#title' => t('Lines per box'),
    '#field_prefix' => t('at most'),
    '#field_suffix' => t('lines'),
    '#description' => t("Maximum number of lines per info box"),
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => $opts['clusterer']['max_lines'],
  );
  $form['gmap_markermanager']['clusterer']['popup_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Popup mode'),
    '#options' => array('orig' => t('Original'), 'zoom' => t('Zoom to Cluster')),
    '#default_value' => $opts['clusterer']['popup_mode'],
  );

  // Martin Pearman's ClusterMarker
  $form['gmap_markermanager']['clustermarker'] = array(
    '#type' => 'fieldset',
    '#title' => t('ClusterMarker settings'),
    '#description' => t('ClusterMarker is a marker manager written by Martin Pearman. To use, you must download it from <a href="@url">here</a> and extract the appropriate file to the <em>thirdparty</em> folder.', array('@url' => 'http://googlemapsapi.martinpearman.co.uk/downloads.php?cat_id=1')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmap_markermanager']['clustermarker']['filename'] = array(
    '#type' => 'textfield',
    '#title' => t('Filename'),
    '#description' => t('Name of downloaded file in the thirdparty folder. Default: %default', array('%default' => 'ClusterMarker.js')),
    '#default_value' => $opts['clustermarker']['filename'],
  );
  $form['gmap_markermanager']['clustermarker']['borderPadding'] = array(
    '#type' => 'textfield',
    '#title' => t('Border padding'),
    '#description' => t('Markers located less than this number of pixels from the viewport will be added to the map by the manager (even if they would be fully invisible.)'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => $opts['clustermarker']['borderPadding'],
  );
  $form['gmap_markermanager']['clustermarker']['clusteringEnabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clustering enabled'),
    '#description' => t('Whether to use clustering or not.'),
    '#default_value' => $opts['clustermarker']['clusteringEnabled'],
  );
  $form['gmap_markermanager']['clustermarker']['clusterMarkerIcon'] = array(
    '#type' => 'gmap_markerchooser',
    '#title' => t('Marker for clusters'),
    '#description' => t('The marker to use when creating a cluster.'),
    '#default_value' => $opts['clustermarker']['clusterMarkerIcon'],
  );
  $form['gmap_markermanager']['clustermarker']['clusterMarkerTitle'] = array(
    '#type' => 'textfield',
    '#title' => t('Title for clusters'),
    '#description' => t("The title to use for clusters. %count will be replaced with the number of markers in the cluster."),
    '#default_value' => $opts['clustermarker']['clusterMarkerTitle'],
  );
  $form['gmap_markermanager']['clustermarker']['fitMapMaxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom for zooming to cluster'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('When clicking on a cluster, the map will not zoom in farther than this.'),
    '#default_value' => $opts['clustermarker']['fitMapMaxZoom'],
  );
  $form['gmap_markermanager']['clustermarker']['intersectPadding'] = array(
    '#type' => 'textfield',
    '#title' => t('Intersect Padding'),
    '#description' => t('Number of pixels to pad marker icons by when determining whether they intersect.'),
    '#default_value' => $opts['clustermarker']['intersectPadding'],
    '#size' => 4,
    '#maxlength' => 4,
  );

  // @@@ Convert to element level validation.
  $form['#validate'][] = 'gmap_admin_settings_validate';

  return system_settings_form($form);
}

function gmap_admin_settings_validate($form, &$form_state) {
  $found = FALSE;
  foreach ($form_state['values']['gmap_default']['baselayers'] as $l) {
    if ($l) {
      $found = TRUE;
      break;
    }
  }
  if (!$found) {
    form_set_error('gmap_default][baselayers', t('You must select at least one baselayer!'));
  }

  // Check that the default map type is sane.
  if (!$form_state['values']['gmap_default']['baselayers'][$form_state['values']['gmap_default']['maptype']]) {
    form_error($form['gmap_default']['baselayers']['maptype'], t('The default map type must be an enabled baselayer!'));
  }
}

/**
 * Rebuild marker js.
 */
function _gmap_rebuild_marker_js_submit($form, &$form_state) {
  gmap_regenerate_markers();
  drupal_set_message(t('Marker cache regenerated.'));
}
