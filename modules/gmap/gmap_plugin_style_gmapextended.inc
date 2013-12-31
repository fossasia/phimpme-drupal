<?php

/**
 * @file
 * GMap style plugin.
 */

/**
 * Style plugin to render a map.
 *
 * @ingroup views_style_plugins
 */
class gmap_plugin_style_gmapextended extends views_plugin_style {
  /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();
    
    $options['fallback_values'] = array('default' => array());
    $options['field_purposes'] = array('default' => array());

    $options['macro'] = array(
      'default' => '[gmap ]',
    );

    $options['datasource'] = array(
      'default' => 'location',
    );

    $options['clickmode'] = array('default' => 'render');

    $options['markers'] = array('default' => 'static');
    $options['markertype'] = array('default' => 'drupal');

    $options['enablermt'] = array('default' => FALSE);
    $options['rmtcallback'] = array('default' => '');

    /*
    $options['center_on_nodearg'] = array('default' => 0);
    $options['center_on_nodearg_arg'] = array('default' => '');

    $options['highlight_nodearg'] = array('default' => 0);
    $options['highlight_nodearg_arg'] = array('default' => '');
    $options['highlight_nodearg_color'] = array('default' => '#FF0000');
    */

    return $options;
  }

  function purpose_list() {
    return array(
      '' => t('Unused'),
      'text' => t('Marker bubble contents'),
      'rmt' => t('RMT tail'), // @@@ Stupid title
      'iwq' => t('Info window query'),
      'iwo' => t('Info window offset'),
      'link' => t('URL to visit when clicking marker'),
      'latitude' => t('Marker latitude'),
      'longitude' => t('Marker longitude'),
      'dyn' => t('Address for dynamic geocoding'),
      'title' => t('Marker title (tooltip)'),
      'markername' => t('Marker type'),
      'offset' => t('Marker offset'),
    );
  }

  function query() {
    parent::query();

    if ($this->options['datasource'] == 'location') {
      $table = $this->view->query->ensure_table('location');
      $this->view->query->add_field($table, 'latitude', 'gmap_lat');
      $this->view->query->add_field($table, 'longitude', 'gmap_lon');
    }

    if ($this->options['markers'] == 'nodetype') {
      $this->view->query->add_field('node', 'type', 'gmap_node_type');
    }
    else if ($this->options['markers'] == 'taxonomy') {
      $this->view->query->add_field('gmap_taxonomy_node', 'marker', 'gmap_node_marker');
    }
    else if ($this->options['markers'] == 'userrole') {
      $this->view->query->add_field('users_roles', 'rid', 'gmap_role_marker');
    }

    if (isset($this->row_plugin)) {
      $this->row_plugin->query();
    }
  }

  /**
   * Render the given style.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#markup' => '<div class="error form-item description">' . t('You need at least one field before you can configure your GMap settings') . '</div>',
      );
      return;
    }

    $form['#theme'] = 'gmap_views_ui_gmapextended';

    $purposes = $this->purpose_list();

    $fields = $this->display->handler->get_option('fields');

    // Create an array of allowed columns from the data we know:
    $field_names = $this->display->handler->get_field_labels();
    foreach ($field_names as $field => $title) {
      $form['field_purposes'][$field] = array(
        '#type' => 'select',
        '#options' => $purposes,
        '#default_value' => isset($this->options['field_purposes'][$field]) ? $this->options['field_purposes'][$field] : '',
      );
      // @@@ Remove?
      $form['info'][$field]['separator'] = array(
        '#type' => 'textfield',
        '#size' => 10,
        '#default_value' => isset($this->options['info'][$field]['separator']) ? $this->options['info'][$field]['separator'] : '',
      );

      // markup for the field name
      $form['info'][$field]['name'] = array(
        '#markup' => $field_names[$field],
      );
    }

    $form['description_markup'] = array(
      '#markup' => '<div class="description form-item">' . t('Assign purposes to fields you are using to supply map data. NOTE: This style is under heavy development currently, no backwards compatibility is guaranteed!') . '</div>',
    );

    $form['clickmode'] = array(
      '#type' => 'radios',
      '#title' => t('Marker click action'),
      '#options' => array(
        'render' => t('Show formatted row in bubble'),
        'text' => t('Show defined text in bubble'),
        'rmt' => t('Fetch data from callback in bubble (RMT mode)'),
        'iwq' => t('Use pieces of the DOM (iwq mode)'),
        'link' => t('Go to URL'),
      ),
      '#default_value' => isset($this->options['clickmode']) ? $this->options['clickmode'] : 'render',
    );
    
    $form['rmtcallback'] = array(
      '#type' => 'textfield',
      '#title' => t('RMT callback path'),
      '#description' => t('Define the base path to the callback here. The value of the RMT tail will be appended.'),
      '#default_value' => $this->options['rmtcallback'],
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('rmt')),
    );
    
    $form['iwq'] = array(
      '#type' => 'textfield',
      '#title' => t('Info window query (default)'),
      '#description' => t('Define the default info window query here. This is in the form of a jQuery selector. You will usually want to match a set of divs and then use iwo to choose which one.'),
      '#process' => array('ctools_dependent_process'),
      '#default_value' => $this->options['iwq'],
      '#dependency' => array('radio:style_options[clickmode]' => array('iwq')),
    );

    $form['fallback_values'] = array(
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#title' => 'Default values',
      '#description' => t('You can set the value to use for marker parameters not controlled by fields here.'),
    );

    $default = $this->options['fallback_values'];
    $form['fallback_values']['text'] = array( // @@@ Make this #text_format compatible so people can do neat tricks.
      '#type' => 'textarea',
      '#title' => t('Marker bubble contents'),
      '#default_value' => isset($default['text']) ? $default['text'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('text')),
    );
    $form['fallback_values']['rmt'] = array(
      '#type' => 'textfield',
      '#title' => t('RMT tail'),
      '#default_value' => isset($default['rmt']) ? $default['rmt'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('rmt')),
    );
    $form['fallback_values']['iwq'] = array(
      '#type' => 'textfield',
      '#title' => t('Info window query'),
      '#default_value' => isset($default['iwq']) ? $default['iwq'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('iwq')),
    );
    $form['fallback_values']['iwo'] = array(
      '#type' => 'textfield',
      '#title' => t('Info window offset'),
      '#default_value' => isset($default['iwo']) ? $default['iwo'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('iwq')),
    );
    $form['fallback_values']['link'] = array(
      '#type' => 'textfield',
      '#title' => t('Marker click URL'),
      '#default_value' => isset($default['link']) ? $default['link'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:style_options[clickmode]' => array('link')),
    );
    $form['fallback_values']['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#default_value' => isset($default['latitude']) ? $default['latitude'] : '',
    );
    $form['fallback_values']['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#default_value' => isset($default['longitude']) ? $default['longitude'] : '',
    );
    $form['fallback_values']['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Marker title (tooltip)'),
      '#description' => t('Set a fallback value for the marker tooltip if you want to use a fixed tooltip instead of assigning a field, or leave blank.'),
      '#default_value' => isset($default['title']) ? $default['title'] : '',
    );
    $markernames = array('' => t('Use site default'));
    $markernames += gmap_get_marker_titles();
    $form['fallback_values']['markername'] = array(
      '#type' => 'select',
      '#title' => t('Marker type'),
      '#options' => $markernames,
      '#description' => t('Set a fallback value for the marker type if you want to use a fixed type.'),
      '#default_value' => isset($default['markername']) ? $default['markername'] : '',
    );
    $form['fallback_values']['offset'] = array(
      '#type' => 'textfield',
      '#title' => t('Marker offset'),
      '#description' => t('Set a fallback value if you want to use a static offset. Leave blank if you want automatic assignment.'),
      '#default_value' => isset($default['offset']) ? $default['offset'] : '',
    );


////////////////////////// Form stuff below this line needs attention.

    $form['macro'] = array(
      '#type' => 'textarea',
      '#title' => t('Macro'),
      '#rows' => 3,
      '#default_value' => $this->options['macro'],
    );

    $form['markers'] = array(
      '#type' => 'select',
      '#title' => t('Marker handling'),
      // @@@ Detect view type automatically?
      '#options' => array(
        'nodetype' => t('By content type (for node views)'),
        'taxonomy' => t('By term (for node views)'),
        'userrole' => t('By user role (for user views)'),
        'field' => t('Use marker field'),
        'static' => t('Use single marker type'),
      ),
      '#default_value' => $this->options['markers'],
    );

    // Hide the taxonomy handling if gmap_taxonomy.module isn't installed.
    if (!module_exists('gmap_taxonomy')) {
      unset($form['markers']['#options']['taxonomy']);
    }

    $form['markertype'] = array(
      '#type' => 'gmap_markerchooser',
      '#title' => t('Marker / fallback marker to use'),
      '#default_value' => $this->options['markertype'],
    );
/*
    $form['center_on_nodearg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Center on node argument'),
      '#default_value' => $this->options['center_on_nodearg'],
      '#description' => ($this->view->base_table == 'node') ? t('Note: The view must contain an argument whose value is a node ID.') : t('Note: The view must contain an argument whose value is a node ID.') . '<br />' . t("The view must contain 'Node: nid' as one of its fields because the view type is not 'Node'."),
    );
    $form['center_on_nodearg_arg'] = array(
      '#title' => t('Argument'),
      '#description' => empty($argument_options) ? t("The value of the selected argument must be a number that matches a node ID.  Use the 'Global: Null' argument if you don't want to also restrict results to that node ID.  You must have added arguments to the view to use this option.") : t("The selected argument must be a number that matches a node ID.  Use the 'Global: Null' argument if you don't want to also restrict results to that node ID."),
      '#type' => 'select',
      '#options' => $argument_options,
      '#default_value' => $this->options['center_on_nodearg_arg'],
//      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-center-on-nodearg' => array(TRUE)),
    );

    $form['highlight_nodearg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Highlight marker for node argument'),
      '#default_value' => $this->options['highlight_nodearg'],
      '#description' => ($this->view->base_table == 'node') ? t('Note: The view must contain an argument whose value is a node ID.') : t('Note: The view must contain an argument whose value is a node ID.') . '<br />' . t("The view must contain 'Node: nid' as one of its fields because the view type is not 'Node'."),
    );
    $form['highlight_nodearg_arg'] = array(
      '#title' => t('Argument'),
      '#description' => empty($argument_options) ? t("The value of the selected argument must be a number that matches a node ID.  Use the 'Global: Null' argument if you don't want to also restrict results to that node ID.  You must have added arguments to the view to use this option.") : t("The value of the selected argument must be a number that matches a node ID.  Use the 'Global: Null' argument if you don't want to also restrict results to that node ID."),
      '#type' => 'select',
      '#options' => $argument_options,
      '#default_value' => $this->options['highlight_nodearg_arg'],
//      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-highlight-nodearg' => array(TRUE)),
    );
    $form['highlight_nodearg_color'] = array(
      '#title' => t('Highlight color'),
      '#description' => t("A 6 digit hex color value to use for the highlight. Include preceding hash. Example #FF0000"),
      '#type' => 'textfield',
      '#size' => 7,
      '#maxlength' => 7,
      '#default_value' => $this->options['highlight_nodearg_color'],
//      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-highlight-nodearg' => array(TRUE)),
    );
*/
  }

  /**
   * Validate the options form.
   */
  function options_validate(&$form, &$form_state) {
    // Check if highlight color is a valid hex color
    if (!preg_match('/^#[a-f0-9]{6}$/i', $form_state['values']['style_options']['highlight_nodearg_color'])) {
      form_error($form['highlight_nodearg_color'], t('Highlight colour must be a valid hex code in the form #FF0000.'));
    }
  }

  function render() {
    if (isset($this->view->live_preview) && $this->view->live_preview) {
      return t('GMap views are not compatible with live preview.');
    }
    return parent::render();
  }
}
