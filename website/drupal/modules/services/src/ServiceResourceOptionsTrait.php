<?php

namespace Drupal\services;

/**
 * Trait \Drupal\services\ServiceResourceOptionsTrait.
 */
trait ServiceResourceOptionsTrait {

  /**
   * Get HTTP authentication options.
   *
   * @return array
   *   An array of HTTP authentication options.
   */
  protected function getAuthOptions() {
    $options = array_keys(
      \Drupal::service('authentication_collector')->getSortedProviders()
    );

    return array_combine($options, $options);
  }

  /**
   * Get HTTP format options.
   *
   * @return array
   *   An array of HTTP serializer format options.
   */
  protected function getFormatOptions() {
    $formats = \Drupal::getContainer()->getParameter('serializer.formats');

    return array_combine($formats, $formats);
  }

}
