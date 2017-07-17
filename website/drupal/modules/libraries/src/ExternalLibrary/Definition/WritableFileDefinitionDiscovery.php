<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

/**
 * Provides a definition discovery based on a writable directory or stream.
 *
 * @see \Drupal\libraries\ExternalLibrary\Definition\FileDefinitionDiscovery
 */
class WritableFileDefinitionDiscovery extends FileDefinitionDiscovery implements WritableDefinitionDiscoveryInterface {

  /**
   * {@inheritdoc}
   */
  public function writeDefinition($id, $definition) {
    file_put_contents($this->getFileUri($id), $this->serializer->encode($definition));
    return $this;
  }

}
