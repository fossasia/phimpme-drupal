<?php

namespace Drupal\libraries\ExternalLibrary\Utility;

/**
 * Provides a trait for classes that have a string identifier.
 */
trait IdAccessorTrait {

  /**
   * The ID.
   *
   * @var string
   */
  protected $id;

  /**
   * Returns the ID.
   *
   * @return string
   *   The ID.
   *
   * @see \Drupal\libraries\ExternalLibrary\LibraryInterface::getId()
   * @see \Drupal\libraries\ExternalLibrary\LibraryType\LibraryTypeInterface::getId()
   */
  public function getId() {
    return $this->id;
  }

}
