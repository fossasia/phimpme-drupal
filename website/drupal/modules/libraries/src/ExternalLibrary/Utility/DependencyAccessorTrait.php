<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides a trait for classes giving access to a library dependency.
 */
trait DependencyAccessorTrait {

  /**
   * The dependency.
   *
   * @var \Drupal\libraries\ExternalLibrary\LibraryInterface
   */
  protected $dependency;

  /**
   * Returns the dependency.
   *
   * @return \Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The library.
   */
  public function getLibrary() {
    return $this->dependency;
  }

}
