<?php

/**
 * @file
 * Longitude field handler.
 */

class location_views_handler_field_longitude extends location_views_handler_field_latitude {

  function render($values) {
    if ($this->options['style'] == 'dms') {
      return theme('location_longitude_dms', array('longitude' => $values->{$this->field_alias}));
    }
    return parent::render($values);
  }
}
