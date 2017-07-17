<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

/**
 * Provides an interface for library definition discoveries that are writable.
 *
 * @see \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface
 * @see \Drupal\libraries\ExternalLibrary\Definition\ChainDefinitionDiscovery
 */
interface WritableDefinitionDiscoveryInterface extends DefinitionDiscoveryInterface {

  /**
   * Writes a library definition persistently.
   *
   * @param string $id
   *   The library ID.
   * @param array $definition
   *   The library definition to write.
   *
   * @return $this
   */
  public function writeDefinition($id, $definition);

}
