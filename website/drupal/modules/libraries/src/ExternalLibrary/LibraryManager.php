<?php

namespace Drupal\libraries\ExternalLibrary;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryTypeNotFoundException;
use Drupal\libraries\ExternalLibrary\Type\LibraryCreationListenerInterface;
use Drupal\libraries\ExternalLibrary\Type\LibraryLoadingListenerInterface;
use Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface;

/**
 * Provides a manager for external libraries.
 *
 * @todo Dispatch events at various points in the library lifecycle.
 * @todo Automatically load PHP file libraries that are required by modules or
 *   themes.
 */
class LibraryManager implements LibraryManagerInterface {

  /**
   * The library definition discovery.
   *
   * @var \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface
   */
  protected $definitionDiscovery;

  /**
   * The library type factory.
   *
   * @var \Drupal\Component\Plugin\Factory\FactoryInterface
   */
  protected $libraryTypeFactory;

  /**
   * Constructs an external library manager.
   *
   * @param \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface $definition_disovery
   *   The library registry.
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $library_type_factory
   *   The library type factory.
   */
  public function __construct(
    DefinitionDiscoveryInterface $definition_disovery,
    FactoryInterface $library_type_factory
  ) {
    $this->definitionDiscovery = $definition_disovery;
    $this->libraryTypeFactory = $library_type_factory;
  }

  /**
   * {@inheritdoc}
   */
  public function getLibrary($id) {
    $definition = $this->definitionDiscovery->getDefinition($id);
    return $this->getLibraryFromDefinition($id, $definition);
  }

  /**
   * {@inheritdoc}
   */
  public function getRequiredLibraryIds() {
    $library_ids = [];
    foreach (['module', 'theme'] as $type) {
      foreach (system_get_info($type) as $info) {
        if (isset($info['library_dependencies'])) {
          $library_ids = array_merge($library_ids, $info['library_dependencies']);
        }
      }
    }
    return array_unique($library_ids);
  }

  /**
   * {@inheritdoc}
   */
  public function load($id) {
    $definition = $this->definitionDiscovery->getDefinition($id);
    $library_type = $this->getLibraryType($id, $definition);
    // @todo Throw an exception instead of silently failing.
    if ($library_type instanceof LibraryLoadingListenerInterface) {
      $library_type->onLibraryLoad($this->getLibraryFromDefinition($id, $definition));
    }
  }

  /**
   * @param $id
   * @param $definition
   * @return mixed
   */
  protected function getLibraryFromDefinition($id, $definition) {
    $library_type = $this->getLibraryType($id, $definition);

    // @todo Make this alter-able.
    $class = $library_type->getLibraryClass();

    // @todo Make sure that the library class implements the correct interface.
    $library = $class::create($id, $definition, $library_type);

    if ($library_type instanceof LibraryCreationListenerInterface) {
      $library_type->onLibraryCreate($library);
      return $library;
    }
    return $library;
  }

  /**
   * @param string $id
   * @param array $definition
   *
   * @return \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface
   */
  protected function getLibraryType($id, $definition) {
    // @todo Validate that the type is a string.
    if (!isset($definition['type'])) {
      throw new LibraryTypeNotFoundException($id);
    }
    return $this->libraryTypeFactory->createInstance($definition['type']);
  }

}
