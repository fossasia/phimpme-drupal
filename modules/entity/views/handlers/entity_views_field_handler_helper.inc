<?php

/**
 * @file
 * Contains the EntityFieldHandlerHelper class.
 */

/**
 * Helper class containing static implementations of common field handler methods.
 *
 * Used by the data selection entity field handlers to avoid code duplication.
 *
 * @see entity_views_table_definition()
 */
class EntityFieldHandlerHelper {

  /**
   * Provide appropriate default options for a handler.
   */
  public static function option_definition($handler) {
    if (entity_property_list_extract_type($handler->definition['type'])) {
      $options['list']['contains']['mode'] = array('default' => 'collapse');
      $options['list']['contains']['separator'] = array('default' => ', ');
    }
    $options['link_to_entity'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Provide an appropriate default option form for a handler.
   */
  public static function options_form($handler, &$form, &$form_state) {
    if (entity_property_list_extract_type($handler->definition['type'])) {
      $form['list']['mode'] = array(
        '#type' => 'select',
        '#title' => t('List handling'),
        '#options' => array(
          'collapse' => t('Concatenate values using a seperator'),
          'first' => t('Show first (if present)'),
          'count' => t('Show item count'),
        ),
        '#default_value' => $handler->options['list']['mode'],
      );
      $form['list']['separator'] = array(
        '#type' => 'textfield',
        '#title' => t('List seperator'),
        '#default_value' => $handler->options['list']['separator'],
        '#dependency' => array('edit-options-list-mode' => array('collapse')),
      );
    }
    $form['link_to_entity'] = array(
      '#type' => 'checkbox',
      '#title' => t('Link this field to its entity'),
      '#description' => t("When using this, you should not set any other link on the field."),
      '#default_value' => $handler->options['link_to_entity'],
    );
  }

  /**
   * Add the field for the entity ID (if necessary).
   */
  public static function query($handler) {
    // Copied over from views_handler_field_entity::query().
    // Sets table_alias (entity table), base_field (entity id field) and
    // field_alias (the field's alias).
    $handler->table_alias = $base_table = $handler->view->base_table;
    $handler->base_field = $handler->view->base_field;

    if (!empty($handler->relationship)) {
      foreach ($handler->view->relationship as $relationship) {
        if ($relationship->alias == $handler->relationship) {
          $base_table = $relationship->definition['base'];
          $handler->table_alias = $relationship->alias;

          $table_data = views_fetch_data($base_table);
          $handler->base_field = empty($relationship->definition['base field']) ? $table_data['table']['base']['field'] : $relationship->definition['base field'];
        }
      }
    }

    // Add the field if the query back-end implements an add_field() method,
    // just like the default back-end.
    if (method_exists($handler->query, 'add_field')) {
      $handler->field_alias = $handler->query->add_field($handler->table_alias, $handler->base_field, '');
    }
    else {
      // To ensure there is an alias just set the field alias to the real field.
      $handler->field_alias = $handler->real_field;
    }
  }

  /**
   * Extracts the innermost field name from a data selector.
   *
   * @param $selector
   *   The data selector.
   *
   * @return
   *   The last component of the data selector.
   */
  public static function get_selector_field_name($selector) {
    return ltrim(substr($selector, strrpos($selector, ':')), ':');
  }

  /**
   * Adds a click-sort to the query.
   *
   * @param $order
   *   Either 'ASC' or 'DESC'.
   */
  public static function click_sort($handler, $order) {
    // The normal orderby() method for this usually won't work here. So we need
    // query plugins to provide their own method for this.
    if (method_exists($handler->query, 'add_selector_orderby')) {
      $selector = self::construct_property_selector($handler, TRUE);
      $handler->query->add_selector_orderby($selector, $order);
    }
  }

  /**
   * Load the entities for all rows that are about to be displayed.
   *
   * Automatically takes care of relationships, including data selection
   * relationships. Results are written into @code $handler->wrappers @endcode
   * and @code $handler->entity_type @endcode is set.
   */
  public static function pre_render($handler, &$values, $load_always = FALSE) {
    if (empty($values)) {
      return;
    }
    if (!$load_always && empty($handler->options['link_to_entity'])) {
      // Check whether we even need to load the entities.
      $selector = self::construct_property_selector($handler, TRUE);
      $load = FALSE;
      foreach ($values as $row) {
        if (empty($row->_entity_properties) || !array_key_exists($selector, $row->_entity_properties)) {
          $load = TRUE;
          break;
        }
      }
      if (!$load) {
        return;
      }
    }

    if (method_exists($handler->query, 'get_result_wrappers')) {
      list($handler->entity_type, $handler->wrappers) = $handler->query->get_result_wrappers($values, $handler->relationship, $handler->real_field);
    }
    else {
      list($handler->entity_type, $entities) = $handler->query->get_result_entities($values, $handler->relationship, $handler->real_field);
      $handler->wrappers = array();
      foreach ($entities as $id => $entity) {
        $handler->wrappers[$id] = entity_metadata_wrapper($handler->entity_type, $entity);
      }
    }
  }

  /**
   * Return an Entity API data selector for the given handler's relationship.
   *
   * A data selector is a concatenation of properties which should be followed
   * to arrive at a desired property that may be nested in related entities or
   * structures. The separate properties are herein concatenated with colons.
   *
   * For instance, a data selector of "author:roles" would mean to first
   * access the "author" property of the given wrapper, and then for this new
   * wrapper to access and return the "roles" property.
   *
   * Lists of entities are handled automatically by always returning only the
   * first entity.
   *
   * @param $handler
   *   The handler for which to construct the selector.
   * @param $complete
   *   If TRUE, the complete selector for the field is returned, not just the
   *   one for its parent. Defaults to FALSE.
   *
   * @return
   *   An Entity API data selector for the given handler's relationship.
   */
  public static function construct_property_selector($handler, $complete = FALSE) {
    $return = '';
    if ($handler->relationship) {
      $current_handler = $handler;
      $view = $current_handler->view;
      while (!empty($current_handler->relationship) && !empty($view->relationship[$current_handler->relationship])) {
        $current_handler = $view->relationship[$current_handler->relationship];
        $return = $current_handler->real_field . ($return ? ":$return" : '');
      }
    }

    if ($complete) {
      $return .= ($return ? ':' : '') . $handler->real_field;
    }
    elseif ($pos = strrpos($handler->real_field, ':')) {
      // If we have a selector as the real_field, append this to the returned
      // relationship selector.
      $return .= ($return ? ':' : '') . substr($handler->real_field, 0, $pos);
    }

    return $return;
  }

  /**
   * Extracts data from several metadata wrappers based on a data selector.
   *
   * All metadata wrappers passed to this function have to be based on the exact
   * same property information. The data will be returned wrapped by one or more
   * metadata wrappers.
   *
   * Can be used in query plugins for the get_result_entities() and
   * get_result_wrappers() methods.
   *
   * @param array $wrappers
   *   The EntityMetadataWrapper objects from which to extract data.
   * @param $selector
   *   The selector specifying the data to extract.
   *
   * @return array
   *   An array with numeric indices, containing the type of the extracted
   *   wrappers in the first element. The second element of the array contains
   *   the extracted property value(s) for each wrapper, keyed to the same key
   *   that was used for the respecive wrapper in $wrappers. All extracted
   *   properties are returned as metadata wrappers.
   */
  public static function extract_property_multiple(array $wrappers, $selector) {
    $parts = explode(':', $selector, 2);
    $name = $parts[0];

    $results = array();
    $entities = array();
    $type = '';
    foreach ($wrappers as $i => $wrapper) {
      try {
        $property = $wrapper->$name;
        $type = $property->type();
        if ($property instanceof EntityDrupalWrapper) {
          // Remember the entity IDs to later load all at once (so as to
          // properly utilize multiple load functionality).
          $id = $property->getIdentifier();
          $entities[$type][$i] = $id;
        }
        elseif ($property instanceof EntityStructureWrapper) {
          $results[$i] = $property;
        }
        elseif ($property instanceof EntityListWrapper) {
          foreach ($property as $item) {
            $results[$i] = $item;
            $type = $item->type();
            break;
          }
        }
        // Do nothing in case it cannot be applied.
      }
      catch (EntityMetadataWrapperException $e) {
        // Skip single empty properties.
      }
    }

    if ($entities) {
      // Map back the loaded entities back to the results array.
      foreach ($entities as $type => $id_map) {
        $loaded = entity_load($type, $id_map);
        foreach ($id_map as $i => $id) {
          if (isset($loaded[$id])) {
            $results[$i] = entity_metadata_wrapper($type, $loaded[$id]);
          }
        }
      }
    }

    // If there are no further parts in the selector, we are done now.
    if (empty($parts[1])) {
      return array($type, $results);
    }
    return self::extract_property_multiple($results, $parts[1]);
  }

  /**
   * Get the value of a certain data selector.
   *
   * Uses $values->_entity_properties to look for already extracted properties.
   *
   * @param $handler
   *   The field handler for which to return a value.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   * @param $field
   *   The field to extract. If no value is given, the field of the given
   *   handler is used instead. The special "entity object" value can be used to
   *   get the base entity instead of a special field.
   * @param $default
   *   The value to return if the entity or field are not present.
   */
  public static function get_value($handler, $values, $field = NULL, $default = NULL) {
    // There is a value cache on each handler so parent handlers rendering a
    // single field value from a list will get the single value, not the whole
    // list.
    if (!isset($field) && isset($handler->current_value)) {
      return $handler->current_value;
    }
    $field = isset($field) ? $field : self::get_selector_field_name($handler->real_field);
    $selector = self::construct_property_selector($handler);
    $selector = $selector ? "$selector:$field" : $field;
    if (!isset($values->_entity_properties)) {
      $values->_entity_properties = array();
    }
    if (!array_key_exists($selector, $values->_entity_properties)) {
      if (!isset($handler->wrappers[$handler->view->row_index])) {
        $values->_entity_properties[$selector] = $default;
      }
      elseif (is_array($handler->wrappers[$handler->view->row_index])) {
        $values->_entity_properties[$selector] = self::extract_list_wrapper_values($handler->wrappers[$handler->view->row_index], $field);
      }
      else {
        $wrapper = $handler->wrappers[$handler->view->row_index];
        try {
          if ($field === 'entity object') {
            $values->_entity_properties[$selector] = $wrapper->value();
          }
          else {
            $values->_entity_properties[$selector] = isset($wrapper->$field) ? $wrapper->$field->value(array('identifier' => TRUE)) : $default;
          }
        }
        catch (EntityMetadataWrapperException $e) {
          $values->_entity_properties[$selector] = $default;
        }
      }
    }
    return $values->_entity_properties[$selector];
  }

  /**
   * Helper method for extracting the values from an array of wrappers.
   *
   * Nested arrays of wrappers are also handled, the values are returned in a
   * flat (not nested) array.
   */
  public static function extract_list_wrapper_values(array $wrappers, $field) {
    $return = array();
    foreach ($wrappers as $wrapper) {
      if (is_array($wrapper)) {
        $values = self::extract_list_wrapper_values($wrapper, $field);
        if ($values) {
          $return = array_merge($return, $values);
        }
      }
      else {
        try {
          if ($field == 'entity object') {
            $return[] = $wrapper->value();
          }
          elseif(isset($wrapper->$field)) {
            $return[] = $wrapper->$field->value(array('identifier' => TRUE));
          }
        }
        catch (EntityMetadataWrapperException $e) {
          // An exception probably signifies a non-present property, so we just
          // ignore it.
        }
      }
    }
    return $return;
  }

  /**
   * Render the field.
   *
   * Implements the entity link functionality and list handling. Basic handling
   * of the single values is delegated back to the field handler.
   *
   * @param $handler
   *   The field handler whose field should be rendered.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   *
   * @return
   *   The rendered value for the field.
   */
  public static function render($handler, $values) {
    $value = $handler->get_value($values);
    if (is_array($value)) {
      return self::render_list($handler, $value, $values);
    }
    return self::render_entity_link($handler, $value, $values);
  }

  /**
   * Render a list of values.
   *
   * @param $handler
   *   The field handler whose field is rendered.
   * @param $list
   *   The list of values to render.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   *
   * @return
   *   The rendered value for the given list.
   */
  public static function render_list($handler, $list, $values) {
    // Allow easy overriding of this behaviour in the specific field handler.
    if (method_exists($handler, 'render_list')) {
      return $handler->render_list($list, $values);
    }
    if (isset($handler->options['list']['mode'])) {
      if ($handler->options['list']['mode'] == 'first') {
        $list = count($list) ? array_shift($list) : NULL;
        if (is_array($list)) {
          return self::render_list($handler, $list, $values);
        }
        elseif (isset($list)) {
          return self::render_entity_link($handler, $list, $values);
        }
        return NULL;
      }
      if ($handler->options['list']['mode'] == 'count') {
        return count($list);
      }
    }
    $inner_values = array();
    foreach ($list as $value) {
      $value = is_array($value) ? self::render_list($handler, $value, $values) : self::render_entity_link($handler, $value, $values);
      if ($value) {
        $inner_values[] = $value;
      }
    }
    $separator = isset($handler->options['list']['separator']) ? $handler->options['list']['separator'] : ', ';
    return implode($separator, $inner_values);
  }

  /**
   * Render a single value as a link to the entity if applicable.
   *
   * @param $handler
   *   The field handler whose field is rendered.
   * @param $value
   *   The single value to render.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   *
   * @return
   *   The rendered value.
   */
  public static function render_entity_link($handler, $value, $values) {
    // Allow easy overriding of this behaviour in the specific field handler.
    if (method_exists($handler, 'render_entity_link')) {
      return $handler->render_entity_link($value, $values);
    }
    $render = self::render_single_value($handler, $value, $values);
    if (!$handler->options['link_to_entity']) {
      return $render;
    }
    $entity = $handler->get_value($values, 'entity object');
    if (is_object($entity) && ($url = entity_uri($handler->entity_type, $entity))) {
      return l($render, $url['path'], array('html' => TRUE) + $url['options']);
    }
    return $render;
  }

  /**
   * Render a single value.
   *
   * @param $handler
   *   The field handler whose field is rendered.
   * @param $value
   *   The single value to render.
   * @param $values
   *   The values for the current row retrieved from the Views query, as an
   *   object.
   *
   * @return
   *   The rendered value.
   */
  public static function render_single_value($handler, $value, $values) {
    // Try to use the method in the specific field handler.
    if (method_exists($handler, 'render_single_value')) {
      $handler->current_value = $value;
      $return = $handler->render_single_value($value, $values);
      unset($handler->current_value);
      return $return;
    }
    // Default fallback in case the field handler doesn't provide the method.
    return is_scalar($value) ? check_plain($value) : nl2br(check_plain(print_r($value, TRUE)));
  }

}
