<?php

namespace Drupal\libraries\ExternalLibrary\Type;

/**
 * Provides an interface for library types.
 */
interface LibraryTypeInterface {

  /**
   * Returns the ID of the library type.
   *
   * @return string
   *   The library type ID.
   */
  public function getId();

  /**
   * Returns the class used for libraries of this type.
   *
   * @return string|\Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The library class for this library type.
   *
   * @todo Consider adding a getLibraryInterface() method, as well.
   */
  public function getLibraryClass();

}
