<?php

namespace Drupal\libraries\ExternalLibrary\Local;
use Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException;

/**
 * Provides a trait for local libraries utilizing a stream wrapper.
 *
 * It assumes that the library files can be accessed using a specified stream
 * wrapper and that the first component of the file URIs are the library IDs.
 * Thus, file URIs are of the form:
 * stream-wrapper-scheme://library-id/path/to/file/within/the/library/filename
 *
 * This trait should only be used by classes implementing LocalLibraryInterface.
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface
 */
trait LocalLibraryTrait {

  /**
   * Whether or not the library is installed.
   *
   * A library being installed means its files can be found on the filesystem.
   *
   * @var bool
   */
  protected $installed = FALSE;

  /**
   * The local path to the library relative to the app root.
   *
   * @var string
   */
  protected $localPath;

  /**
   * Checks whether the library is installed.
   *
   * @return bool
   *   TRUE if the library is installed; FALSE otherwise;
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::isInstalled()
   */
  public function isInstalled() {
    return $this->installed;
  }

  /**
   * Marks the library as uninstalled.
   *
   * A corresponding method to mark the library as installed is not provided as
   * an installed library should have a library path, so that
   * LocalLibraryInterface::setLibraryPath() can be used instead.
   *
   * @return $this
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::setUninstalled()
   */
  public function setUninstalled() {
    $this->installed = FALSE;
    return $this;
  }

  /**
   * Gets the path to the library.
   *
   * @return string
   *   The path to the library relative to the app root.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::getLocalPath()
   */
  public function getLocalPath() {
    if (!$this->isInstalled()) {
      /** @var \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface $this */
      throw new LibraryNotInstalledException($this);
    }

    return $this->localPath;
  }

  /**
   * Sets the library path of the library.
   *
   * @param string $path
   *   The path to the library.
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::getLocalPath()
   */
  public function setLocalPath($path) {
    $this->installed = TRUE;
    $this->localPath = (string) $path;

    assert('$this->localPath !== ""');
    assert('$this->localPath[0] !== "/"');
  }

}
