<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides an interface for classes giving access to a library ID.
 */
interface LibraryAccessorIdInterface {

  /**
   * Returns the ID of the library.
   *
   * @return string
   *   The library ID.
   */
  public function getLibraryId();

}
