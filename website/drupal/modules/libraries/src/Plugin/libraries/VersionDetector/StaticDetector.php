<?php

namespace Drupal\libraries\Plugin\libraries\VersionDetector;

use Drupal\Core\Plugin\PluginBase;
use Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException;
use Drupal\libraries\ExternalLibrary\Version\VersionDetectorInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;

/**
 * Detects the version by returning a static string.
 *
 * As this does not perform any actual detection and, thus, circumvents any
 * negotiation of versions by Libraries API it should only be used for testing
 * or when the version of a library cannot be determined from the source code
 * itself.
 *
 * @VersionDetector("static")
 */
class StaticDetector extends PluginBase implements VersionDetectorInterface {

  /**
   * Constructs a static version detector.
   *
   * @param array $configuration
   * @param string $plugin_id
   * @param array $plugin_definition
   */
  public function __construct(array $configuration, $plugin_id, array $plugin_definition) {
    $configuration += [
      'version' => NULL,
    ];
    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * {@inheritdoc}
   */
  public function detectVersion(VersionedLibraryInterface $library) {
    if (!isset($this->configuration['version'])) {
      throw new UnknownLibraryVersionException($library);
    }
    $library->setVersion($this->configuration['version']);
  }

}
