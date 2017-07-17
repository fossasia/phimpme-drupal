<?php

namespace Drupal\libraries\Plugin;

use Drupal\Component\Plugin\Exception\PluginException;

/**
 * Provides an exception class for missing plugin configuration.
 *
 * The plugin system allows passing arbitrary data to plugins in form of the
 * $configuration array. Some plugins, however, may depend on certain keys to
 * be present in $configuration. This exception class can be used if such keys
 * are missing.
 *
 * @todo Provide accessors for the passed-in information.
 */
class MissingPluginConfigurationException extends PluginException {

  /**
   * Constructs an exception for a missing plugin configuration value.
   *
   * @param string $plugin_id
   *   The plugin ID.
   * @param $plugin_definition
   *   The plugin definition
   * @param array $configuration
   *   The plugin configuration.
   * @param $missing_key
   *   The missing key in the configuration.
   * @param string $message
   *   (optional) The exception message.
   * @param int $code
   *   (optional) The error code.
   * @param \Exception $previous
   *   (optional) The previous exception.
   */
  public function __construct(
    $plugin_id,
    $plugin_definition,
    array $configuration,
    $missing_key,
    $message = '',
    $code = 0,
    \Exception $previous = NULL
  ) {
    $message = $message ?: "The '{$missing_key}' key is missing in the configuration of the '{$plugin_id}' plugin.";
    parent::__construct($message, $code, $previous);
  }

}
