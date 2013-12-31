<?php

/**
 * @file
 *   Allows displaying dates in FBSS's custom themed format.
 */

/**
 * A handler for the "Created time" field.
 */
class statuses_views_handler_field_created extends views_handler_field_date {
  function option_definition() {
    $options = parent::option_definition();
    $options['date_format'] = array('default' => 'themed');
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['date_format']['#options']['themed'] = t('Themed (changes based on recency)');
    $form['date_format']['#default_value'] = isset($this->options['date_format']) ? $this->options['date_format'] : 'themed';
  }
  function render($values) {
    $value = $values->{$this->field_alias};
    $format = $this->options['date_format'];
    $custom_format = '';
    $output = '<div class="statuses-time">';
    if (in_array($format, array('custom', 'raw time ago', 'time ago', 'raw time span', 'time span'))) {
      $custom_format = $this->options['custom_date_format'];
    }
    if (!$value) {
      $output .= theme('views_nodate');
    }
    else {
      $time_diff = REQUEST_TIME - $value; // will be positive for a datetime in the past (ago), and negative for a datetime in the future (hence)
      switch ($format) {
        case 'raw time ago':
          $output .= format_interval($time_diff, is_numeric($custom_format) ? $custom_format : 2);
          break;
        case 'time ago':
          $output .= t('%time ago', array('%time' => format_interval($time_diff, is_numeric($custom_format) ? $custom_format : 2)));
          break;
        case 'raw time span':
          $output .= ($time_diff < 0 ? '-' : '') . format_interval(abs($time_diff), is_numeric($custom_format) ? $custom_format : 2);
          break;
        case 'time span':
          $args = array('%time' => format_interval(abs($time_diff), is_numeric($custom_format) ? $custom_format : 2));
          if ($time_diff < 0) {
            $output .= t('%time hence', $args);
            break;
          }
          $output .= t('%time ago', $args);
          break;
        case 'custom':
          $output .= format_date($value, $format, $custom_format);
          break;
        case 'themed':
          $output .= theme('statuses_time', array('time' => $value));
          break;
        default:
          $output .= format_date($value, $format);
          break;
      }
    }
    return $output . '</div>';
  }
}
