<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides a trait for classes giving access to a library ID.
 */
trait LibraryIdAccessorTrait {

  /**
   * The ID of the library.
   *
   * @var string
   */
  protected $libraryId;

  /**
   * Returns the ID of the library.
   *
   * @return string
   *   The library ID.
   */
  public function getLibraryId() {
    return $this->libraryId;
  }

}
