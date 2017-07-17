<?php

namespace Drupal\libraries\ExternalLibrary\Local;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\LibraryInterface;

/**
 * Provides an interface for local libraries.
 *
 * Local libraries are libraries that can be found on the filesystem. If the
 * library files can be found in the filesystem a library is considered
 * installed and its library path can be retrieved.
 *
 * Because determining whether or not the library is available locally is not
 * the responsibility of the library itself, but of a designated locator, this
 * interface declares setter methods, as well.
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
 */
interface LocalLibraryInterface extends LibraryInterface {

  /**
   * Checks whether the library is installed.
   *
   * @return bool
   *   TRUE if the library is installed; FALSE otherwise;
   */
  public function isInstalled();

  /**
   * Marks the library as uninstalled.
   *
   * A corresponding method to mark the library as installed is not provided as
   * an installed library should have a library path, so that
   * LocalLibraryInterface::setLibraryPath() can be used instead.
   *
   * @return $this
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::isInstalled()
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::setLocalPath()
   */
  public function setUninstalled();

  /**
   * Gets the local path to the library.
   *
   * @return string
   *   The absolute path to the library on the filesystem.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::setLocalPath()
   */
  public function getLocalPath();

  /**
   * Sets the local path of the library.
   *
   * @param string $path
   *   The path to the library.
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::getLocalPath()
   */
  public function setLocalPath($path);

  /**
   * Gets the locator of this library using the locator factory.
   *
   * Because determining the installation status and library path of a library
   * is not specific to any library or even any library type, this logic is
   * offloaded to separate locator objects.
   *
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $locator_factory
   *
   * @return \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
   */
  public function getLocator(FactoryInterface $locator_factory);

}
