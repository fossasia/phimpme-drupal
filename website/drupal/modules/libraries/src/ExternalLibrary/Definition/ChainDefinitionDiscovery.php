<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

use Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException;

/**
 * Provides a definition discovery that checks a list of other discoveries.
 *
 * The discoveries are checked sequentially. If the definition was not present
 * in some discoveries but is found in a later discovery the definition will be
 * written to the earlier discoveries if they implement
 * WritableDefinitionDiscoveryInterface.
 *
 * @see \Drupal\libraries\ExternalLibrary\Definition\WritableDefinitionDiscoveryInterface
 */
class ChainDefinitionDiscovery implements DefinitionDiscoveryInterface {

  /**
   * The list of definition discoveries that will be checked.
   *
   * @var \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface[]
   */
  protected $discoveries = [];

  /**
   * {@inheritdoc}
   */
  public function hasDefinition($id) {
    foreach ($this->discoveries as $discovery) {
      if ($discovery->hasDefinition($id)) {
        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function getDefinition($id) {
    /** @var \Drupal\libraries\ExternalLibrary\Definition\WritableDefinitionDiscoveryInterface[] $discoveries_to_write */
    $discoveries_to_write = [];
    foreach ($this->discoveries as $discovery) {
      if ($discovery->hasDefinition($id)) {
        $definition = $discovery->getDefinition($id);
        break;
      }
      elseif ($discovery instanceof WritableDefinitionDiscoveryInterface) {
        $discoveries_to_write[] = $discovery;
      }
    }

    if (!isset($definition)) {
      throw new LibraryDefinitionNotFoundException($id);
    }

    foreach ($discoveries_to_write as $discovery_to_write) {
      $discovery_to_write->writeDefinition($id, $definition);
    }

    return $definition;
  }

  /**
   * Adds a definition discovery to the list to check.
   *
   * @param \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface $discovery
   *   The definition discovery to add.
   *
   * @return $this
   */
  public function addDiscovery(DefinitionDiscoveryInterface $discovery) {
    $this->discoveries[] = $discovery;
    return $this;
  }

}
