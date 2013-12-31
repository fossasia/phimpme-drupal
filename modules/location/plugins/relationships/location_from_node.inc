<?php

/**
 * @file
 * Plugin to provide an relationship handler for location data from node.
 */

/**
 * Implement hook_[relationship_name]_ctools_relationships().
 */
function location_location_from_node_ctools_relationships() {
  return array(
    'title' => t('Node location'),
    'keyword' => 'location',
    'description' => t('Creates a location context from a node.'),
    'required context' => new ctools_context_required(t('Node'), 'node'),
    'context' => 'location_location_from_node_context',
    'settings form' => 'location_location_from_node_settings_form',
  );
}

/**
 * Return a new location context based on a node context.
 *
 * This does all the magic from the relationship settings to select if we're
 * dealing with a location_node field or a location_cck field, what field
 * offset to use, etc.
 *
 * @param $context
 *   The node context we're creating a relationship context out of.
 * @param $conf
 *   The configuration settings for this relationship.
 *
 * @return
 *   Either a valid location context object based on the given node and the
 *   currently active configuration settings, an empty context object
 *   (e.g. for configuration in the Panels admin UI), or NULL in case the
 *   given node context does not contain valid location data matching the
 *   current relationship settings.
 */
function location_location_from_node_context($context, $conf) {
  // If unset it wants a generic, unfilled context, which is just NULL.
  if (empty($context->data)) {
    $new_context = ctools_context_create_empty('location', NULL);
  }

  // Grab the right location data based on the relationship settings.
  if (isset($conf['location_type'])) {
    if ($conf['location_type'] == 'location_node' && isset($context->data->locations)) {
      $location_offset = isset($conf['location_node_offset']) ? $conf['location_node_offset'] : 0;
      $location = $context->data->locations[$location_offset];
    }
    elseif ($conf['location_type'] == 'location_cck' && isset($conf['location_cck_field_name'])) {
      $field_name = $conf['location_cck_field_name'];
      $field_offset = isset($conf['location_cck_field_offset']) ? $conf['location_cck_field_offset'] : 0;
      if (isset($context->data->{$field_name}[$field_offset]['lid'])) {
        $location = $context->data->{$field_name}[$field_offset];
      }
    }
  }

  if (!empty($location)) {
    $location['nid'] = $context->data->nid;
    $new_context = ctools_context_create('location', $location);
  }

  if (!empty($new_context)) {
    return $new_context;
  }
}

/**
 * Settings form for the location relationship.
 *
 * Before we can create a CTools context for the relationship attached to a
 * node, we need to know what kind of location we're looking for. So, the
 * relationship settings form tries to figure out if the site is using
 * location_node or location_cck.
 *
 * If only location_node is enabled, the only thing we have to handle is the
 * case where there are multiple locations attached to nodes, so the only
 * settings form element is the location offset to use (which defaults to 0
 * for the first location in the node).
 *
 * If only location_cck is enabled, we see if there are any Location CCK
 * fields defined on any content types and present all of those as options for
 * the admin to select which Location CCK field should be used for the
 * relationship. Again, to handle multi-valued fields, we need to ask for an
 * offset.
 *
 * If both location_node and location_cck are enabled, the form presents two
 * radio buttons so the user selects which kind of location field they want to
 * use for the relationship, and we use the magic of CTools #dependency to
 * conditionally show/hide the appropriate sub-settings based on the currently
 * selected radio.
 *
 * @param $conf
 *   Array of existing configuration options for this relationship.
 *
 * @return
 *   FormAPI array of settings form elements.
 */
function location_location_from_node_settings_form($conf) {
  ctools_include('dependent');
  $location_type_options = array();
  $offset_options = array();
  for ($i = 0; $i < 10; $i++) {
    $offset_options[$i] = $i;
  }

  if (module_exists('location_node')) {
    $location_type_options['location_node'] = t('Use location directly saved in the node for this relationship');

    $form['location_node_offset'] = array(
      '#title' => t('Node location offset'),
      '#type' => 'select',
      '#options' => $offset_options,
      '#default_value' => isset($conf['location_node_offset']) ? $conf['location_node_offset'] : 0,
      '#description' => t('If the nodes that will be used for this relationship have multiple locations, select which location you want to use.'),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('radio:relationship[relationship_settings][location_type]' => array('location_node')),
      '#weight' => 4,
    );
  }
  if (module_exists('location_cck')) {
    $cck_options = array();
    foreach (content_fields() as $field) {
      if ($field['type'] == 'location') {
        $cck_options[$field['field_name']] = t($field['widget']['label']);
      }
    }
    if (!empty($cck_options)) {
      $form['location_cck_field_name'] = array(
        '#title' => t('Location CCK field to use for this relationship'),
        '#type' => 'select',
        '#options' => $cck_options,
        '#default_value' => isset($conf['location_cck_field_name']) ? $conf['location_cck_field_name'] : '',
        '#process' => array('ctools_dependent_process'),
        '#dependency' => array('radio:relationship[relationship_settings][location_type]' => array('location_cck')),
        '#prefix' => '<div class="clear-block">',
        '#suffix' => '</div>',
        '#weight' => 6,
      );

      $form['location_cck_field_offset'] = array(
        '#title' => t('Location CCK field offset'),
        '#type' => 'select',
        '#options' => $offset_options,
        '#default_value' => isset($conf['location_cck_field_offset']) ? $conf['location_cck_field_offset'] : 0,
        '#description' => t('If the nodes that will be used for this relationship have multiple locations, select which location you want to use.'),
        '#process' => array('ctools_dependent_process'),
        '#dependency' => array('radio:relationship[relationship_settings][location_type]' => array('location_cck')),
        '#weight' => 7,
        '#prefix' => '<div class="clear-block">',
        '#suffix' => '</div>',
      );

      // Since we have valid Location CCK fields on the site, present this as
      // an option for the location relationship.
      $location_type_options['location_cck'] = t('Use location CCK fields for this relationship');
    }
  }
  if (isset($location_type_options)) {
    if (count($location_type_options) > 1) {
      $form['location_type'] = array(
        '#title' => t('Location type'),
        '#type' => 'radios',
        '#options' => $location_type_options,
        '#default_value' => isset($conf['location_type']) ? $conf['location_type'] : '',
        '#weight' => 2,
      );
    }
    else {
      $form['location_type'] = array(
        '#type' => 'value',
        '#value' => reset(array_keys($location_type_options)),
      );
    }
  }

  return $form;
}
