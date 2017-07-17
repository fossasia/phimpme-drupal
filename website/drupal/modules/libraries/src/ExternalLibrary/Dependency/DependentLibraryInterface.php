<?php

namespace Drupal\libraries\ExternalLibrary\Dependency;

use Drupal\libraries\ExternalLibrary\LibraryInterface;

/**
 * Provides an interface for libraries that depend on other libraries.
 *
 * @todo Implement versioned dependencies.
 */
interface DependentLibraryInterface extends LibraryInterface {

  /**
   * Returns the libraries dependencies, if any.
   *
   * @return string[]
   *   An array of library IDs of libraries that the library depends on.
   */
  public function getDependencies();

}
