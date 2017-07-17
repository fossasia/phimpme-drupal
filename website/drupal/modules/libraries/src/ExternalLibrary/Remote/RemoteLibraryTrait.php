<?php

namespace Drupal\libraries\ExternalLibrary\Remote;

/**
 * Provides a trait for remote libraries.
 */
trait RemoteLibraryTrait {

  /**
   * The remote library URL.
   *
   * @var string
   */
  protected $remoteUrl;

  /**
   * Checks whether the library has a remote URL.
   *
   * This check allows using the same library class for multiple libraries only
   * some of which are available remotely.
   *
   * @return bool
   *   TRUE if the library has a remote URL; FALSE otherwise.
   *
   * @see \Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface::hasRemoteUrl()
   */
  public function hasRemoteUrl() {
    return !empty($this->remoteUrl);
  }

  /**
   * Returns the remote URL of the library.
   *
   * @return string
   *   The remote URL of the library.
   *
   * \Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface::getRemoteUrl()
   */
  public function getRemoteUrl() {
    return $this->remoteUrl;
  }

}
