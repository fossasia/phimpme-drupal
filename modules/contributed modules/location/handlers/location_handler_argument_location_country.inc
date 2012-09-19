<?php

/**
 * Argument handler to accept a country code or name.
 */
class location_handler_argument_location_country extends views_handler_argument {
  /**
   * Override the behavior of title(). Get the country name.
   */
  function title() {
    return location_country_name($this->argument);
  }

  function summary_name($data) {
    $value = $data->{$this->name_alias};
    if (empty($value)) {
      return t('Unknown');
    }
    return location_country_name($value);
  }
}
