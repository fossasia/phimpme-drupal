<?php

namespace Drupal\libraries\ExternalLibrary\Remote;

use Drupal\libraries\ExternalLibrary\LibraryInterface;

/**
 * Provides an interface for remote libraries.
 *
 * Assuming they declare a remote URL, remote libraries are always loaded. It is
 * not checked whether or not the Drupal site has network access or the remote
 * resource is available.
 */
interface RemoteLibraryInterface extends LibraryInterface {

  /**
   * Checks whether the library has a remote URL.
   *
   * This check allows using the same library class for multiple libraries only
   * some of which are available remotely.
   *
   * @return bool
   *   TRUE if the library has a remote URL; FALSE otherwise.
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryInterface
   */
  public function hasRemoteUrl();

  /**
   * Returns the remote URL of the library.
   *
   * @return string
   *   The remote URL of the library.
   *
   * @todo Consider throwing an exception if hasRemoteUrl() return FALSE.
   */
  public function getRemoteUrl();

}
