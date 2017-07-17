<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides an interface for classes giving access to a library.
 */
interface LibraryAccessorInterface {

  /**
   * Returns the library.
   *
   * @return \Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The library.
   */
  public function getLibrary();

}
