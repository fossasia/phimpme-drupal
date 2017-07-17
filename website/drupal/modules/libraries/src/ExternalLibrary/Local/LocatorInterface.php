<?php

namespace Drupal\libraries\ExternalLibrary\Local;

/**
 * Provides an interface for library locators.
 *
 * Because determining the installation status and library path of a library
 * is not specific to any library or even any library type, this logic can be
 * implemented generically in form of a locator.
 */
interface LocatorInterface {

  /**
   * Locates a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface $library
   *   The library to locate.
   */
  public function locate(LocalLibraryInterface $library);

}
