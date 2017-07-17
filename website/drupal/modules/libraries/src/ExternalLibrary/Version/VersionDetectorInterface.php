<?php

namespace Drupal\libraries\ExternalLibrary\Version;

/**
 * Provides an interface for version detectors.
 *
 * @ingroup libraries
 */
interface VersionDetectorInterface {

  /**
   * Detects the version of a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface $library
   *   The library whose version to detect.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException
   *
   * @todo Provide a mechanism for version detectors to provide a reason for
   *   failing.
   */
  public function detectVersion(VersionedLibraryInterface $library);

}
