<?php

namespace Drupal\libraries\ExternalLibrary;

use Drupal\libraries\ExternalLibrary\Dependency\DependentLibraryInterface;
use Drupal\libraries\ExternalLibrary\Dependency\DependentLibraryTrait;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface;
use Drupal\libraries\ExternalLibrary\Utility\IdAccessorTrait;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryTrait;

/**
 * Provides a base external library implementation.
 */
abstract class LibraryBase implements
  LibraryInterface,
  DependentLibraryInterface,
  VersionedLibraryInterface
{

  use
    IdAccessorTrait,
    DependentLibraryTrait,
    VersionedLibraryTrait
  ;

  /**
   * The library type of this library.
   *
   * @var \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface
   */
  protected $type;

  /**
   * Constructs a library.
   *
   * @param string $id
   *   The library ID.
   * @param array $definition
   *   The library definition array.
   * @param \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface $type
   *   The library type of this library.
   */
  public function __construct($id, array $definition, LibraryTypeInterface $type) {
    $this->id = (string) $id;
    $this->type = $type;
    $this->dependencies = $definition['dependencies'];
    $this->versionDetector = $definition['version_detector'];
  }

  /**
   * {@inheritdoc}
   */
  public static function create($id, array $definition, LibraryTypeInterface $type) {
    static::processDefinition($definition);
    return new static($id, $definition, $type);
  }

  /**
   * Gets library definition defaults.
   *
   * @param array $definition
   *   A library definition array.
   */
  protected static function processDefinition(array &$definition) {
    $definition += [
      'dependencies' => [],
      // @todo This fallback is not very elegant.
      'version_detector' => [
        'id' => 'static',
        'configuration' => ['version' => ''],
      ],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->type;
  }

}
