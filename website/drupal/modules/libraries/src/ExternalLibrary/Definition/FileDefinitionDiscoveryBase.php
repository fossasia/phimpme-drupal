<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException;

/**
 * Provides a base implementation for file-based definition discoveries.
 *
 * This discovery assumes that library files contain the serialized library
 * definition and are accessible under a common base URI. The expected library
 * file URI will be constructed from this by appending '/$id.$extension' to
 * this, where $id is the library ID and $extension is the serializer extension.
 */
abstract class FileDefinitionDiscoveryBase implements DefinitionDiscoveryInterface {

  /**
   * The serializer for the library definition files.
   *
   * @var \Drupal\Component\Serialization\SerializationInterface
   */
  protected $serializer;

  /**
   * The base URI for the library files.
   *
   * @var string
   */
  protected $baseUri;

  /**
   * Constructs a stream-based library definition discovery.
   *
   * @param \Drupal\Component\Serialization\SerializationInterface $serializer
   *   The serializer for the library definition files.
   * @param string $base_uri
   *   The base URI for the library files.
   */
  public function __construct(SerializationInterface $serializer, $base_uri) {
    $this->serializer = $serializer;
    $this->baseUri = $base_uri;
  }

  /**
   * {@inheritdoc}
   */
  public function getDefinition($id) {
    if (!$this->hasDefinition($id)) {
      throw new LibraryDefinitionNotFoundException($id);
    }
    return $this->serializer->decode($this->getSerializedDefinition($id));
  }

  /**
   * Gets the contents of the library file.
   *
   * @param $id
   *   The library ID to retrieve the serialized definition for.
   *
   * @return string
   *   The serialized library definition.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException
   */
  abstract protected function getSerializedDefinition($id);

  /**
   * Returns the file URI of the library definition file for a given library ID.
   *
   * @param $id
   *   The ID of the external library.
   *
   * @return string
   *   The file URI of the file the library definition resides in.
   */
  protected function getFileUri($id) {
    $filename = $id . '.' . $this->serializer->getFileExtension();
    return "$this->baseUri/$filename";
  }

}
