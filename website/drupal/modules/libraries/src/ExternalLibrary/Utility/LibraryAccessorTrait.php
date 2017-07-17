<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides a trait for classes giving access to a library.
 */
trait LibraryAccessorTrait {

  /**
   * The library.
   *
   * @var \Drupal\libraries\ExternalLibrary\LibraryInterface
   */
  protected $library;

  /**
   * Returns the library.
   *
   * @return \Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The library.
   */
  public function getLibrary() {
    return $this->library;
  }

}
