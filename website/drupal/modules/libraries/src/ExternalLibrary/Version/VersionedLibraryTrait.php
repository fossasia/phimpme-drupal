<?php

namespace Drupal\libraries\ExternalLibrary\Version;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException;

/**
 * Provides a trait for versioned libraries.
 *
 * @see \Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface
 */
trait VersionedLibraryTrait {

  /**
   * The library version.
   *
   * @var string
   */
  protected $version;

  /**
   * Information about the version detector to use fo rthis library.
   *
   * Contains the following keys:
   * id: The plugin ID of the version detector.
   * configuration: The plugin configuration of the version detector.
   *
   * @var array
   */
  protected $versionDetector = [
    'id' => NULL,
    'configuration' => [],
  ];

  /**
   * Gets the version of the library.
   *
   * @return string
   *   The version string, for example 1.0, 2.1.4, or 3.0.0-alpha5.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException
   *
   * @see \Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface::getVersion()
   */
  public function getVersion() {
    if (!isset($this->version)) {
      throw new UnknownLibraryVersionException($this);
    }
    return $this->version;
  }

  /**
   * Sets the version of the library.
   *
   * @param string $version
   *   The version of the library.
   *
   * @return $this
   *
   * @see \Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface::setVersion()
   */
  public function setVersion($version) {
    $this->version = (string) $version;
    return $this;
  }

  /**
   * Gets the version detector of this library using the detector factory.
   *
   * Because determining the installation version of a library is not specific
   * to any library or even any library type, this logic is offloaded to
   * separate detector objects.
   *
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $detector_factory
   *
   * @return \Drupal\libraries\ExternalLibrary\Version\VersionDetectorInterface
   *
   * @see \Drupal\libraries\ExternalLibrary\Version\VersionDetectorInterface
   */
  public function getVersionDetector(FactoryInterface $detector_factory) {
    return $detector_factory->createInstance($this->versionDetector['id'], $this->versionDetector['configuration']);
  }

}
