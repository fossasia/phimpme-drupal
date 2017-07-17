<?php

namespace Drupal\libraries\ExternalLibrary\Dependency;

/**
 * Provides a trait for libraries that depend on other libraries.
 */
trait DependentLibraryTrait {

  /**
   * An array of library IDs of libraries that the library depends on.
   *
   * @return string[]
   */
  protected $dependencies;

  /**
   * Returns the libraries dependencies, if any.
   *
   * @return string[]
   *   An array of library IDs of libraries that the library depends on.
   *
   * @see \Drupal\libraries\ExternalLibrary\Dependency\DependentLibraryInterface::getDependencies()
   */
  public function getDependencies() {
    return $this->dependencies;
  }

}
