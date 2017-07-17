<?php

namespace Drupal\libraries\ExternalLibrary\Asset;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\Dependency\DependentLibraryInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException;
use Drupal\libraries\ExternalLibrary\LibraryBase;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryTrait;
use Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface;
use Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryTrait;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;

/**
 * Provides a class for a library with multiple attachable asset libraries.
 */
class MultipleAssetLibrary extends LibraryBase implements
  MultipleAssetLibraryInterface,
  VersionedLibraryInterface,
  DependentLibraryInterface,
  LocalLibraryInterface,
  RemoteLibraryInterface
{

  use
    LocalLibraryTrait,
    RemoteLibraryTrait,
    LocalRemoteAssetTrait
  ;

  /**
   * An array of attachable asset libraries.
   */
  protected $libraries = [];

  /**
   * Construct an external library.
   *
   * @param string $id
   *   The library ID.
   * @param array $definition
   *   The library definition array.
   * @param \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface $library_type
   *   The library type of the library.
   */
  public function __construct($id, array $definition, LibraryTypeInterface $library_type) {
    parent::__construct($id, $definition, $library_type);
    $this->remoteUrl = $definition['remote_url'];
    $this->libraries = $definition['libraries'];
  }

  /**
   * {@inheritdoc}
   */
  protected static function processDefinition(array &$definition) {
    parent::processDefinition($definition);
    $definition += [
      'remote_url' => '',
      'libraries' => [],
    ];
    foreach ($definition['libraries'] as &$library) {
      $library += [
        'css' => [],
        'js' => [],
        'dependencies' => [],
      ];
    }
  }

  /**
   * Returns a core library array structure for this library.
   *
   * @param \Drupal\libraries\ExternalLibrary\LibraryManagerInterface $library_manager
   *   The library manager that can be used to fetch dependencies.
   *
   * @return array
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\getAttachableAssetLibraries::getAttachableAssetLibraries()
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\InvalidLibraryDependencyException
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryTypeNotFoundException
   * @throws \Drupal\Component\Plugin\Exception\PluginException
   *
   * @todo Document the return value.
   */
  public function getAttachableAssetLibraries(LibraryManagerInterface $library_manager) {
    if (!$this->canBeAttached()) {
      throw new LibraryNotInstalledException($this);
    }
    $attachable_libraries = [];
    foreach ($this->libraries as $attachable_library_id => $attachable_library) {
      $attachable_libraries[$attachable_library_id] = [
        'version' => $this->getVersion(),
        'css' => $this->processCssAssets($attachable_library['css']),
        'js' => $this->processJsAssets($attachable_library['js']),
        'dependencies' => $attachable_library['dependencies'],
      ];
    }
    return $attachable_libraries;
  }

  /**
   * Gets the locator of this library using the locator factory.
   *
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $locator_factory
   *
   * @return \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface::getLocator()
   */
  public function getLocator(FactoryInterface $locator_factory) {
    // @todo Consider consolidating the stream wrappers used here. For now we
    // allow asset libs to live almost anywhere.
    return $locator_factory->createInstance('chain')
      ->addLocator($locator_factory->createInstance('uri', ['uri' => 'asset://']))
      ->addLocator($locator_factory->createInstance('uri', ['uri' => 'php-file://']));
  }

}
