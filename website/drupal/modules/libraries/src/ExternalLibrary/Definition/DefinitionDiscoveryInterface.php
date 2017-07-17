<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

/**
 * Provides an interface for library definition discoveries.
 *
 * This is similar to the plugin system's DiscoveryInterface, except that this
 * does not require knowing all definitions upfront, so there is no
 * getDefinitions() method.
 *
 * @see \Drupal\Component\Plugin\Discovery\DiscoveryInterface
 *
 * @ingroup libraries
 */
interface DefinitionDiscoveryInterface {

  /**
   * Checks whether a library definition exists.
   *
   * @param string $id
   *   The library ID.
   *
   * @return bool
   *   TRUE if a library definition with the given ID exists; FALSE otherwise.
   */
  public function hasDefinition($id);

  /**
   * Gets a library definition by its ID.
   *
   * @param string $id
   *   The library ID.
   *
   * @return array
   *   The library definition.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException
   *
   * @todo Consider returning a classed object instead of an array or at least
   *   document and validate the array structure.
   */
  public function getDefinition($id);

}
