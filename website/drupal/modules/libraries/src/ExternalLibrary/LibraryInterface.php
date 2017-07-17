<?php

namespace Drupal\libraries\ExternalLibrary;

use Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface;

/**
 * Provides an interface for different types of external libraries.
 *
 * @ingroup libraries
 */
interface LibraryInterface {

  /**
   * Creates an instance of the library from its definition.
   *
   * @param string $id
   *   The library ID.
   * @param array $definition
   *   The library definition array.
   * @param \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface $type
   *   The library type of this library.
   *
   * @return static
   */
  public static function create($id, array $definition, LibraryTypeInterface $type);

  /**
   * Returns the ID of the library.
   *
   * @return string
   *   The library ID. This must be unique among all known libraries.
   */
  public function getId();

  /**
   * Returns the library type of the library.
   *
   * @return \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface
   *   The library of the library.
   */
  public function getType();

}
