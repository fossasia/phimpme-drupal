<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException;

/**
 * Provides a libraries definition discovery using PHP's native file functions.
 *
 * It supports either a URI with a stream wrapper, an absolute file path or a
 * file path relative to the Drupal root as a base URI.
 *
 * By default YAML files are used.
 *
 * @see \Drupal\libraries\StreamWrapper\LibraryDefinitionsStream
 *
 * @ingroup libraries
 */
class FileDefinitionDiscovery extends FileDefinitionDiscoveryBase implements DefinitionDiscoveryInterface {

  /**
   * {@inheritdoc}
   */
  public function hasDefinition($id) {
    return file_exists($this->getFileUri($id));
  }

  /**
   * {@inheritdoc}
   */
  protected function getSerializedDefinition($id) {
    return file_get_contents($this->getFileUri($id));
  }

}
